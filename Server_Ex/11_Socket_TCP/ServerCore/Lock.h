#pragma once

/*
* ǥ�� mutex�� ��� ��������� lock�� ���� �� ����
* �ٸ� ������ ��ȣ ��Ÿ���� Ư���� ���� �ʿ�
* ��� �����Ͱ� ������ �ʴ´ٸ� lock�� �� �ʿ䰡 ����
* ���� ����� ������ ���ϴ� �������� ����ȭ, ǥ�� lock�� ����� �� �� ������ ���λ��
*/
#include "Types.h"
//���ӿ����� ���� ��ø� �����͸� �����ߴٰ� ���ִ� ��찡 ���Ƽ� SpinLock����
// 32��Ʈ�� ���� 16��Ʈ�� ���� 16��Ʈ�� �ǹ̰� �ٸ�,
// [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
// W : WriteFlag (Exclusive Lock Owner ThreadID)
// R : ReadFlag (Shared Lock Count)
//RW SpinLock
//������ ������ ����
// W�� ���� ���¿��� R��°� OK
// R�� ���� ���¿��� W��°� �ȵ�
/*
* �� W -> W (O)
* W -> R (O)
* R -> W (X)
*/
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK =10000, //�ִ�� ��ٷ���
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,//���� 16��Ʈ�� �̾ƿ��� ���� ����ũ
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};
public:
	void WriteLock(const char* name);
	void WriteUnLock(const char* name);
	void ReadLock(const char* name);
	void ReadUnLock(const char* name);
private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;//lock�� �����ָ� ���������� ����ϱ� ������ lock �ʿ� ����
private:
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnLock(_name); }
private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnLock(_name); }
private:
	Lock& _lock;
	const char* _name;
};