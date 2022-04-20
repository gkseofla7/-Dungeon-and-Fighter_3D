
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
		bool expected = false;
		bool desired = true;

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			//this_thread::sleep_for(std::chrono::milliseconds(100));
			//this_thread::sleep_for(100ms);
			this_thread::yield();//자기 타임슬라이스를 양보함 == sleep_for(0ms)
		}

	}

	void unlock()
	{
		//_locked = false;
		_locked.store(false);
	}

private:

	atomic<bool> _locked = false;//atomic에 volatile 포함됨
};

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

	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}
