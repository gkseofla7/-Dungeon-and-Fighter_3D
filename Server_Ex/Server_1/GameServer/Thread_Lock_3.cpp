// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
vector<int32> v;//stl는 일반적으로 멀티스레드에서 작동하지않음
//vector 같은경우 동적 할당인데, 배열이 꽉차면 더 큰영역을 할당받아서 데이터를 다 복사함, 그리고 원래 있던 데이터 영역을 몽땅 날려줌
//만약 동시 다발적으로 push가 이루어지면 이미 꽉찼다면 이전에 데이터 영역을 다 삭제해주는데 이미 먼저 실행한게 지워줌.. 지워줄게 없음
//즉 싱글스레드에서만...
//atomic은 일반적인 type에만
// Mutual Exclusive (상호배타적)
mutex m;

//RAII (Resource Acquisition Is Initialization), 이건 어떤 Wrapper 클래스를 만들어줘서 생성자에서 잠그고 소멸자에서 풀어줌
template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}
	~LockGuard()
	{
		_mutex->unlock();
	}
private:
	T* _mutex;
};
void Push()
{
	for (int32 i = 0; i < 10000; i++)
	{
		// 자물쇠 잠그기
		//m.lock();//이리 수동으로 lock을 관리하는건 안좋음..
		//m.lock();
		//LockGuard<std::mutex> lockGuard(m);
		//std::lock_guard<std::mutex> lockGuard(m);//위랑 똑같음
		std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock);//lock_guard에다가 옵션 추가 가능, 당장 lock을 잠그지 않음
		uniqueLock.lock();
		v.push_back(i);

		if (i == 5000)
		{
			break;//여기서 나가면 LockGuard가 소멸자 자동 불러와서 자동으로 제거해줌
		}
		//자물쇠 풀기
		//m.unlock();
		//m.unlock();//이리 재귀적으로 호출 불가능
	}
}
int main()
{
	//v.reserve(20000);//이 경우에도 crash가 되는데 데이터를 추가하는 과정에서 앞으로 넣어야될 위치를 인덱스가 가리키는데 같이 추가되면 그 과정에서 빠짐..
	std::thread t1(Push);
	std::thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}
