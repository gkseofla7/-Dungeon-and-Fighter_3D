#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
using namespace std;

thread_local __int32 LThreadId = 0;

void ThreadMain(__int32 threadId)
{
    LThreadId = threadId;

    while (true)
    {
        cout << "Hi! I am Thread " << LThreadId << endl;
        this_thread::sleep_for(1s);
    }
}

int main()
{
    vector<thread>threads;

    for (__int32 i = 0; i < 10; i++)
    {
        __int32 threadId = i + 1;
        threads.push_back(thread(ThreadMain, threadId));
    }

    for (thread& t : threads)
        t.join();
}
