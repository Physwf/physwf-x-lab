#pragma once

#include "Templates/AndOrNot.h"
#include "Templates/IsTriviallyDestructiable.h"
#include "Templates/IsTriviallyCopyConstructible.h"
#include "Templates/IsTriviallyCopyAssignable.h"

template <typename T>
struct TIsTrival
{
	enum { Value = TAnd<TIsTriviallyDestructible<T>,TIsTrivallyCopyAssignable<T> , TIsTriviallyCopyConstructible<T>>::Value };
};