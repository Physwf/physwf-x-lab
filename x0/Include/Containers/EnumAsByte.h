#pragma once

#include "CoreTypes.h"

template <typename TEnum>
class TEnumAsByte
{
public:
	typedef TEnum EnumType;

	TEnumAsByte() {};

	TEnumAsByte(const TEnumAsByte& &InValue)
		:Value(InValue.Value)
	{

	}

	TEnumAsByte(TEnum InValue)
		:Value(static_cast<uint8>(InValue))
	{

	}

	explicit TEnumAsByte(int32 InValue)
		:Value(static_cast<uint8>(InValue))
	{

	}

	explicit TEnumAsByte(uint8 InValue)
		:Value(InValue)
	{

	}
	
public:
	TEnumAsByte& operator=(TEnumAsByte InValue)
	{
		Value = InValue;
		return *this;
	}

	TEnumAsByte& operator=(TEnum InValue)
	{
		Value = static_cast<uint8>(InValue);
		return *this;
	}

	bool operator==(TEnumAsByte InValue)
	{
		return Value == InValue.Value;
	}

	operator TEnum() const 
	{
		return (TEnum)Value;
	}
private:

	uint8 Value;
}