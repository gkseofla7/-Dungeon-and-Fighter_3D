#pragma once
#include "Allocator.h"

//가변 길이 템플릿
/*템플릿을 사용해서 임의의 개수의 인자를 받는 방법
* ...은 템플릿 파라미터 팩이라고 부름, 0개 이상의 템플릿 인자들
* 템플릿 파라미터 팩의 경우 타입 앞에 ...를 붙이고 함수의 경우 타입 뒤에 ...를 붙임
* 파라미터 팩이 없는 함수의 우선순위가 높음
*/
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(_xalloc(sizeof(Type)));
	//placement new 메모리 위에 생성자 호출함
	new(memory)Type(std::forward<Args>(args)...);
	//메모리 위에다가 생성자 호출해줌, 인자가 있을때는 Type에 인자 건네줌
	//forward의 경우 우측레퍼런스일때 move함수 불러주는
	return memory;

}
template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//소멸자 호출
	_xrelease(obj);
}



