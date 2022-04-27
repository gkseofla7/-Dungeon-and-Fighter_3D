#pragma once

//template<typename T>
//struct Node//data 를 위해 node를 동적으로 할당해 줘야됨;; 낭비
//{
//	T data;
//	Node* node;
//};

/* 메모리 풀의 경우에 굳이 아쉬운점은 계속 락을 해서 접근을 함
* 내부적인 구현자체가 queue가 있어서 동적 배열로 돼 있음
* 데이터를 넣어주기 위해서 데이터를 할당하기 위해 공간을 만듬..
* 
*/

// -------------------
//		1차 시도
// -------------------
/*
struct SListEntry
{
	SListEntry* next;
};

class Data //: public SListEntry
{
public:
	SListEntry _entry;//첫번째 맴버 변수!
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
//		2차 시도
// -------------------

//struct SListEntry
//{
//	SListEntry* next;
//};
//
//class Data //: public SListEntry
//{
//public:
//	SListEntry _entry;//첫번째 맴버 변수!
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
//	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) == 0)//desired, expected 순서
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
//	//여기서 문제는 header의 next를 다른애가 꺼내서 삭제해버리면 여기서 crash 남
//	//이전까지에서는 reference counting등을 확인해서 이를 막았음
//	//하지만 여기선 그 방법을 사용할 수가 없는게 node라는 객체를 lock free stack 객체 안에서 관리해서 가능했다면 지금은
//	//노드라는거 자체가 데이터에 포함돼있어서 안됨
//	//그보다 다른문제로 ABA 문제,  해결은 주소값만 보지말고 다른값도 같이 동시에 비교
//	while (expected && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) == 0)
//	{
//
//	}
//
//	return expected;
//}


// -------------------
//		3차 시도
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
//		} DUMMYSTRUCTNAME;//이 이름 넣으나 안넣으나
//		struct
//		{
//			uint64 depth : 16;
//			uint64 sequence : 48;
//			//depth와 sequence의 경우 카운터로 depth가 데이터가 들어가있는 개수, sequence는 pop할때
//			// 즉 alignment값이 계속 바뀜
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
//	//해당 주소를 16바이트 정렬시켜줌, 즉 최하위 4비트 0
//
//	//16바이트 정렬
//	desired.HeaderX64.next = (((uint64)entry) >> 4);//주소를 60비트, 여분의 4비트를 구분할때 사용
//
//	while (true)
//	{
//		expected = *header;
//		
//		//이 사이에 계속 변경될 수 있음
//
//		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);//즉 헤더의 next
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
//		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);//즉 헤더의 next
//		if (entry == nullptr)
//			break;
//
//		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
//		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
//		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;//얘는 계속 증가
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
//	SListEntry _entry;//첫번째 맴버 변수!
//	int64 _rand = rand() % 1000;
//};
//SListHeader* GHeader;
//int main()
//{
//	GHeader = new SListHeader();
//	ASSERT_CRASH(((uint64)GHeader % 16) == 0); //16바이트로 정렬돼있는지
//	InitializeHead(GHeader);
//
//	for (int32 i = 0; i < 3; i++)
//	{
//		GThreadManager->Launch([]()
//			{
//				while (true)
//				{
//					Data* data = new Data();
//					ASSERT_CRASH(((uint64)data % 16) == 0); //16바이트로 정렬돼있는지
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

