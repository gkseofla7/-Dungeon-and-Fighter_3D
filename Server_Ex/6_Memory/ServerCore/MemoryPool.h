#pragma once
// ��� �������� �Ҵ��ϴ� �޸𸮰� ��ü���� �ִ°� �ƴ�, vector, map ��
// ��ü�����θ� Ǯ�� ����
// �� polling? ���� �Ҵ��� �ݺ��ϸ� ���ؽ�Ʈ ����Ī�� �Ͼ�� ��ȿ����, �޸� ����ȭ������ ���ؼ���

//�������� memory pool�� �Ҵ��ؼ� ���� ����ϴ� ũ�⸦ ������ ����
//�� ������� ũ���� �޸𸮴� ��� �޸�Ǯ�� ���
//���⼭�� ������ ũ��� ����(�׷��� ����ȭ�� ��)

//��ü�鸶�� ����� �ٸ��� ������� ����
struct MemoryHeader
{

	//�� �����͸��� [MemoryHeader][Data]
	//��� �ٸ��������� memoryHeader����ϴµ� delete�Ҷ� ��� ���� �޸� ����
	
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); //placement new, �� �޸� ����������� �ű⿡ ������ ȣ��
		//����ϴ¾� ���忡���� ����� ���縦 �𸣴� �׳� �������� �ּҷ� ��ȯ
		return reinterpret_cast<void*>(++header);//++�ϸ� MemoryHeaderũ�⸸ŭ �����ϱ� ������

	}
	
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*> (ptr) - 1;
		return header;
	}

	int32 allocSize;
	

};

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);//�ٻ�������� push
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;//������ MemoryPool���� ����ϴ� ����� �ٸ��� �� ũ��
	atomic<int32> _allocCount = 0;

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

