#pragma once

#include "Templates/AndOrNot.h"
#include "Templates/IsArithmetic.h"
#include "Templates/IsPointer.h"
#include "Templates/IsEnum.h"

namespace XGetTypeHashExists_Private
{
	struct FNotSpecified {};

	template <typename T>
	struct FReturnValueCheck
	{
		static char (&Func())[2];
	};

	template<>
	struct FReturnValueCheck<FNotSpecified>
	{
		static char (&Func())[1];
	};

	template <typename T>
	FNotSpecified GetTypeHash(const T&);

	template <typename T>
	const T& Make();

	template<typename T, bool bIsHashableScalarType = TOr<TIsArithmetic<T>,TIsPointer<T>,TIsEnum<T>>::Value>
	struct GetTypeHashQuery
	{
		enum { Value = true };
	};

	template <typename T>
	struct GetTypeHashQuery<T, false>
	{
		enum { Value = sizeof(FReturnValueCheck<decltype(GetTypeHash(Make<T>()))>::Func()) == sizeof(char[2]) };
	};
}

template <typename T>
struct THasGetTypeHash
{
	enum { Value = XGetTypeHashExists_Private::GetTypeHashQuery<T>::Value };
};