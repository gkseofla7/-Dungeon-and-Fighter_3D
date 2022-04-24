// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Lock.h"
#include "Memory.h"

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) :_hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	//static void* operator new(size_t size)//static 붙이나 안붙이나 new연산자는 특별해서 붙인것처럼 행동
	//{
	//	cout << "Knight new! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete(void* ptr)
	//{
	//	cout << "Knight delete!" << endl;
	//	::free(ptr);
	//}
	int32 _hp = 100;
	int32 _mp = 10;;
};



//void* operator new[](size_t size)
//{
//	cout << "new[]! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete[](void* ptr)
//{
//	cout << "delete[]!" << endl;
//	::free(ptr);
//}


int main()
{
	//메모리 할당받을때 커널로 가서 받을 확률이 높음, 그래서 한번에 크게 받고 나눠서 씀
	//할당 됐다가 취소되면 빈공간 생기는데 그 크기가 안맞아서 못들어가는 경우도 생김
	//메모리풀 사용~

	//Knight* knight = new Knight();//new 연산자로 메모리를 할당한 다음 생성자 실행

	//delete knight;//소멸자 호출후 메모리 날림
	Knight* knight = xnew<Knight>(10);

	xdelete(knight);

}
//shared_ptr<int> s_ptr = make_shared<int>();
//weak_ptr<int> a = s_ptr;
//
//bool expired = a.expired();//데이터가 존재하는지 확인함
//shared_ptr<int> s_ptr2 = a.lock();//shared_ptr로 바꿔줌
////RefCountBlock같은 경우는 shared_ptr이 모두 release 돼면 해당 객체는 삭제되지만 RefCountBlock은 삭제 안함, weakPtr까지 삭제 돼야 삭제함