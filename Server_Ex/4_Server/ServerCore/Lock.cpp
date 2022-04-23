#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
void Lock::WriteLock()
{
	//동일한 쓰레드가 소유하고 있다면 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}
	// 아무도 소유(Write) 및 공유(Read)하고 있지 않을 때, 즉_lockFlag가 0, 경합해서 소유권을 얻음
	//if (_lockFlag == EMPTY_FLAG)
	//{//체크하는 순간에 끼어들 수 있음
	//	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	//	_lockFlag = desired;
	//}
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; 0 < spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))//OUT은 그냥 OUTPUT 값으로 나오기 때문에 붙여줌
			{
				_writeCount++;//writeCount를 따로 관리하고 있는 이유는
				//lock을 재귀적으로 호출하는걸 허락해서
				return;
			}
		}
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");


		this_thread::yield();//잠시 쉬고, 즉 컨텍스트 스위칭
	}
}

void Lock::WriteUnLock()
{
	//ReadLock을 다 풀기 전에는 WriteUnlock 불가능
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)//누군가 읽고있음
		CRASH("INVALID_UNLOCK_ORDER");


	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock()
{
	//동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}
	//아무도 소유(Write)하고 있지 않을 때 경합해서 공유 카운트를 올림
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//누군가가 Write하고 있던가 누가 새치기해서 읽고있으면 다시 돌아감
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");


		this_thread::yield();//잠시 쉬고, 즉 컨텍스트 스위칭
	}
}

void Lock::ReadUnLock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
