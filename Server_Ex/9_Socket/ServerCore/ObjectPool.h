#pragma once
#include "Types.h"//template Ŭ������ ����� �����ؾߵż�
#include "MemoryPool.h"
//�����ؼ� �޸� Ǯ�� ���ٺ��� ���������� � Ŭ������ ������ �����״��� ã�� ����-> ObjectPool
template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)//Memory���� allocate
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
		
		new(memory)Type(forward<Args>(args)...);//�ش� �޸𸮿� ������ ����
		return memory;
	}
	static void Push(Type* obj)//Memory���� Release
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
	static int32 s_allocSize;//static�̶� �ص� ���ø� Ŭ�����̹Ƿ� �� ���ø� Ÿ�Ը��� ����
	static MemoryPool s_pool;//�޸� Ŭ�������� �� �������� �޸� Ǯ�� �����ߴµ�
	//������ �� Ÿ�� ������ pool�� ����
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };

