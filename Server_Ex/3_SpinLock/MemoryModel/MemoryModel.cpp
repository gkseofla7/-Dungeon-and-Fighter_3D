#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <Windows.h>
using namespace std;
atomic<bool> flag ;
//복습용 코드
//int main()
//{
//	
//	flag = false;
//
//	//flag = true;
//	flag.store(true,memory_order::memory_order_seq_cst);
//
//
//	//bool val = flag;
//	bool val = flag.load(memory_order::memory_order_seq_cst);
//	flag.is_lock_free();
//
//	// 이전 flag 값을 prev에 넣고, flag 값을 수정
//	{
//		bool prev = flag.exchange(true); //읽고 쓰는걸 한번에
//		/*bool prev = flag;
//		flag = true;*/
//	}
//
//	// CAS (Compare-And-Swap) 조건부 수정
//	{
//		bool expected = false;
//		bool desired = true;
//		flag.compare_exchange_strong(expected, desired);
//
//
//		//if (flag == expected)
//		//{
//		//	expected = flag;
//		//	flag = desired;
//		//	return true;
//		//}
//		//else
//		//{
//		//	expected = flag;
//		//	return false;
//		//}
//
//		
//		// // 이건 무슨 차이?  동작방식은 똑같지만  flag == expected인 경우 다른 스레드의 방해를 받던 해서 실패할수가 있음
//		//strong의 경우엔 무조건 같으면 성공, strong의 경우 묘한상황으로 인해 실패하면 다시 또 함
//		//while (true)
//		//{
//		//	bool expected = flag;
//		//	bool desired = true;
//		//	flag.compare_exchange_weak(expected, desired);
//		//}
//	}
//}
atomic<bool> ready;
__int32 value;

void Producer()
{
	value = 10;

	ready.store(true, memory_order::memory_order_seq_cst);//release
	// ------------------- 절취선 ------------------ 위에 애들이 아래로 내려가는걸 막아줌, 위에서는 섞일수 있음
}

void Consumer()
{
	//------------------ 절취선 ----------------
	while (ready.load(memory_order::memory_order_seq_cst) == false)//acquire
		;
	cout << value << endl;
}

int main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	//Memory Model (정책)
	// 1) Sequentially Consistent (seq_cst)
	// 2) Acquire-Release (acquire, release)
	// 3) Relaxed (relaxed)

	// 1) seq_cst (가장 엄격 = 컴파일러 최적화 여지 적음 = 직관적)
	// // 가시성 문제 바로 해결! 코드 재배치 바로 해결!
	// 
	// 2) acquire-release
	// 딱 중간
	// release 명령 이전의 메모리 명령들이, 해당 명령 이후로 재배치 되는 것을 금지
	// 그리고 acquire로 같은 변수를 읽는 쓰레드가 있다면
	// release 이전의 명령들이 -> acquire 하는 순간에 관찰 가능 (가시성 보장)
	// 
	// 3) release (자유롭다 = 컴파일러 최적화 여지 많음 = 직관적이지 않음)
	// 코드 재배치도 멋대로 가능! 가시성 해결 No!
	// 가장 기본 조건 (동일 객체에 대한 동일 관전 순서만 보장)
}

