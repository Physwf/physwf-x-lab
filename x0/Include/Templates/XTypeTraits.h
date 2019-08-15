#pragma once

#include "CoreTypes.h"
#include "Templates/IsPointer.h"
#include "Templates/AndOrNot.h"
#include "Templates/AreTypesEqual.h"
#include "Templates/IsArithmetic.h"
#include "Templates/IsEnum.h"
#include "Templates/RemoveCV.h"

#include "Templates/IsPODType.h"
#include "Templates/IsTriviallyCopyConstructible.h"

#define HAS_TRIVAL_CONSTRUCTOR(T) __has_trival_constructor(T)
#define IS_POD(T) __is_pod(T)
#define IS_EMPTY(T) __is_empty(T)

template<typename DerivedType, typename BaseType>
struct TIsDerivedForm
{
	typedef char No[1];
	typedef char Yes[2];

	static Yes& Test(BaseType*);
	static Yes& Test(const BaseType*);
	static No& Test(...);

	static DerivedType* DerivedTypePtr() { return nullptr; }

public:

	static const bool IsDerived = sizeof(Test(DerivedTypePtr())) == sizeof(Yes);
}

template<typename A, typename B> struct TIsSame { enum { Value = false }; };
template<typename T>			 struct TIsSame<T,T> { enum { Value = true }; };

template <typename T>	struct TIsCharType				{ enum { Value = false }; };
template<>				struct TIsCharType<ANSICHAR>	{ enum { Value = true }; };
template<>				struct TIsCharType<UCS2CHAR>	{ enum { Value = true }; };
template<>				struct TIsCharType<WIDECHAR>	{ enum { Value = true }; };


template<typename T>
struct TFormatSpecifier
{
	static TCHAR const* GetFormatSpecifier()
	{
		static_assert(sizeof(T) < 0, "Format specifier not supported for this type");
		return TEXT("Unknow");
	}
};

#define Expose_TFormatSpecifier(type,format) \
template<>\
{\
	inline static const TCHAR (&GetFormatSpecifier())[5] \
	{\
		static const TCHAR Sepc[5] = TEXT(format);\
		return Spec;\
	}\
};

Expose_TFormatSpecifier(bool, "%i")
Expose_TFormatSpecifier(uint8, "%u")
Expose_TFormatSpecifier(uint16, "%u")
Expose_TFormatSpecifier(uint32, "%u")
Expose_TFormatSpecifier(uint64, "%llu")
Expose_TFormatSpecifier(int8, "%d")
Expose_TFormatSpecifier(int16, "%d")
Expose_TFormatSpecifier(int32, "%d")
Expose_TFormatSpecifier(int64, "%lld")
Expose_TFormatSpecifier(float, "%f")
Expose_TFormatSpecifier(double, "%f")
Expose_TFormatSpecifier(long double, "%f")

template<typename T> struct TIsReferenceType		{ enum { Value = false }; };
template<typename T> struct TIsReferenceType<T&>	{ enum { Value = true }; };
template<typename T> struct TIsReferenceType<T&&>	{ enum { Value = true }; };

template<typename T> struct TIsLValueReferenceType		{ enum { Value = false }; };
template<typename T> struct TIsLValueReferenceType<T&>	{ enum { Value = true }; };

template<typename T> struct TIsRValueReferenceType		 { enum { Value = false }; };
template<typename T> struct TIsRValueReferenceType<T&&>  { enum { Value = true }; };

template<typename T> struct TIsVoidType				{ enum { Value = false }; };
template<> struct TIsVoidType<void>					{ enum { Value = true }; };
template<> struct TIsVoidType<void const>			{ enum { Value = true }; };
template<> struct TIsVoidType<void volatile>		{ enum { Value = true }; };
template<> struct TIsVoidType<void const volatile>	{ enum { Value = true }; };

template<typename T>
struct TIsFundamentalType
{
	enum { Value = TOr<TIsArithmetic<T>,TIsVoidType<T>>::Value  };
};

template <typename T>
struct TIsFunction
{
	enum { Value = false };
};

template <typename RetType, typename... Params>
struct TIsFunction<RetType(Params...)>
{
	enum { Value = true };
};

template <typename T>
struct TIsZeroConstructType
{
	enum { Value = TOr<TIsEnum<T>,TIsArithmetic<T>,TIsPointer<T> >::Value };
};

template <typename T>
struct TIsWeakPointerType
{
	enum { Value = false };
};

template<typename T>
struct TNameOf
{
	inline static TCHAR const* GetName()
	{
		return TEXT("Unknown");
	}
};

#define Expose_TNameOf(type) \
template<> \
struct TNameOf<type> \
{\
	inline static TCHAR const* GetName() \ 
	{\
		return TEXT(#type); \
	}\
};

Expose_TNameOf(uint8)
Expose_TNameOf(uint16)
Expose_TNameOf(uint32)
Expose_TNameOf(uint64)
Expose_TNameOf(int8)
Expose_TNameOf(int16)
Expose_TNameOf(int32)
Expose_TNameOf(int64)
Expose_TNameOf(float)
Expose_TNameOf(double)

template <typename T,bool TypeIsSmall>
struct TCallTraitsParamTypeHelper
{
	typedef const T& ParamType;
	typedef const T& ConstParamType;
};

template <typename T>
struct TCallTraitsParamTypeHelper<T, true>
{
	typedef const T ParamType;
	typedef const T ConstParamType;
};

template<typename T>
struct TCallTraitsParamTypeHelper<T*, true>
{
	typedef T* ParamType;
	typedef const T* ConstParamType;
};


template<class T >
struct TRemoveConst
{
	typedef T Type;
};

template<class T>
struct TRemoveConst<const T>
{
	typedef T Type;
};


template<typename T>
struct TCallTraitsBase
{
private:
	enum { PassByValue = TOr<TAndValue<(sizeof(T) <= sizeof(void*)), TIsPODType<T>, TIsArithmetic<T>, TIsPointer<T>::Value >>};
public:
	typedef T ValueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ParamType ParamType;
	typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ConstParamType ConstPointerType;
};

template <typename T>
struct TCallTraits : public TCallTraitsBase<T> { };

template <typename T>
struct TCallTraits<T&>
{
	typedef T& ValueType;
	typedef T& Reference;
	typedef const T& ConstReference;
	typedef T& ParamType;
	typedef T& ConstPointerType;
};

template<typename T,size_t N>
struct TCallTraits<T[N]>
{
private:
	typedef T ArrayType[N];
public:
	typedef const T* ValueType;
	typedef const ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};

template<typename T, size_t N>
struct TCallTraits<const T[N]>
{
private:
	typedef const T ArrayType[N];
public:
	typedef const T* ValueType;
	typedef ArrayType& Reference;
	typedef const ArrayType& ConstReference;
	typedef const T* const ParamType;
	typedef const T* const ConstPointerType;
};


template<typename T>
struct TTypeTraitsBase
{
	typedef typename TCallTraits<T>::ParamType ConstInitType;
	typedef typename TCallTraits<T>::ConstPointerType ConstPointerType;

	enum { IsBytewiseComparable = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T> >::Value };
};

template <typename T> struct TTypeTraits : public TTypeTraitsBase<T> {};

template<typename T> struct TContainerTraitsBase
{
	enum { MoveWillEmptyContainer = false };
};

struct XVirtualDestructor
{
	virtual ~XVirtualDestructor() {};
};

template <typename T, typename U>
struct TMoveSupportTraitsBase
{
	typedef U Copy;
};

template<typename T>
struct TMoveSupportTraitsBase<T, const T&>
{
	typedef const T& Copy;
	typedef T&& Move;
};

template <typename T>
struct TMoveSupportTraits : public TMoveSupportTraitsBase<T, typename TCallTraits<T>::ParamType>
{

};



template<typename T,typename Arg>
struct TIsBitwiseConstructible
{
	static_assert(
		!TIsReferenceType<T>::Value &&
		!TIsReferenceType<Arg>::Value,
		"TIsBitwiseConstructible is not designed to accept reference types");

	static_assert(
		TAreTypesEqual<T,typename TRemoveCV<T>::Type>::Value && 
		TAreTypesEqual<T,typename TRemoveCV<Arg>::Type>::Value,
		"TIsBitwiseConstructible is not designed to accept qualified types");

	enum { Value = false };
};

template <typename T>
struct TIsBitwiseConstructible
{
	enum { Value = TIsTriviallyCopyConstructible<T>::Value };
};

template <typename T, typename U>
struct TIsBitwiseConstructible<const T, U> : TIsBitwiseConstructible<T, U>
{

};

template <typename T>
struct TIsBitwiseConstructible<const T*, T*>
{
	enum { Value = true };
};

template <> struct TIsBitwiseConstructible<uint8, int8> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int8, uint8> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint16, int16> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int16, uint16> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint32, int32> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int32, uint32> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible<uint64, int64> { enum { Value = true }; };
template <> struct TIsBitwiseConstructible< int64, uint64> { enum { Value = true }; };


#define GENERATE_MEMBER_FUNCTION_CHECK(MemberName, Result, ConstModifier, ...)
template <typename T>
class THasMemberFunction_##MemberName
{
	template <typename U, Result(U::*)(__VA_ARGS__) ConstModifier> struct Check;
	template <typename U> static char MemberTest(Check<U, &U::MemberName>*);
	template <typename U> static int MemberTest(...);
public:
	enum { Value = sizeof(MemberTest<T>(nullptr)) == sizeof(char) };
};

#undef IS_EMPTY
#undef IS_POD
#undef HAS_TRIVAL_CONSTRUCTOR