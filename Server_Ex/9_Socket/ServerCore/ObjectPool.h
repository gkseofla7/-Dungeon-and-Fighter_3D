#pragma once
#include "Types.h"//template 클래스라 헤더에 정의해야돼서
#include "MemoryPool.h"
//공유해서 메모리 풀을 쓰다보면 실질적으로 어떤 클래스가 문제를 일으켰는지 찾기 힘듬-> ObjectPool
template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)//Memory에서 allocate
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
		
		new(memory)Type(forward<Args>(args)...);//해당 메모리에 생성자 실행
		return memory;
	}
	static void Push(Type* obj)//Memory에서 Release
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
		
		
	}

	static shared_ptr<Type> MakeShared()
	{
		shared_ptr<Type> ptr = { Pop(), Push };
		return ptr;
	}
private:
	static int32 s_allocSize;//static이라 해도 템플릿 클래스이므로 각 템플릿 타입마다 존재
	static MemoryPool s_pool;//메모리 클래스에서 각 사이즈의 메모리 풀을 관리했는데
	//이제는 각 타입 전용의 pool을 만듬
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };

