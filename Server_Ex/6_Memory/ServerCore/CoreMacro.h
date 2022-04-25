#pragma once
//crash를 잡고싶다고해서 3/0 이리 하면 컴파일러가 잡아버림

#define OUT
/*-----------
* LOCK
------------*/
#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());//전처리단계에서 자동처리해줌
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());//전처리단계에서 자동처리해줌
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

/*-----------
* Memory
------------*/
#ifdef _DEBUG
#define _xalloc(size)	PoolAllocator::Alloc(size)
#define _xrelease(ptr)	PoolAllocator::Release(ptr)

#else
#define _xalloc(size)	BaseAllocator::Alloc(size)
#define _xrelease(ptr)	BaseAllocator::Release(ptr)
#endif

/*-----------
* CRASH
------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}											\



#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}