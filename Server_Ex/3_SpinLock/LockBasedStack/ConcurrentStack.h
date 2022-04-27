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

			// �̿� ȥ���ΰ�, ���� ����� �ٸ� �����͵鵵 ����
			Node* node = _pendingList.exchange(nullptr);
			//���� �׳� list �� ��°�� ��������
			if (--_popCount == 0)//popCount��ü�� atomic�ؼ� --�� atomic�ϰ� �����
			{
				// ����� �ְ� ���� -> ���� ����
				// �����ͼ� �����, ������ �����ʹ� �и��ص� ����!
				DeleteNodes(node);
			}
			else if(node)
			{

				// ���� ���������� �ٽ� ���� ����
				ChainPendingNodeList(node);

			}
			// �� ������ ����
			delete oldHead;
		}
		else
		{
			// ���� �ֳ�? �׷� ���� �������� �ʰ�, ���� ���ุ
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

	atomic<unsigned __int32> _popCount = 0; // Pop�� �������� ������ ����
	atomic<Node*> _pendingList; // ���� �Ǿ�� �� ���� (ù��° ���)
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

		while (std::atomic_compare_exchange_weak(&_head, &node->next, node)== false)// _head.compare_exchange_weak(node->next, node)�̰ſ� ���� ���
		{

		}
	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);//shared_ptr�� �����ö� reference count�� �����ϰ� �����ϰ� �� �ϱ� ������ ���������� �Ͼ�� �ʾƼ�
		
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
		//�ڱ� ���� �޸𸮿� ����� �ִ°Ŷ� ��������
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
			//������ ȹ�� (externalCount�� �� ���� ���� +1 �� �ְ� �̱�)
			IncreaseHeadCount(oldHead);
			// �ּ��� externalCount >= 2 ���״� ����X (dkswjsgkrp wjqrmsgkf tn dlTsms)

			Node* ptr = oldHead.ptr;

			// ������ ����
			if (ptr == nullptr)
				return shared_ptr<T>();


			// ������ ȹ�� (ptr->next�� head�� �ٲ�ġ�� �� �ְ� �̱�)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);//���� ������ �ٲ�

				// external : 1 -> 2(��+1) -> 4(��+1 ��+2)
				// internal : 0

				// �� ���� �� ���� �ִ°�?
				const __int32 countIncrease = oldHead.externalCount - 2;
				//internalCount ->2
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)//fetch_add�� ��� ���ϴµ� return�� ������
					delete ptr;

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)//�����鼭 internalcount �ϳ��� �ٿ���
			{
				//�������� �������, �������� ���� -> �޼����� ���� �Ѵ�.
				delete ptr;
			}
		}
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)//counter�� 1 ����
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