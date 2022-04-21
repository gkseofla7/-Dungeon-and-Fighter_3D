#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
using namespace std;

mutex m;
queue<__int32> q;

condition_variable cv; //뮤텍스랑 동작함
//CV는 User-Level Object (커널 오브젝트X)
//만약 sleep_for가 오랜시간 하면 문제 없지만 만약 Producer코드가 계속 큐에 집어넣으면
//즉 이벤트를 Signal상태로 바꿔주는 코드랑 queue에 넣어주는 코드랑 분리돼있어서 문제 발생
void Producer()
{
	while (true)
	{
		// 1) Lock을 잡고
		// 2) 공유변수 값을 수정
		// 3) Lock을 풀고
		// 4)조건변수 통해 다른 쓰레드에게 통제
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one();//wait중인 쓰레드가 있으면 딱 1개를 깨움
		//this_thread::sleep_for(10000ms);
		
	}
}
void Consumer()
{
	while (true)
	{

		unique_lock<mutex> lock(m);//여기서 또 기다림;;;
		cv.wait(lock, []() { return q.empty() == false; });//큐가 비어있지 않을때까지 기다리고 싶음
		/*
		* 1) Lock을 잡고
		* 2) 조건 확인
		* - 만족O => 빠져 나와서 이어서 코드 진행
		* - 만족X 즉 큐가 비워져 있을때=> Lock을 풀어주고 대기 상태
		* 무조건 unique_lock 사용
		*/
		
		// 그런데 notify_one을 했으면 항상 조건식을 만족하는거 아님?
		// Spurious Wakeup(가짜 기상?)
		// notify_one을 하기 직전에 쓰레드가 바뀔수도 있음, 또 다른 누군가가 queue에 있는거 사용가능..
		// notify_one을 할때 lock을 잡고있는 것이 아니기 때문에 위 발생가능
		{
			__int32 data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}
int main()
{

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

}