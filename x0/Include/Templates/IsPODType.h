#pragma once

#include "CoreTypes.h"
#include "Templates/IsPointer.h"
#include "Templates/AndOrNot.h"
#include "Templates/IsArithmetic.h"


template<typename T>
struct TIsPODType
{
	enum { Value = TOrValue<__is_pod(T)> || __is_enum(T), TIsArithmetic<T>, TIsPointer<T>>::Value  };
};