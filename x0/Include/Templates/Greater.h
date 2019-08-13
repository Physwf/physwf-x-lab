#pragma once

#include "CoreTypes.h"

template <typename T = void>
struct TGreater
{
	bool operator()(const T& A, const T& B) const
	{
		return B < A;
	}
};

template<>
struct TGreater<void>
{
	template<typename T>
	bool operator()(const T& A, const T& B) const
	{
		return B < A;
	}
};