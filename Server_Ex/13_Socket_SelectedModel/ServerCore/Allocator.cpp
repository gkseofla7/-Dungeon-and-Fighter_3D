#include "pch.h"
#include "Allocator.h"
#include "Memory.h"


/*--------------------
	BaseAllocator
--------------------*/
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}


/*--------------------
	StompAllocator
--------------------*/
void* StompAllocator::Alloc(int32 size)
{
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;//반올림
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	
	void* baseAddress =  ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//casting 해주는 이유는 포인터 계산은 타입 크기에 따라 영향을 줘서
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);

}

void StompAllocator::Release(void* ptr)
{
	//reinterpret_cast : 임의의 포인터 타입끼리 변환을 허용하는 캐스트 연산자
	//또한 정수형을 포인터로 바꿀 수도 있음
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}




/*--------------------
	PoolAllocator
--------------------*/
void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);

}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}