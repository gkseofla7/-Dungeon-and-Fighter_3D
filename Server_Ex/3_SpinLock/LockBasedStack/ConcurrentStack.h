#pragma once

#include <iostream>
#include <stack>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
using namespace std;
template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();

	}
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
	}

	//bool Empty()//멀티스레드의 경우에는 empty 다음에 팝할때 그 사이에 다른 쓰레드가 낄수 있어서 사실상 별 의미가 없음;;
	//{
	//	lock_guard<mutex> lock(_mutex);
	//	return _stack.empty();
	//}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& Value) : data(Value), next(nullptr)
		{


		}
		T data;
		Node* next;
	};
public:

	// 1)  새 노드를 만듬
	// 2) 새 노드의 next = head
	// 3) head = 새 노드
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		//if (_head == node->next)
		//{
		//	_head = node;
		//	return true;
		//}
		//else
		//{
		//	node->next = _head;
		//	return false;
		//}
		while (_head.compare_exchange_weak(node->next, node) == false)
		{

		}
		//_head = node;
	}

	// 1) head 읽기
	// 2) head->next 읽기
	// 3) head = head->next
	// 4) data 추출해서 반환
	// 5) 추출한 노드를 삭제
	bool TryPop(T& value)
	{
		++_popCount;
		Node* oldHead = _head;

		//if (_head == oldHead)
		//{
		//	_head = oldHead->next;
		//	return true;
		//}
		//else
		//{
		//	oldHead = _head;
		//	return false;
		//}

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next)== false)
		{

		}
		if (oldHead == nullptr)
		{
			--_popCount;
			return false;
		}
		value = oldHead->data;
		TryDelete(oldHead);
		//delete oldHead;
		return true;
	}
	// 1) 데이터 분리
	// 2) Count 체크
	// 3) 나 혼자면 삭제
	void TryDelete(Node* oldHead)
	{
		//나 외에 누가 있는가?
		if (_popCount == 1)
		{
			//나 혼자
			//여기서 또 다른 스레드가 개입할 수는 있겠지만 이미 head는 다른걸 가르키고 있어서 상관X
			delete oldHead;
		}
		else
		{

		}
	}
private:

	atomic<Node*> _head;

	atomic<unsigned __int32> _popCount = 0; // Pop을 실행중인 쓰레드 개수
	atomic<Node*> _pendingList; // 삭제 되어야 할 노드들 (첫번째 노드)
};