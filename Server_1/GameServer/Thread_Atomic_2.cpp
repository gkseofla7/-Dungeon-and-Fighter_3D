// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
//atomic : All-Or-Nothing

//DB
// A라는 유저 인벤에서 집행검을 빼고
// B라는 유저 인벤에 집행검 추가 이런걸 한번에 진행해야됨
atomic<int32> sum = 0;

void Add()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum++;

		
	}
}
void Sub()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum--;
	}
}
int main()
{
	Add();
	Sub();
	cout << sum << endl;

	std::thread t1(Add);
	std::thread t2(Sub);
	t1.join();
	t2.join();

	cout << sum << endl;

}
