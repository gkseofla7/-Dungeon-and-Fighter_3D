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

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
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

			// 이왕 혼자인거, 삭제 예약된 다른 데이터들도 삭제
			Node* node = _pendingList.exchange(nullptr);
			//아주 그냥 list 를 통째로 보내버림
			if (--_popCount == 0)//popCount자체가 atomic해서 --는 atomic하게 실행됨
			{
				// 끼어든 애가 없음 -> 삭제 진행
				// 이제와서 끼어들어도, 어차피 데이터는 분리해둔 상태!
				DeleteNodes(node);
			}
			else if(node)
			{

				// 누가 끼어들었으니 다시 갖다 놓음
				ChainPendingNodeList(node);

			}
			// 내 데이터 삭제
			delete oldHead;
		}
		else
		{
			// 누가 있네? 그럼 지금 삭제하지 않고, 삭제 예약만
			ChainPendingNode(oldHead);
			--_popCount;
		}
	}
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{

		}
	}

	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}
	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}
private:

	atomic<Node*> _head;

	atomic<unsigned __int32> _popCount = 0; // Pop을 실행중인 쓰레드 개수
	atomic<Node*> _pendingList; // 삭제 되어야 할 노드들 (첫번째 노드)
};


template<typename T>
class LockFreeStack_2
{
	struct Node
	{
		Node(const T& Value) : data(make_shared<T>(Value)), next(nullptr)
		{


		}
		shared_ptr<T> data;
		shared_ptr<Node> next;
	};
public:


	void Push(const T& value)
	{
		shared_ptr<Node> node = make_shared<Node>(value);
		node->next = std::atomic_load(&_head);

		while (std::atomic_compare_exchange_weak(&_head, &node->next, node)== false)// _head.compare_exchange_weak(node->next, node)이거와 거의 비슷
		{

		}
	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);//shared_ptr은 꺼내올때 reference count가 증가하고 감소하고 등 하기 때문에 원자적으로 일어나지 않아서
		
		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
		{

		}

		if (oldHead == nullptr)
			return shared_ptr<T>();

		return oldHead->data;
	}

	
private:

	shared_ptr<Node> _head;

};


template<typename T>
class LockFreeStack_3
{
	struct Node;

	struct CountedNodePtr
	{
		__int32 externalCount = 0;
		Node* ptr = nullptr;
	};
	struct Node
	{
		Node(const T& Value) : data(make_shared<T>(Value))
		{

		}
		shared_ptr<T> data;
		atomic<__int32> internalCount = 0;
		CountedNodePtr next;
	};
public:


	void Push(const T& value)
	{
		//자기 스택 메모리에 만들어 주는거라 문제없음
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;

		// [!]
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{
		}


	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			//참조권 획득 (externalCount를 현 시점 기준 +1 한 애가 이김)
			IncreaseHeadCount(oldHead);
			// 최소한 externalCount >= 2 일테니 삭제X (dkswjsgkrp wjqrmsgkf tn dlTsms)

			Node* ptr = oldHead.ptr;

			// 데이터 없음
			if (ptr == nullptr)
				return shared_ptr<T>();


			// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);//둘이 데이터 바꿈

				// external : 1 -> 2(나+1) -> 4(나+1 남+2)
				// internal : 0

				// 나 말고 또 누가 있는가?
				const __int32 countIncrease = oldHead.externalCount - 2;
				//internalCount ->2
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)//fetch_add의 경우 더하는데 return은 이전값
					delete ptr;

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)//나가면서 internalcount 하나씩 줄여줌
			{
				//참조권은 얻었으나, 소유권은 실패 -> 뒷수습은 내가 한다.
				delete ptr;
			}
		}
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)//counter에 1 증가
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				break;
			}
		}
	}

private:

	atomic<CountedNodePtr> _head;

};