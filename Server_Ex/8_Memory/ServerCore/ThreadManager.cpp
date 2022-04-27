#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)//function : Ŭ���� ��ü�� �Լ� ���� ��� ������ �� ����
{//std::function<��ȯ Ÿ�� (����)>
	LockGuard guard(_lock);

	_threads.push_back(thread([=]()//[�Խ���] (����) -> ��ȯŸ�� {�Լ��� ����}
		{
			InitTLS();
			callback();
			DestroyTLS();
		}
	));//[&] �ܺ��� ��� �������� ���۷����� call by reference, [=] ��� ���� call by value
	//[=, &x, &y] : x, y�� ���۷����� ������ , [&, x, y]
	// Ŭ���� ��ü�� ó�� �����ɶ� �������� ���� ĸ����, ������ ĸ���Ҷ� �� �����鿡 �ڵ����� const �Ӽ��� ����
	//�ٲٰ� ������ mutable �ٿ��ָ��
}

void ThreadManager::Join()
{
	for (std::thread& t : _threads)
	{
		if (t.joinable())//thread�� Ȱ��ȭ�� ���������� Ȯ��
		{
			t.join();
		}

	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}
