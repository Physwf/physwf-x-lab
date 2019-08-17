#pragma once

#include "CoreTypes.h"

template <typename...>
struct TMaxSizeof;

template <>
struct TMaxSizeof<>
{
	static const uint32 Value = 0;
};

template<typename T, typename... TRest>
struct TMaxSizeof<T, TRest...>
{
	static const uint32 Value = sizeof(T) > TMaxSizeof<TRest...>::Value ? sizeof(T) : TMaxSizeof<TRest...>::Value;
};