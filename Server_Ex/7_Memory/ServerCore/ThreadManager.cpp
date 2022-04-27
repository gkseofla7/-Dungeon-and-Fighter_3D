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

void ThreadManager::Launch(function<void(void)> callback)//function : 클로저 객체나 함수 등을 모두 보관할 수 있음
{//std::function<반환 타입 (인자)>
	LockGuard guard(_lock);

	_threads.push_back(thread([=]()//[게시자] (인자) -> 반환타입 {함수의 몸통}
		{
			InitTLS();
			callback();
			DestroyTLS();
		}
	));//[&] 외부의 모든 변수들을 래퍼런스로 call by reference, [=] 모든 변수 call by value
	//[=, &x, &y] : x, y는 래퍼런스로 가져옴 , [&, x, y]
	// 클로저 객체가 처음 생성될때 변수들의 값을 캡쳐함, 값으로 캡쳐할때 그 변수들에 자동으로 const 속성이 붙음
	//바꾸고 싶으면 mutable 붙여주면됨
}

void ThreadManager::Join()
{
	for (std::thread& t : _threads)
	{
		if (t.joinable())//thread가 활성화된 스레드인지 확인
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
