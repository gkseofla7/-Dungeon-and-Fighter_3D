
#include <iostream>
#include <stack>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
using namespace std;


LockQueue<__int32> q;
LockFreeStack<__int32> s;

void Push()
{
	while (true)
	{
		__int32 value = rand() % 100;
		s.Push(value);

		//this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{

		__int32 data = 0;
		if(s.TryPop(OUT data))
			cout << data << endl;

	}
}
int main()
{
	thread t1(Push);
	thread t2(Pop);

	t1.join();
	t2.join();
}

