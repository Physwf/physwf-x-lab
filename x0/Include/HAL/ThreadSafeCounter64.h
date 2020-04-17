#pragma once

#include "CoreTypes.h"
#include "HAL/ThreadSafeCounter.h"

class FThreadSafeCounter64
{
public:
	typedef int64 IntegerType;

	FThreadSafeCounter64()
	{
		Counter = 0;
	}

	FThreadSafeCounter64(const FThreadSafeCounter& Other)
	{
		Counter = Other.GetValue();
	}

	FThreadSafeCounter64& operator=(const FThreadSafeCounter64& Other)
	{
		if (this == &Other) return *this;
		Set(Other.GetValue());
		return *this;
	}

	FThreadSafeCounter64(int64 Value)
	{
		Counter = Value;
	}

	int64 Increment()
	{
		return FPlatformAtomics::InterlockedIncrement(&Counter);
	}

	int64 Add(int64 Amount)
	{
		return FPlatformAtomics::InterlockedAdd(&Counter, Amount);
	}

	int64 Decrement()
	{
		return FPlatformAtomics::InterlockedDecrement(&Counter);
	}

	int64 Subtract(int64 Amount)
	{
		return FPlatformAtomics::InterlockedAdd(&Counter, -Amount);
	}

	int64 Set(int64 Value)
	{
		return FPlatformAtomics::InterlockedExchange(&Counter, Value);
	}

	int64 Reset()
	{
		return FPlatformAtomics::InterlockedExchange(&Counter, 0);
	}

	int64 GetValue() const
	{
		return Counter;
	}

private:
	/** Thread-safe counter */
	volatile int64 Counter;
};