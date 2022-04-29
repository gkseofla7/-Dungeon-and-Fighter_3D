#pragma once



class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};



//���׸� ��µ� ������ ����, �޸� ���� ����
//���� ū ���� �Ҵ���
//�޸� �����÷ο�� ������� ����.. �� �θ� Ŭ������ �ڽ�Ŭ������ ��ȯ�ؼ� �θ�Ŭ������
//���� ������ �ǵ���� �츮�� �Ҵ��� ������ �ǵ��̱� ������ ũ���� �߻�����;;
//�����÷ο� ������ ��� �����? �ǵڿ������� ä��� ��!, ����÷ο�� ����������� ����÷ο�� �� �Ȼ���

class StompAllocator
{
	enum {PAGE_SIZE = 0x1000};//�Ҵ��Ҷ� ������ ������� �Ҵ�

public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);

};






class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);

};

template<typename T>
class StlAllocator
{

public:
	using value_type = T;

	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {	}

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(_xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		_xrelease(ptr);
	}

};