#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

class CoreGlobal//���߿� manager�� ������ �����Ҷ� Manager���� ������ �����ٶ�
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;



