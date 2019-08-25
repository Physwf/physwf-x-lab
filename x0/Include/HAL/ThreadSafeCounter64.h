#pragma once

#include "CoreTypes.h"
#include "HAL/PlatformAtomics.h"

class XThreadSafeCounter64
{
public:
	typedef int64 IntegerType;

	XThreadSafeCounter64()
	{
		Counter = 0;
	}

	XThreadSafeCounter64(const XThreadSafeCounter64& Other)
	{
		Counter = Other.GetValue();
	}

	int64 Increment()
	{
		return XPlatformAtomics::InterlockedIncrement(&Counter);
	}

	int64 Add(int64 Amount)
	{
		return XPlatformAtomics::InterlockedAdd(&Counter, Amount);
	}

	int64 Decrement()
	{
		return XPlatformAtomics::InterlockedDecrement(&Counter);
	}

	int64 Subtract(int64 Amount)
	{
		return XPlatformAtomics::InterlockedAdd(&Counter, -Amount);
	}

	int64 Set(int64 Value)
	{
		return XPlatformAtomics::InterlockedExchange(&Counter, Value);
	}

	int64 Reset()
	{
		return XPlatformAtomics::InterlockedExchange(&Counter, 0);
	}

	int64 GetValue() const
	{
		return Counter;
	}

private:
	void operator=(const XThreadSafeCounter64& Other) { }

	volatile int64 Counter;
};