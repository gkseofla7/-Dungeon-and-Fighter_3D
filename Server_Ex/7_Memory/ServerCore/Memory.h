#pragma once
#include "Allocator.h"



class MemoryPool;
//�갡 �޸�Ǯ �Ѱ�
class Memory
{
	enum
	{
		// ~1024����Ʈ ������ 32����,
		// ~2048������ 128����, ~4096���� 256����
		POOL_COUNT = (1024/32) + (1024/128) +(2048/256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void Release(void* ptr);


private:
	vector<MemoryPool*> _pools;

	// �޸� ũ�� -> �޸�Ǯ
	// �޸� Ǯ�� ������ ã�� ���� ���̺�
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};






//���� ���� ���ø�
/*���ø��� ����ؼ� ������ ������ ���ڸ� �޴� ���
* ...�� ���ø� �Ķ���� ���̶�� �θ�, 0�� �̻��� ���ø� ���ڵ�
* ���ø� �Ķ���� ���� ��� Ÿ�� �տ� ...�� ���̰� �Լ��� ��� Ÿ�� �ڿ� ...�� ����
* �Ķ���� ���� ���� �Լ��� �켱������ ����
*/
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(_xalloc(sizeof(Type)));
	//placement new �޸� ���� ������ ȣ����
	new(memory)Type(std::forward<Args>(args)...);
	//�޸� �����ٰ� ������ ȣ������, ���ڰ� �������� Type�� ���� �ǳ���
	//forward�� ��� �������۷����϶� move�Լ� �ҷ��ִ�
	return memory;

}
template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//�Ҹ��� ȣ��
	_xrelease(obj);
}



