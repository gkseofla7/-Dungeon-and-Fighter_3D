#pragma once

/*
* 표준 mutex의 경우 재귀적으로 lock을 잡을 수 없음
* 다른 문제는 상호 배타적인 특성이 가끔 필요
* 사실 데이터가 변하지 않는다면 lock을 걸 필요가 없음
* 새로 만드는 이유는 원하는 방향으로 최적화, 표준 lock을 사용할 수 도 있지만 공부삼아
*/
#include "Types.h"
//게임에서는 서로 잠시만 데이터를 참조했다가 놔주는 경우가 많아서 SpinLock으로
// 32비트중 상위 16비트와 하위 16비트의 의미가 다름,
// [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
// W : WriteFlag (Exclusive Lock Owner ThreadID)
// R : ReadFlag (Shared Lock Count)
//RW SpinLock
//동일한 스레드 기준
// W를 잡은 상태에서 R잡는거 OK
// R를 잡은 상태에서 W잡는거 안됨
/*
* 즉 W -> W (O)
* W -> R (O)
* R -> W (X)
*/
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK =10000, //최대로 기다려줄
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,//상위 16비트만 뽑아오기 위한 마스크
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};
public:
	void WriteLock();
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();
private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;//lock을 잡은애만 독단적으로 사용하기 때문에 lock 필요 없음
private:
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnLock(); }
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnLock(); }
private:
	Lock& _lock;
};