#pragma once

#include "CoreTypes.h"
#include "HAL/PlatformAtomics.h"

class XThreadSafeCounter
{
public:
	typedef int32 IntegerType;

	XThreadSafeCounter()
	{
		Counter = 0;
	}

	XThreadSafeCounter(const XThreadSafeCounter& Other)
	{
		Counter = Other.GetValue();
	}

	int32 Increment()
	{
		return XPlatformAtomics::InterlockedIncrement(&Counter);
	}

	int32 Add(int32 Amount)
	{
		return XPlatformAtomics::InterlockedAdd(&Counter, Amount);
	}

	int32 Decrement()
	{
		return XPlatformAtomics::InterlockedDecrement(&Counter);
	}

	int32 Subtract(int32 Amount)
	{
		return XPlatformAtomics::InterlockedAdd(&Counter, -Amount);
	}

	int32 Set(int32 Value)
	{
		return XPlatformAtomics::InterlockedExchange(&Counter, Value);
	}

	int32 Reset()
	{
		return XPlatformAtomics::InterlockedExchange(&Counter, 0);
	}

	int32 GetValue() const
	{
		return XPlatformAtomics::AtomicRead(&const_cast<XThreadSafeCounter*>(this)->Counter);
	}

private:
	void operator=(const XThreadSafeCounter& Other) { }

	volatile int32 Counter;
};