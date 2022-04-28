#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};
// ��� �������� �Ҵ��ϴ� �޸𸮰� ��ü���� �ִ°� �ƴ�, vector, map ��
// ��ü�����θ� Ǯ�� ����
// �� polling? ���� �Ҵ��� �ݺ��ϸ� ���ؽ�Ʈ ����Ī�� �Ͼ�� ��ȿ����, �޸� ����ȭ������ ���ؼ���

//�������� memory pool�� �Ҵ��ؼ� ���� ����ϴ� ũ�⸦ ������ ����
//�� ������� ũ���� �޸𸮴� ��� �޸�Ǯ�� ���
//���⼭�� ������ ũ��� ����(�׷�di ����ȭ�� ��)

//��ü�鸶�� ����� �ٸ��� ������� ����
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{

	//�� �����͸��� [MemoryHeader][Data]
	//��� �ٸ��������� memoryHeader����ϴµ� delete�Ҷ� ��� ���� �޸� ����
	
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		//�޸� ������ �ٿ���
		new(header)MemoryHeader(size); //placement new, �� �޸� ����������� �ű⿡ ������ ȣ��
		//����ϴ¾� ���忡���� ����� ���縦 �𸣴� �׳� �������� �ּҷ� ��ȯ
		return reinterpret_cast<void*>(++header);//++�ϸ� MemoryHeaderũ�⸸ŭ �����ϱ� ������

	}
	
	static MemoryHeader* DetachHeader(void* ptr)
	{
		//�޸� �����Ҷ� ���� Ȯ��
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

	void Push(MemoryHeader* ptr);//�ٻ�������� push
	MemoryHeader* Pop();

private:
	SLIST_HEADER _header;//�갡 lock free stack�� ����Ŵ
	int32 _allocSize = 0;//������ MemoryPool���� ����ϴ� ����� �ٸ��� �� ũ��
	atomic<int32> _useCount = 0;// ���� �޸� Ǯ�� ��� ���ǰ� �ִ���
	atomic<int32> _reserveCount = 0;
};

