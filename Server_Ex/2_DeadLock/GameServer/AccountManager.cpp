#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"
void AccountManager::ProcessLogin()
{
	// accountLock
	lock_guard<mutex> guard(_mutex);

	// usesrLock
	UserManager::Instance()->GetUser(100);
	
	//���� �ٸ� ���ؽ� ��ü..
}
