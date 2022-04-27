#pragma once
#include "Types.h"
//������Ÿ�ӿ� �� ������
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
//enum�� ������Ÿ�ӿ� ������
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

//Ŭ������ �����ָ� ��� �ε���?
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

	static Small Test(const To&) { return 0; }//From�̶��� To�� ��ȯ���� ������
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }//��� ������ �ʿ� ����
	//�����Ϸ��� �˾Ƽ� ��ȯ���ִµ� ...�� ���� ������ ����
	//�� �ΰ��� ���� �ش��� ���������� ���� �ϳ� ���õǰ�
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
	enum{value = v};//�� 1, 2, 3 �� ��� �ٸ� Ŭ������ �ν���, ���� ��ü�� Ŭ������ �ν�
};


//TL�� ���̰� 3�̸� 3X3 �迭�� ���� ��ȯ�� �������� �˷���
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
		//���⼭ i, j�� ������Ÿ�ӿ� ����ž� �ż� �����ž���..
		//�̸� �ϸ� i, j�� ��Ÿ�ӿ� ���ÿ� ���� ���������� �ƴ϶� maketable�� �����ż� ������ �������
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
		{
			s_convert[i][j] = true;
		}
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>());//�ٸ��Լ��� �� �������
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

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))//�����͸� �Ϲ� ����Ÿ������
		return static_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))//�����͸� �Ϲ� ����Ÿ������
		return static_pointer_cast<To>(ptr);//����Ʈ�����͹�ȯ

	return nullptr;
}


template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value));//�����͸� �Ϲ� ����Ÿ������


	return false;
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value));//�����͸� �Ϲ� ����Ÿ������


	return false;
}
#pragma endregion

#define DECLARE_TL using TL = TL; int32 _typeId;
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value;

//{
		//	//�����ڵ�
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