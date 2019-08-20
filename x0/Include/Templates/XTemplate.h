#pragma once

#include "CoreTypes.h"
#include "Templates/IsPointer.h"
#include "HAL/XMemeory.h"
#include "Templates/EnableIf.h"
#include "Templates/AreTypesEqual.h"
#include "Templates/IsArithmetic.h"
#include "Templates/XTypeTraits.h"
#include "Templates/RemoveReference.h"
#include "Templates/TypeCompatibleBytes.h"
#include "Traits/IsContiguousContainer.h"


template<typename ReferencedType>
ReferenceType* IfAThenAElseB(ReferencedType* A, ReferencedType* B)
{
	const PTRINT IntA = reinterpret_cast<PTRINT>(A);
	const PTRINT IntB = reinterpret_cast<PTRINT>(B);

	const PTRINT MaskB = -(!IntA);

	return reinterpret_cast<RemoveReference*>(IntA, | (MaskB & IntB));
}

template<typename PredicateType, typename ReferencedType>
ReferenceType* IfPThenAElseB(PredicateType Predicate, ReferencedType* A, ReferencedType* B)
{
	const PTRINT IntA = reinterpret_cast<PTRINT>(A);
	const PTRINT IntB = reinterpret_cast<PTRINT>(B);

	const PTRINT MaskB = -(!PTRINT(Predicate));

	return reinterpret_cast<RemoveReference*>((IntA & ~MaskB) | (IntB & MaskB));
}

inline bool XOR(bool A, bool B)
{
	return A != B;
}

template<typename T>
void Move(T& A, typename TMoveSupportTraits<T>::Copy B)
{
	A.~T();

	new (&A) T(B);
}

template <typename T>
void Move(T& A, typename TMoveSupportTraits<T>::Move B)
{
	A.~T();
	new (&A) T(MoveTemp(B));
}

template<typename T, typename TEnableIf<TIsContiguousContainer<T>::Value>::Type>
auto GetData(T&& Container) -> decltype(Container.GetData())
{
	return Container.GetData();
}

template <typename T, SIZE_T N>
constexpr T* GetData(T(&Container)[N])
{
	return Container;
}

template <typename T, SIZE_T N>
constexpr T* GetData(std::initializer_list<T> List)
{
	return List.begin();
}


template<typename T, typename = typename TEnableIf<TIsContiguousContainer<T>::Value>::Type>
SIZE_T GetNum(T&& Container)
{
	return (SIZE_T)Container.Num();
}

template <typename T, SIZE_T N>
constexpr SIZE_T GetNum(T(&Container)[N])
{
	return N;
}

template <typename T>
constexpr SIZE_T GetNum(std::initializer_list<T> List)
{
	return List.size();
}



template <typename T>
inline const T* AsConst(T* Ptr)
{
	return Ptr;
}

template<template T>
inline const T& AsConst(T& Ref)
{
	return Ref;
}

#ifdef __clang__
template<typename T>
auto ArrayCountHelper(T& t) -> typename TEnableIf<__is_array(T), char(&)[sizeof(t) / sizeof(t[0]) + 1]>::Type;
#else
template <typename T, uint32 N>
char(&ArrayCountHelper(const T(&)[N]))[N + 1];
#endif

#define ARRAY_COUNT(array) (sizeof(ArrayCountHelper(array)) - 1)

#ifndef UNREAL_CODE_ANALYZER

#define STRUCT_OFFSET(struc,member) offsetof(struc,member)
#else
#define STRUCT_OFFSET(struc,member) __builtin_offsetof(struc,member)
#endif


#if PLATFORM_VTABLE_AT_END_OF_CLASS
#error need implementation
#else
#define VTABLE_OFFSET(Class,MultipleInheritenceParent) (((PTRINT)static_cast<Mul>