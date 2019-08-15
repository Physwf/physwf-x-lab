#pragma once

#include "CoreTypes.h"

template <typename T>			struct TRemoveExtent		{ typedef T Type; };
template <typename T>			struct TRemoveExtent<T[]>	{ typedef T Type; };
template <typename T,uint32 N>	struct TRemoveExtent<T[N]>	{ typedef T Type; };