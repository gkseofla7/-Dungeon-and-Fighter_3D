﻿// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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
// 소수 구하기
bool IsPrime(int number)
{
	if (number <= 1)
		return false;
	if (number == 2 || number == 3)
		return true;

	for (int i = 2; i < number; i++)
	{
		if ((number % i) == 0)
			return false;
	}

	return true;
}

int CountPrime(int start, int end)
{
	int count = 0;

	for (int number = start; number <= end; number++)
	{
		if (IsPrime(number))
			count++;
	}
	return count;
}
int main()
{
	const int MAX_NUMBER = 100'0000;

	vector<thread> threads;
	atomic<int> primeCount = 0;
	int coreCount = thread::hardware_concurrency();
	int jobCount = (MAX_NUMBER / coreCount) + 1;
	for (int i = 0; i < coreCount; i++)
	{
		int start = (i * jobCount) + 1;
		int end = min(MAX_NUMBER, ((i + 1) * jobCount));

		threads.push_back(thread([start, end, &primeCount]()
			{
				primeCount += CountPrime(start, end);
			}
		));

		
	}
	for (thread& t : threads)
		t.join();

	cout << primeCount << endl;
}
