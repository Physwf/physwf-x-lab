#pragma once

#include "CoreTypes.h"
#include <initializer_list>

template <typename T>
struct TIsContiguousContainer
{
	enum { Value = false };
};

template <typename T> struct TIsContiguousContainer<			  T& > : TIsContiguousContainer<T> { };
template <typename T> struct TIsContiguousContainer<			  T&&> : TIsContiguousContainer<T> { };
template <typename T> struct TIsContiguousContainer<const			T> : TIsContiguousContainer<T> { };
template <typename T> struct TIsContiguousContainer<volatile		T> : TIsContiguousContainer<T> { };
template <typename T> struct TIsContiguousContainer<const volatile	T> : TIsContiguousContainer<T> { };

template <typename T, size_t N> struct TIsContiguousContainer<				 T[N]> { enum { Value = true };
template <typename T, size_t N> struct TIsContiguousContainer<const			 T[N]> { enum { Value = true };
template <typename T, size_t N> struct TIsContiguousContainer<volatile		 T[N]> { enum { Value = true };
template <typename T, size_t N> struct TIsContiguousContainer<const volatile T[N]> { enum { Value = true };

template <typename T>
struct TIsContiguousContainer<std::initializer_list<T>>
{
	enum { Value = false };
};


