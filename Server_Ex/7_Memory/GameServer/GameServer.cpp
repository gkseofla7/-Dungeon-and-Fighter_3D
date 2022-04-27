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
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) :_hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}


	int32 _hp = 100;
	int32 _mp = 10;;
};



int main()
{
	//다양한 STL Container 사용, 여기 내부에서는 new, delete를 사용하고 있음
	//Vector<Knight> v(100);

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Knight> v(10);

					Map<int32, Knight> m;

					m[100] = Knight();
					this_thread::sleep_for(10ms);
				}
			}
		);
	}

	GThreadManager->Join();
}

//STomp Allocator
////SYSTEM_INFO info;
////::GetSystemInfo(&info);
////
////info.dwPageSize;//paging size 가져옴, 아마 4kB
////info.dwAllocationGranularity;//메모리를 할당할때 이 숫자의 배수로 주소를 할당해줌, 64kB
//
////int * test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);//처음인자 알아서 메모리 할장해줘, 4바이트, 예약과 할당을 같이해서 바로 사용가능
////	//Read Write, new연산자 사용안하고 메모리 할당, delete는 효율성을 위해 바로 날리지 않음
////*test = 100;
////::VirtualFree(test, 0, MEM_RELEASE);
//////왜 new, delete 연산자 안쓰고? 크래시남!
//Knight* knight = xnew<Knight>(10);
//
//xdelete(knight);
//
//knight->_hp = 100;


