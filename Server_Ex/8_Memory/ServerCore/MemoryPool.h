#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};
// 사실 동적으로 할당하는 메모리가 객체만이 있는게 아님, vector, map 등
// 객체단위로만 풀링 ㄴㄴ
// 왜 polling? 해제 할당을 반복하면 컨텍스트 스위칭이 일어나서 비효율적, 메모리 파편화등으로 인해서도

//여러개의 memory pool을 할당해서 각각 담당하는 크기를 가지고 있음
//즉 어느정도 크기의 메모리는 어느 메모리풀이 담당
//여기서는 동일한 크기로 나눔(그래di 파편화때 굿)

//객체들마다 사이즈가 다르니 디버깅을 위해
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{

	//각 데이터마다 [MemoryHeader][Data]
	//사실 다른곳에서도 memoryHeader사용하는데 delete할때 헤더 보고 메모리 삭제
	
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		//메모리 넣을때 붙여줌
		new(header)MemoryHeader(size); //placement new, 즉 메모리 만들어졌으니 거기에 생성자 호출
		//사용하는애 입장에서는 헤더의 존재를 모르니 그냥 데이터쪽 주소로 반환
		return reinterpret_cast<void*>(++header);//++하면 MemoryHeader크기만큼 증가하기 때문에

	}
	
	static MemoryHeader* DetachHeader(void* ptr)
	{
		//메모리 제거할때 떼서 확인
		MemoryHeader* header = reinterpret_cast<MemoryHeader*> (ptr) - 1;
		return header;
	}

	int32 allocSize;
	

};
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);//다사용했으면 push
	MemoryHeader* Pop();

private:
	SLIST_HEADER _header;//얘가 lock free stack을 가르킴
	int32 _allocSize = 0;//각각의 MemoryPool마다 담당하는 사이즈가 다르니 그 크기
	atomic<int32> _useCount = 0;// 지금 메모리 풀에 몇개가 사용되고 있는지
	atomic<int32> _reserveCount = 0;
};

