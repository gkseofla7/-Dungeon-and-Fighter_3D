
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
using namespace std;




class SpinLock
{
public:
	void lock()
	{
		////CAS (Compare-And_Swap) 한번에 일어나게 함
		bool expected = false;
		bool desired = true;

		////CAS 의사코드
		//if (_locked == expected)
		//{
		//	expected = _locked;
		//	_locked = desired;
		//	//즉 locked가 false였으면 true로 바꿔줌
		//	return true;//이건 잘바꿨다는뜻
		//}
		//else
		//{
		//	expected = _locked;
		//	return false;
		//}
		//일단 locked라는 값이 어떤 값이길 기대하고 있음
		while (_locked.compare_exchange_strong(expected, desired)==false)
		{
			//expected값을 내부에서 계속 바꿔줘서 되돌려줘야됨
			expected = false;
		}
		//while (_locked)
		//{

		//}
		//_locked = true;//이 두 과정이 동시에 일어나야됨
	}

	void unlock()
	{
		//_locked = false;
		_locked.store(false);
	}

private:
	//volatile bool _locked = false;
	//volatile 키워드는 컴파일러한테 최적화 하지말아달라, 캐쉬X 그건 C# 자바
	atomic<bool> _locked = false;//atomic에 volatile 포함됨
};
//우리가 만든 스핀락에 대체 어디가 문제?
/*
* 같이 들어와서 같이잠금... 즉 문 잠그는게 여러 단계로 나눠져있어서
* lock의 상태를 체크하는거랑 lock을 잠궈주는거랑 분리돼서 발생해서.. atomic하게 일어나게 해야됨
*/
__int32 sum = 0;
mutex m;
SpinLock spinLock;

void Add()
{
	for (__int32 i = 0; i < 10'000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}
void Sub()
{
	for (__int32 i = 0; i < 10'000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}
int main()
{

	//bool flag = true;

	//while (flag)
	//{
	//	//이리하면 flag값을 체크도 안하고 그냥 점프해서 무한루프, 최적화때매 컴파일러 입장에서는 어차피 점프할거라고 생각해서;
	//}
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}
