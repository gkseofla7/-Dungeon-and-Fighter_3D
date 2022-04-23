#pragma once
//tls 는 ThreadLocalStorage

//thread 만들어진 순서대로 id 1부터 제공
extern thread_local uint32 LThreadId;


//extern thread_local std::stack<int32> LLockStack;
