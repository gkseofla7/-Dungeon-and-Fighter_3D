#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
using namespace std;

//조금더 가벼운 상황에서 이벤트로 알려줌
//미래 객체가 뭔지!

__int64 Calculate()
{
	__int64 sum = 0;

	for (__int32 i = 0; i < 100'000; i++)
	{
		sum += i;
	}
	return sum;
}
void PromiseWorker(std::promise<string>&& promise)//오른값으로 만듬, 우측값 래퍼런스인거지 우측값이 아님
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<__int64(void)>&& task)
{
	task();
}


int main()
{
	//동기(synchronous) 실행, 이러면 Calculate가 끝날때까지 기다려야됨
	__int64 sum = Calculate();
	cout << sum  << endl;
	

	{

		// 1)deferred -> lazy evaluation 지연해서 실행하세요, 대체 이게 무슨 의미? 
		// 2) async -> 별도의 쓰레드를 만들어서 실행하세요
		// 3) deferred | async -> 알아서 골라주세요
		// 언젠가 미래에 결과물을 뱉어줄거야!
		std::future<__int64> future = std::async(std::launch::async, Calculate);

		//TODO
		//일감 자체가 끝났는지 엿보기
		/*std::future_status status = future.wait_for(1ms);
		if (status == future_status::ready)
		{

		}*/
		//future.wait();//결과물 나올때까지 기다림.. 뭐가다르지
		__int64 sum = future.get();

		//class Knight
		//{
		//public:
		//	__int64 GetHp() { return 100; }

		//};
		//Knight knight;
		//std::future<__int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight);//맴버함수일때
	}

	// std::promise
	{
		//미래(std::future)에 결과물을 반환해줄꺼라 약속(std::promise)해줘~ (계약서)
		std::promise<string> promise;
		std::future<string> future = promise.get_future();
		thread t(PromiseWorker, std::move(promise));//move 내일 다시 확인
		/*
		* lvalue는 메모리 상에서 존재하는 변수, 즉 &연산자를 통해 알아 낼 수 있음
		* 임시로 생성된 객체(rvalue)값의 주소를 다른 변수로 이동시키려고 할때
		* 그냥 이동하려하면 임시 객체가 소멸 시에 메모리에서 해제되기 되서 해당 
		* 객체가 소멸됨, 모든 문제는 const MyString& 이 좌측값 우측값 모두 받을 수 있다는
		* 점에서 비롯됨, 우측값만 받을 수 있는 방법?
		* move의 경우는 좌측값이 우측값으로 취급될 수 있게 바꿔주는 함수
		* xvalue : lvaue와 prvalue, 좌측값으로 분류되는 식을 이동시킬수 있음
		* move를 호출한 식은 lvalue처럼 좌측값 래퍼런스를 초기화 하는데 사용할 수 도 있고 prvalue처럼 우측값
		* 래퍼런스에 붙이거나 이동 생성자에 전달해서이동시킬 수 있음
		* std::move 된 객체를 함수에 전달한다면, 우측값 래퍼런스를 인자로 받는 함수가 오버로딩 되어서 선택됨
		* 
		*/

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	{
		std::packaged_task<__int64(void)> task(Calculate);
		std::future<__int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));

		__int64 sum = future.get();
		cout << sum << endl;

		t.join();
		
	}
}

/*
* 5장 오른값 참조, 이동 의미론, 완벽 전달
* 이동 의미론 :복사대신 이동
* 완벽 전달
*/


//void test(int && task)
//{
//	task++;
//}
//int main()
//{
//	int i = 1;
//	test(move(i));
//	i++;
//	cout << i << endl;
//}