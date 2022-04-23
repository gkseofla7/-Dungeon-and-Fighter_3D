#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
class CoreGlobal//나중에 manager가 여러개 등장할때 Manager끼리 순서를 맞춰줄때
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;



