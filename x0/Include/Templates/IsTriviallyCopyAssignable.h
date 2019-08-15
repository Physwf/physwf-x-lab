#pragma once

#include "CoreTypes.h"
#include "Templates/AndOrNot.h"
#include "Templates/IsPODType.h"

template <typename T>
struct TIsTrivallyCopyAssignable
{
	enum { Value = TOrValue<__has_trival_assign(T), TIsPODType<T> >::Value };
};