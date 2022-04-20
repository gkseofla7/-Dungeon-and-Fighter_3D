// Event.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
using namespace std;

mutex m;
queue<__int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		::SetEvent(handle);
		this_thread::sleep_for(10000ms);
	}
}
void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);//Sleep으로 기다림
		//bManualReset이 auto라 자동으로 Non-Signal상태로 바꿔줌
		//::RegisterEventSource(handle);
		unique_lock<mutex> lock(m);
		if (!q.empty())
		{
			__int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}
int main()
{
	handle = ::CreateEvent(NULL, FALSE/*bManualReset*/, FALSE, NULL);//두번째 인자의 경우 true면 수동으로, 세번째 인자는 일단 signal상태를 꺼둠
	
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}