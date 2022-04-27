#pragma once
#include "Types.h"
//컴파일타임에 다 정해짐
#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};

#pragma endregion


#pragma region Length
//enum은 컴파일타임에 지정됨
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};
template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion


#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};
template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};


#pragma endregion

//클래스를 던져주면 몇번 인덱스?
#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0};
};
template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum {value = -1};
};
template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value};

public:
	enum{value = (temp == -1)? -1: temp + 1 };
};

#pragma endregion


#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }//From이란게 To로 변환되지 않으면
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }//사실 구현부 필요 없음
	//컴파일러가 알아서 변환해주는데 ...이 제일 순위가 낮음
	//즉 두갈래 길을 준다음 강제적으로 둘중 하나 선택되게
public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};
#pragma endregion

#pragma region TypeCast
template<int32 v>
struct Int2Type
{
	enum{value = v};//즉 1, 2, 3 등 모두 다른 클래스로 인식함, 숫자 자체를 클래스로 인식
};


//TL의 길이가 3이면 3X3 배열을 만들어서 변환이 가능한지 알려줌
template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()//i->j
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
		
	}


	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		//여기서 i, j도 컴파일타임에 실행돼야 돼서 고정돼야함..
		//이리 하면 i, j가 런타임에 스택에 들어가는 지역변수가 아니라 maketable이 고정돼서 여러개 만들어짐
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
		{
			s_convert[i][j] = true;
		}
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>());//다른함수가 또 만들어짐
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}
public:
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];


template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))//포인터를 일반 변수타입으로
		return static_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))//포인터를 일반 변수타입으로
		return static_pointer_cast<To>(ptr);//스마트포인터반환

	return nullptr;
}


template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value));//포인터를 일반 변수타입으로


	return false;
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value));//포인터를 일반 변수타입으로


	return false;
}
#pragma endregion

#define DECLARE_TL using TL = TL; int32 _typeId;
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value;

//{
		//	//예제코드
	//TypeList<Mage, Knight>::Head whoAMI;
	//TypeList<Mage, Knight>::Tail whoAMI2;

	//TypeList < Mage, TypeList<Knight, Archer>>::Head whoAmI3;
	//TypeList < Mage, TypeList<Knight, Archer>>::Tail::Head whoAmI4;
	//TypeList < Mage, TypeList<Knight, Archer>>::Tail::Tail whoAmI5;


	//int32 len1 = Length<TypeList<Mage, Knight>>::value;
	//int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;


	//using TL = TypeList<Player, Mage, Knight, Archer>;
	//TypeAt<TL, 0>::Result whoAmI6;
	//TypeAt<TL, 1>::Result whoAmI7;
	//TypeAt<TL, 2>::Result whoAmI8;

	//int32 index1 = IndexOf<TL, Mage>::value;


	//bool canConvert1 = Conversion<Player, Knight>::exists;
	//bool canConvert2 = Conversion<Knight, Player>::exists;//Knight->Player
	//bool canConvert3 = Conversion<Knight, Dog>::exists;
//}