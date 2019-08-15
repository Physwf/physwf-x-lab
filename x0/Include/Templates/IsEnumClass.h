#pragma once

#include "CoreTypes.h"
#include "Templates/AndOrNot.h"

namespace XIsEnumClass_Private
{
	template <typename T>
	struct TIsEnumConvertiableToInt
	{
		static char (&Resolve(int))[2];
		static char Resovle(...);

		enum { Value = sizeof(Resovle(T())) - 1 };
	};
}

template <typename T>
struct TIsEnumClass
{
	enum { Value = TAndValue<__is_enum(T),TNot<XIsEnumClass_Private::TIsEnumConvertiableToInt<T>> >::Value };
}