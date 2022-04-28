#pragma once

//template<typename T>
//struct Node//data �� ���� node�� �������� �Ҵ��� ��ߵ�;; ����
//{
//	T data;
//	Node* node;
//};

/* �޸� Ǯ�� ��쿡 ���� �ƽ������� ��� ���� �ؼ� ������ ��
* �������� ������ü�� queue�� �־ ���� �迭�� �� ����
* �����͸� �־��ֱ� ���ؼ� �����͸� �Ҵ��ϱ� ���� ������ ����..
* 
*/

// -------------------
//		1�� �õ�
// -------------------
/*
struct SListEntry
{
	SListEntry* next;
};

class Data //: public SListEntry
{
public:
	SListEntry _entry;//ù��° �ɹ� ����!
};
class LockFreeStack
{
};

struct SListHeader
{
	SListEntry* next = nullptr;
};


void InitializeHead(SListHeader* header)
{
	header->next = nullptr;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;
	header->next = entry;
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* first = header->next;

	if (first != nullptr)
		header->next = first->next;

	return first;
}*/


// -------------------
//		2�� �õ�
// -------------------

//struct SListEntry
//{
//	SListEntry* next;
//};
//
//class Data //: public SListEntry
//{
//public:
//	SListEntry _entry;//ù��° �ɹ� ����!
//};
//class LockFreeStack
//{
//};
//
//struct SListHeader
//{
//	SListEntry* next = nullptr;
//};
//
//
//void InitializeHead(SListHeader* header)
//{
//	header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{
//	entry->next = header->next;
//
//	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) == 0)//desired, expected ����
//	{
//
//	}
//
//
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* expected = header->next;
//	//���⼭ ������ header�� next�� �ٸ��ְ� ������ �����ع����� ���⼭ crash ��
//	//�������������� reference counting���� Ȯ���ؼ� �̸� ������
//	//������ ���⼱ �� ����� ����� ���� ���°� node��� ��ü�� lock free stack ��ü �ȿ��� �����ؼ� �����ߴٸ� ������
//	//����°� ��ü�� �����Ϳ� ���Ե��־ �ȵ�
//	//�׺��� �ٸ������� ABA ����,  �ذ��� �ּҰ��� �������� �ٸ����� ���� ���ÿ� ��
//	while (expected && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) == 0)
//	{
//
//	}
//
//	return expected;
//}


// -------------------
//		3�� �õ�
// -------------------

//DECLSPEC_ALIGN(16)
//struct SListEntry
//{
//	SListEntry* next;
//};
//
//
//class LockFreeStack
//{
//};
//
//
//DECLSPEC_ALIGN(16)
//struct SListHeader
//{
//	SListHeader()
//	{
//		alignment = 0;
//		region = 0;
//	}
//	
//	union
//	{
//		struct
//		{
//			uint64 alignment;
//			uint64 region;
//		} DUMMYSTRUCTNAME;//�� �̸� ������ �ȳ�����
//		struct
//		{
//			uint64 depth : 16;
//			uint64 sequence : 48;
//			//depth�� sequence�� ��� ī���ͷ� depth�� �����Ͱ� ���ִ� ����, sequence�� pop�Ҷ�
//			// �� alignment���� ��� �ٲ�
//			uint64 reserved : 4;
//			uint64 next : 60;
//		} HeaderX64;
//
//	};
//};
//
//
//void InitializeHead(SListHeader* header)
//{
//	header->alignment = 0;
//	header->region = 0;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{
//	SListHeader expected = {};
//	SListHeader desired = {};
//	//�ش� �ּҸ� 16����Ʈ ���Ľ�����, �� ������ 4��Ʈ 0
//
//	//16����Ʈ ����
//	desired.HeaderX64.next = (((uint64)entry) >> 4);//�ּҸ� 60��Ʈ, ������ 4��Ʈ�� �����Ҷ� ���
//
//	while (true)
//	{
//		expected = *header;
//		
//		//�� ���̿� ��� ����� �� ����
//
//		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);//�� ����� next
//		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
//		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;
//
//		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
//			break;
//
//
//	}
//
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListHeader expected = {};
//	SListHeader desired = {};
//	SListEntry* entry = nullptr;
//
//	while (true)
//	{
//		expected = *header;
//		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);//�� ����� next
//		if (entry == nullptr)
//			break;
//
//		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
//		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
//		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;//��� ��� ����
//
//		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
//			break;
//
//
//	}
//	return entry;
//}
//
//DECLSPEC_ALIGN(16)
//class Data //: public SListEntry
//{
//public:
//	SListEntry _entry;//ù��° �ɹ� ����!
//	int64 _rand = rand() % 1000;
//};
//SListHeader* GHeader;
//int main()
//{
//	GHeader = new SListHeader();
//	ASSERT_CRASH(((uint64)GHeader % 16) == 0); //16����Ʈ�� ���ĵ��ִ���
//	InitializeHead(GHeader);
//
//	for (int32 i = 0; i < 3; i++)
//	{
//		GThreadManager->Launch([]()
//			{
//				while (true)
//				{
//					Data* data = new Data();
//					ASSERT_CRASH(((uint64)data % 16) == 0); //16����Ʈ�� ���ĵ��ִ���
//
//					PushEntrySList(GHeader, (SListEntry*)data);
//					this_thread::sleep_for(10ms);
//				}
//			});
//	}
//
//	for (int32 i = 0; i < 2; i++)
//	{
//		GThreadManager->Launch([]()
//			{
//				while (true)
//				{
//					Data* pop = nullptr;
//					pop = (Data*)PopEntrySList(GHeader);
//
//					if (pop)
//					{
//						cout << pop->_rand << endl;
//						delete pop;
//					}
//					else
//					{
//						cout << "None" << endl;
//					}
//				}
//			});
//	}
//}

