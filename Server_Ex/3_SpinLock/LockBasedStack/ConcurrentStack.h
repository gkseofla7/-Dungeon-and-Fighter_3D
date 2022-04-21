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

	//bool Empty()//��Ƽ�������� ��쿡�� empty ������ ���Ҷ� �� ���̿� �ٸ� �����尡 ���� �־ ��ǻ� �� �ǹ̰� ����;;
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

	// 1)  �� ��带 ����
	// 2) �� ����� next = head
	// 3) head = �� ���
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

	// 1) head �б�
	// 2) head->next �б�
	// 3) head = head->next
	// 4) data �����ؼ� ��ȯ
	// 5) ������ ��带 ����
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
	// 1) ������ �и�
	// 2) Count üũ
	// 3) �� ȥ�ڸ� ����
	void TryDelete(Node* oldHead)
	{
		//�� �ܿ� ���� �ִ°�?
		if (_popCount == 1)
		{
			//�� ȥ��
			//���⼭ �� �ٸ� �����尡 ������ ���� �ְ����� �̹� head�� �ٸ��� ����Ű�� �־ ���X
			delete oldHead;
		}
		else
		{

		}
	}
private:

	atomic<Node*> _head;

	atomic<unsigned __int32> _popCount = 0; // Pop�� �������� ������ ����
	atomic<Node*> _pendingList; // ���� �Ǿ�� �� ���� (ù��° ���)
};