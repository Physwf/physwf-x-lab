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

	template<typename T, typename DiffType>
	FORCEINLINE T AddExchange(volatile T* Element, DiffType Diff)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T>*)Element, (TUnderlyingIntegerType_T<T>)Diff);
		return *(const T*)&Result;
	}

	template <typename T, typename DiffType>
	FORCEINLINE T* AddExchange(T* volatile* Element, DiffType Diff)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T*>*)Element, (TUnderlyingIntegerType_T<T*>)(Diff * sizeof(T)));
		return *(T* const*)&Result;
	}

	template <typename T>
	FORCEINLINE T DecrementExchange(volatile T* Element)
	{
		auto Result = XPlatformAtomics::InterlockedDecrement((volatile TUnderlyingIntegerType_T<T>*)Element) + 1;
		return *(const T*)&Result;
	}

	template <typename T,typename DiffType>
	FORCEINLINE T* DecrementExchange(T* volatile*  Element)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T*>*)Element,-(TUnderlyingIntegerType_T<T*>)sizeof(T));
		return *(T* const*)&Result;
	}

	template<typename T, typename DiffType>
	FORCEINLINE T SubExchange(volatile T* Element, DiffType Diff)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T>*)Element, -(TUnderlyingIntegerType_T<T>)Diff);
		return *(const T*)&Result;
	}

	template <typename T, typename DiffType>
	FORCEINLINE T* AddExchange(T* volatile* Element, DiffType Diff)
	{
		auto Result = XPlatformAtomics::InterlockedAdd((volatile TUnderlyingIntegerType_T<T*>*)Element, -(TUnderlyingIntegerType_T<T*>)(Diff * sizeof(T)));
		return *(T* const*)&Result;
	}

	template <typename T>
	FORCEINLINE T CompareExchange(volatile T* Element, T ExpectedValue, T NewValue)
	{
		auto Result = XPlatformAtomics::InterlockedCompareExchange((volatile TUnderlyingIntegerType_T<T>*)Element, *(const TUnderlyingIntegerType_T<T>*)&NewValue, *(const TUnderlyingIntegerType_T<T>*)&ExpectedValue);
		return *(const T*)&Result;
	}

	template <typename T, bool bIsVoidPointer,bool bIsIntegral, bool bCanUsePlatformAtomics>
	struct TAtomicBaseType
	{
		using Type = TAtomicBase_Basic<T>
	};

	template <typename T, false, false, false>
	struct TAtomicBaseType
	{
		static_assert(sizeof(T) == 0, "TAtomic of this size are not currently supported");
		using Type = TAtomicBase_Mutex<T>;
	};

	template <typename T*, false, false, true>
	struct TAtomicBaseType
	{
		using Type = TAtomicBase_Pointer<T>;
	};

	template <typename T*, false, true, true>
	struct TAtomicBaseType
	{
		using Type = TAtomicBase_Integral<T>;
	};

	template <typename T, bool bIsVoidPointer = TIsVoidPointer<T>::Value, bool bIsIntegral = TIsIntegral<T>::Value, bool bCanUsePlatformAtomics = TIsSupportedSize<sizeof(T)>::Value>
	using TAtomicBaseType_T = typename TAtomicBaseType<T, bIsVoidPointer, bIsIntegral, bCanUsePlatformAtomics>::Type;
	
}

template <typename T>
struct alignas(XAtomic_Private::TIsSupportedSize<T>::Value ? alignof(XAtomic_Private::TUnderlyingIntegerType_T<T>) : alignof(T)) TAtomicBase_Basic
{
public:

	FORCEINLINE T Load(EMemoryOrder Order = EMemoryOrder::SequenciallyConsistent)
	{
		switch (Order)
		{
		case EMemoryOrder::Relaxed:
			return XAtomic_Private::LoadRelaxed(&Element);
		}
		return XAtomic_Private::Load(&Element);
	}

	FORCEINLINE void Store(T Value, EMemoryOrder Order = EMemoryOrder::SequenciallyConsistent)
	{
		switch (Order)
		{
		case EMemoryOrder::Relaxed:
			XAtomic_Private::StoreRelaxed(&Element, Value);
		}
		XAtomic_Private::Store(&Element);
	}

	FORCEINLINE T Exchange(T Value)
	{
		return XAtomic_Private::Exchange(&Element, Value);
	}

	FORCEINLINE bool CompareExchange(T& Expected, T Value)
	{
		T PreValue = XAtomic_Private::CompareExchange(&this->Element, Expected, Value);
		bool Result = PreValue == Expected;
		Expected = PreValue;
		return bResult;
	}

protected:
	TAtomicBase_Basic() = default;

	constexpr TAtomicBase_Basic(T Value)
		:Element(Value)
	{

	}

	volatile T Element;
};

//TODO:
template <typename T>
struct TAtomicBase_Mutex;

template <typename T, typename DiffType>
struct TAtomicBase_Arithmetic : public TAtomicBase_Basic<T>
{
public:

	FORCEINLINE operator++()
	{
		return XAtomic_Private::IncrementExchange(&this->Element);
	}

	FORCEINLINE operator++(int)
	{
		return XAtomic_Private::IncrementExchange(&this->Element) + 1;
	}

	FORCEINLINE T operator+=(DiffType Value)
	{
		return XAtomic_Private::AddExchange(&this->Element, Value) + Value;
	}

	FORCEINLINE operator--()
	{
		return XAtomic_Private::DecrementExchange(&this->Element) - 1;
	}

	FORCEINLINE operator--(int)
	{
		return XAtomic_Private::DecrementExchange(&this->Element);
	}

	FORCEINLINE T operator-=(DiffType Value)
	{
		return XAtomic_Private::SubExchange(&this->Element, Value) - Value;
	}

protected:
	TAtomicBase_Arithmetic() = default;

	constexpr TAtomicBase_Arithmetic(T Value)
		:TAtomicBase_Basic<T>(Value)
	{

	}
};

template <typename T>
struct TAtomicBase_Pointer : public TAtomicBase_Arithmetic<T, PTRINT>
{
protected:
	TAtomicBase_Pointer() = default;

	constexpr TAtomicBase_Pointer(T Value)
		:TAtomicBase_Arithmetic<T , PTRINT>(Value)
	{

	}
};

template <typename T>
struct TAtomicBase_Integral : public TAtomicBase_Arithmetic<T, T>
{
protected:
	TAtomicBase_Integral() = default;

	constexpr TAtomicBase_Integral(T Value)
		:TAtomicBase_Integral<T, T>(Value)
	{

	}
};

template <typename T>
class TAtomic final : public XAtomic_Private::TAtomicBaseType_T<T>
{
	static_assert(TIsTrival<T>::Value, "TAtomic is only usable with trivial types");

public:
	using ElementType = T;

	FORCEINLINE TAtomic() = default;

	constexpr TAtomic(T Arg):
		XAtomic_Private::TAtomicBaseType_T<T>(Arg)
	{

	}

	FORCEINLINE operator T() const
	{
		return this->Load();
	}

	FORCEINLINE T operator=(T Value)
	{
		this->Exchange(Value);
		return Value;
	}

private:
	TAtomic(TAtomic&&) = delete;
	TAtomic(const TAtomic&) = delete;
	TAtomic& operator=(TAtomic&&) = delete;
	TAtomic& operator=(const TAtomic&) = delete;
};

