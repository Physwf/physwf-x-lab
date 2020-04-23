#pragma once

#include "Templates/IsPODType.h"
#include "Templates/IsEnumClass.h"

template <bool> struct TEnumAsByte_EnumClass;
template <> struct DEPRECATED(4.15, "TEnumAsByte is not intended for use with enum classes - please derive your enum class from uint8 instead.") TEnumAsByte_EnumClass<true> {};
template <> struct TEnumAsByte_EnumClass<false> {};

/**
 * Template to store enumeration values as bytes in a type-safe way.
 */
template<class TEnum>
class TEnumAsByte
{
	typedef TEnumAsByte_EnumClass<TIsEnumClass<TEnum>::Value> Check;

public:
	typedef TEnum EnumType;

	FORCEINLINE TEnumAsByte() {}

	FORCEINLINE TEnumAsByte(const TEnumAsByte& InValue)
		: Value(InValue.Value)
	{}

	FORCEINLINE TEnumAsByte(TEnum InValue)
		: Value(static_cast<uint8>(InValue))
	{ }

	explicit FORCEINLINE TEnumAsByte(int32 InValue)
		: Value(static_cast<uint8>(InValue))
	{ }

	explicit FORCEINLINE TEnumAsByte(uint8 InValue)
		: Value(InValue)
	{ }
public:
	FORCEINLINE TEnumAsByte& operator=(TEnumAsByte InValue)
	{
		Value = InValue.Value;
		return *this;
	}
	FORCEINLINE TEnumAsByte& operator=(TEnum InValue)
	{
		Value = static_cast<uint8>(InValue);
		return *this;
	}
	bool operator==(TEnum InValue) const
	{
		return static_cast<TEnum>(Value) == InValue;
	}
	bool operator==(TEnumAsByte InValue) const
	{
		return Value == InValue.Value;
	}
	operator TEnum() const
	{
		return (TEnum)Value;
	}
private:
	uint8 Value;

	FORCEINLINE friend uint32 GetTypeHash(const TEnumAsByte& Enum)
	{
		return GetTypeHash(Enum.Value);
	}
};