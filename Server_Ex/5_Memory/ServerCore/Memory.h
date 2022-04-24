#pragma once
#include "Allocator.h"

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



