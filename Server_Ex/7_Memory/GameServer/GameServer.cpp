// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Lock.h"
#include "Memory.h"
#include "Allocator.h"

class Knight
{
public:
	int32 _hp = rand() % 100;
};

class Monster
{
public:
	int64 _id = 0;
};
int main()
{
	//Knight* k = ObjectPool<Knight>::Pop();

	//ObjectPool<Knight>::Push(k);

	//위에서는 보다시피 포인터 관리를 알아서 해야됨..
	//shared_ptr 사용하면? 일반 new, delete를 사용해서 다른걸 사용
	// shared_ptr<Knight> sptr ={ ObjectPool<Knight>::Pop(), ObjectPool<Knight>::Push };
	//shared_ptr<Knight> sptr = ObjectPool<Knight>::MakeShared();
	// 위 방법보다 이방법으로 사용

	Knight* knights[100];

	for (int32 i = 0; i < 100; i++)
		knights[i] = ObjectPool<Knight>::Pop();

	for (int32 i = 0; i < 100; i++)
	{
		ObjectPool<Knight>::Push(knights[i]);
		knights[i] = nullptr;
	}

	shared_ptr<Knight> sptr = ObjectPool<Knight>::MakeShared();//오브젝트풀
	shared_ptr<Knight> sptr2 = MakeShared<Knight>();//얘는 메모리풀

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();

					cout << knight->_hp << endl;

					this_thread::sleep_for(10ms);

					xdelete(knight);
				}
			});
	}

	GThreadManager->Join();
}




