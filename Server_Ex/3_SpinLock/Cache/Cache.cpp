#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <Windows.h>
using namespace std;

__int32 x = 0;
__int32 y = 0;
__int32 r1 = 0;
__int32 r2 = 0;

volatile bool ready;
//가시성, 코드 재배치
// 가시성 : CPU마다 자신만의 cache를 가지고 있음;; 메모리에서 가져왔는지 캐쉬에서 가져왔는지
// 코드 재배치 : 싱글스레드 입장으로만 봐서 상관 없으면 더 효율적으로 코드를 뒤바꿈, 결과가 똑같을때, CPU가 해줄 수 도있음
void Thread_1()
{
	while (!ready)
		;
	y = 1;
	r1 = x;
}

void Thread_2()
{
	while (!ready)
		;
	x = 1;
	r2 = y;

}
int main()
{
	__int32 count = 0;
	while (true)
	{
		ready = false;
		count++;
		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}
	cout << count << " 번만에 빠져나옴 " << endl;
}

