#pragma once

#include "CoreTypes.h"

template <typename T>			struct TIsArray			{ enum { Value = false }; };
template <typename T>			struct TIsArray<T[]>	{ enum { Value = true  }; };
template <typename T,uint32 N>	struct TIsArray<T[N]>	{ enum { Value = true  }; };

template <typename T>			struct TIsBoundArray		{ enum { Value = false }; };
template <typename T, uint32 N>	struct TIsBoundArray<T[N]>	{ enum { Value = false }; };

template <typename T>			struct TIsUnboundedArray		{ enum { Value = false }; };
template <typename T>			struct TIsUnboundedArray<T[]>	{ enum { Value = false }; };


