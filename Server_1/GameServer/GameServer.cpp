// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}
int main()
{
	//사실 이런애들은 System Call 이라고(OS 커널 요청) 운영체제가 요청받은걸 출력해줌, 즉 스레드 만드는것도 운영체제가 해줌
	//cout << "Hello World" << endl;

	//::CreateThread()//이건 너무 윈도우..
	//std::thread t(HelloThread);
	std::thread t;
	auto id1 = t.get_id();
	t = std::thread(HelloThread);
	int32 count = t.hardware_concurrency();//CPU 코어 개수 몇개?, 정확하진 않음
	auto id = t.get_id();// 각 쓰레드마다 부여되는 id, 쓰레드끼리 구분가능

	
	//t.detach();// join은 기다려주는건데 이건 반대로 우리가 만들어준 스레드 객체랑 실질적으로 구동하는애랑 연결고리를 끊어줌, 즉 독립적으로 스레드
	//std::thread 객체에서 실제 쓰레드를 분리, 잘안씀~
	if(t.joinable())// 쓰레드를 만들때 한번에 꼭 함수를 연결시켜줄 필요 없음, 즉 함수가 연결 돼있어야, 객체가 관리하고 있는 스레드가 살아있는지 판단
		t.join();//t 스레드가 끝날때 까지 기다려줌



	vector<std::thread> v;

	for (int32 i = 0; i < 10; i++)
	{
		v.push_back(std::thread(HelloThread_2, i));//인자는 이리 전달
	}
	
	for (int32 i = 0; i < 10; i++)
	{
		if (v[i].joinable())
			v[i].join();
	}
	cout << "Hello Main" << endl;
}
