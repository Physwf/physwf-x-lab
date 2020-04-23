#pragma once

#include "CoreTypes.h"

#include "X0.h"

class X0_API FTlsAutoCleanup
{
public:
	virtual ~FTlsAutoCleanup()
	{}

	void Register();
};

template<class T>
class TTlsAutoCleanup
{
public:
	/** Constructor. */
	TTlsAutoCleanupValue(const T& InValue)
		: Value(InValue)
	{ }

	/** Gets the value. */
	T Get() const
	{
		return Value;
	}

	/* Sets the value. */
	void Set(const T& InValue)
	{
		Value = InValue;
	}

	/* Sets the value. */
	void Set(T&& InValue)
	{
		Value = MoveTemp(InValue);
	}

private:

	/** The value. */
	T Value;
};