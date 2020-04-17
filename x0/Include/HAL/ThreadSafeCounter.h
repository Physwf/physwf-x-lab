#pragma once

#include "CoreTypes.h"
#include "HAL/PlatformAtomics.h"

class FThreadSafeCounter
{
public:
	typedef int32 IntegerType;

	FThreadSafeCounter()
	{
		Counter = 0;
	}

	FThreadSafeCounter(const FThreadSafeCounter& Other)
	{
		Counter = Other.GetValue();
	}

	FThreadSafeCounter(int32 Value)
	{
		Counter = Value;
	}

	int32 Increment()
	{
		return FPlatformAtomics::InterlockedIncrement(&Counter);
	}

	int32 Add(int32 Amount)
	{
		return FPlatformAtomics::InterlockedAdd(&Counter, Amount);
	}

	int32 Decrement()
	{
		return FPlatformAtomics::InterlockedDecrement(&Counter);
	}

	int32 Subtract(int32 Amount)
	{
		return FPlatformAtomics::InterlockedAdd(&Counter, -Amount);
	}

	int32 Set(int32 Value)
	{
		return FPlatformAtomics::InterlockedExchange(&Counter, Value);
	}

	int32 Reset()
	{
		return FPlatformAtomics::InterlockedExchange(&Counter, 0);
	}

	int32 GetValue() const
	{
		return FPlatformAtomics::AtomicRead(&const_cast<FThreadSafeCounter*>(this)->Counter);
	}
private:
	void operator=(const FThreadSafeCounter& Other) {}

	volatile int32 Counter;
};