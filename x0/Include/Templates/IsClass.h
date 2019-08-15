#pragma once

#include "CoreTypes.h"

template <typename T>
struct TIsClass
{
private:
	template <typename U> static uint16 Func(int U::*);
	template <typename U> static uint8 Func(...);
public:
	enum { Value = !__is_union(T) && sizeof(Func<T>(0) - 1) };
};
