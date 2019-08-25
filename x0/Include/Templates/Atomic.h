#pragma once

#include "HAL/ThreadSafeCounter.h"
#include "HAL/ThreadSafeCounter64.h"
#include "Templates/IsIntegral.h"
#include "Templates/IsTrival.h"
#include "Traits/IntType.h"

template <typename T>
struct TAtomicBase_Basic;

template<typename ElementType>
struct TAtomicBase_Mutex;

template <typename ElementType, typename DiffType>
struct TAtomicBase_Arithmetic;

template<typename ElementType>
struct TAtomicBase_Pointer;

template<typename ElementType>
struct TAtomicBase_Integral;

enum class EMemoryOrder
{
	Relaxed,
	SequenciallyConsistent
};

namespace XAtomic_Private
{
	template <int Size>
	struct TIsSupportedSize
	{
		enum { Value = (Size == 1) || (Size == 2) || (Size == 4)  || (Size == 8) };
	};

	template <typename T>
	using TUnderlyingIntegerType_T = TSignedIntType_T<sizeof(T)>;

	template <typename T>	struct TIsVoidPointer							{ static constexpr bool Value = false; };
	template				struct TIsVoidPointer<					void*>	{ static constexpr bool Value = true; };
	template				struct TIsVoidPointer<const				void*>	{ static constexpr bool Value = true; };
	template				struct TIsVoidPointer<		volatile	void*>	{ static constexpr bool Value = true; };
	template				struct TIsVoidPointer<const volatile	void*>	{ static constexpr bool Value = true; };

	template<typename T>
	FORCEINLINE T LoadRelaxed(const volatile T* Element)
	{
#ifdef __clang__
		TUnderlyingIntegerType_T<T> Result;
		__atomic_load();
#else
		auto Result = *(volatile TUnderlyingIntegerType_T<T>*) Element;
#endif
		return *(const T*)&Result;
	}

	template<typename T>
	FORCEINLINE T Load(const volatile T* Element)
	{
		auto Result = XPlatformAtomics::AtomicRead((volatile TUnderlyingIntegerType_T<T>*)Element);
		return *(const T*)&Result;
	}

	template<typename T>
	FORCEINLINE void StoreRelaxed(const volatile T* Element, T Value)
	{
#ifdef __clang__
#else
		*(volatile TUnderlyingIntegerType_T<T>*)Element = *(TUnderlyingIntegerType_T<T>*)(&Value);
	}

	template<typename T>
	FORCEINLINE T Exchange(volatile T* Element, T Value)
	{
		auto Result = XPlatformAtomics::InterlockedExchange((volatile TUnderlyingIntegerType_T<T>*)Element, *(const TUnderlyingIntegerType_T<T>*)&Value);
		return *(const T*)Result;
	}

	template<typename T>
	FORCEINLINE T IncrementExchange(volatile T* Element)
	{
		auto Result = XPlatformAtomics::InterlockedIncrement((volatile TUnderlyingIntegerType_T<T>*)Element) - 1;
		return *(const T*)&Result;
	}

	template <typename T>
	FORCEINLINE T* IncrementExchange(T* volatile* Element)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T*>*)Element, sizeof(T));
		return *(T* const*)&Result;
	}
}