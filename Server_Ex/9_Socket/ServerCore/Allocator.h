#pragma once



class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};



//버그를 잡는데 굉장히 유용, 메모리 오염 버그
//단점 큰 영억 할당함
//메모리 오버플로우는 잡아주지 못함.. 즉 부모 클래스를 자식클래스로 변환해서 부모클래스에
//없는 영역을 건드려도 우리가 할당한 영역을 건들이기 때문에 크래쉬 발생안함;;
//오버플로우 문제는 어떻게 잡아줌? 맨뒤에서부터 채우면 됨!, 언더플로우는 못잡아주지만 언더플로우는 잘 안생김

class StompAllocator
{
	enum {PAGE_SIZE = 0x1000};//할당할때 페이즈 사이즈로 할당

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