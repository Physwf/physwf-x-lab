#pragma once

#include "Templates/RemoveCV.h"
#include "Templates/RemoveReference.h"

namespace XDecay_Private
{
	template <typename T>
	struct TDecayNoReference
	{
		typedef typename TRemoveCV<T>::Type Type;
	};

	template <typename T>
	struct TDecayNoReference<T[]>
	{
		typedef T* Type;
	};

	template <typename T, uint32 N>
	struct TDecayNoReference<T[N]>
	{
		typedef T* Type;
	};

	template <typename RetType, typename... Params>
	struct TDecayNoReference<RetType(Params...)>
	{
		typedef RetType(*Type)(Params...);
	};
}

template <typename T>
struct TDecay
{
	typedef typename XDecay_Private::TDecayNoReference<typename TRemoveReference<T>::Type>::Type Type;
};