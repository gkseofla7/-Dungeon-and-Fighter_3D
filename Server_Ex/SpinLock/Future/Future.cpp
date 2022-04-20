#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
using namespace std;

//조금더 가벼운 상황에서이벤트로 알려줌
//미래 객체가 뭔지!

__int64 Calculate()
{
	__int64 sum = 0;

	for (__int32 i = 0; i < 100'000; i++)
	{
		sum += i;
	}
}
int main()
{
	//동기(synchronous) 실행, 이러면 Calculate가 끝날때까지 기다려야됨
	cout << Calculate() << endl;

	thread t(Calculate);

}