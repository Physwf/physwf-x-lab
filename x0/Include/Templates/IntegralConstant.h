#pragma once

#include "CoreTypes.h"

template <typename T, T Val>
struct TIntegralConstant
{
	static const T Value = Val;
};