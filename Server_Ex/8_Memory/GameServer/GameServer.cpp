// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Lock.h"
#include "Memory.h"
#include "Allocator.h"


using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
public:
	Player()
	{
		INIT_TL(Player)
	}
	virtual ~Player()
	{}
	DECLARE_TL
};
class Knight: public Player
{
public:
	Knight() { INIT_TL(Knight) }
	
	int _typeId;
};

class Mage : public Player
{
public:
	Mage() { INIT_TL(Mage); }
	int _typeId;
};
class Archer : public Player
{
public:
	int _typeId;
};
class Dog
{

};
class Monster
{
public:
	int64 _id = 0;
};

int main()
{

	{
		Player* player = new Knight();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);

		delete player;
	}

	{
		shared_ptr<Knight> knight = MakeShared<Knight>();

		shared_ptr<Player> player = TypeCast<Player>(knight);
		bool canCast =  CanCast<Mage>(player);
	}

	TypeConversion<TL> test;
	test.s_convert[0][0];
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{

				}
			});
	}

	GThreadManager->Join();

}




