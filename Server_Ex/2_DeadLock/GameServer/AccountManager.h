#pragma once
#include <mutex>
class Account
{

};
class AccountManager
{
public:
	static AccountManager* Instance()
	{
		
		static AccountManager instance;
		return &instance;
		

		
	}

	Account* GetAccount(int32 id)
	{
		lock_guard<mutex> guard(_mutex);

		return nullptr;
	
	}

	void ProcessLogin();//AccountManager를 통해서 어떤 유저가 로그인했을때

private:
	mutex _mutex;
	//map<int32, Account> _accounts;//이게 임계영역
};

