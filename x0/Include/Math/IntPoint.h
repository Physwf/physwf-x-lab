#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Math/XMathUtility.h"

#include "X0.h"

struct FIntPoint
{
	int32 X, Y;
public:
	X0_API static const FIntPoint ZeroValue;
	X0_API static const FIntPoint NoneValue;
public:
	FIntPoint();

	FIntPoint(int32 InX, int32 InY);

public:
	const int32& operator()(int32 PointIndex) const;
	int32& operator()(int32 PointIndex);

	bool operator==(const FIntPoint& Other) const;
	bool operator!=(const FIntPoint& Other) const;
	FIntPoint& operator*=(int32 Scale);
	FIntPoint& operator/=(int32 Divisor);
	FIntPoint& operator+=(const FIntPoint& Other);
	FIntPoint& operator-=(const FIntPoint& Other);
	FIntPoint& operator/=(const FIntPoint& Other);
	FIntPoint& operator=(const FIntPoint& Other);
	FIntPoint operator*(int32 Scale) const;
	FIntPoint operator/(int32 Divisor) const;
	FIntPoint operator+(const FIntPoint& Other) const;
	FIntPoint operator-(const FIntPoint& Other) const;
	FIntPoint operator/(const FIntPoint& Other) const;
	int32& operator[](int32 Index);
	int32 operator[](int32 Index) const;
public:
	int32 Size() const;
	int32 SizeSquared() const;
};

inline FIntPoint::FIntPoint()
{
}

inline FIntPoint::FIntPoint(int32 InX, int32 InY)
	: X(InX)
	, Y(InY)
{
}

inline const int32& FIntPoint::operator()(int32 PointIndex) const
{
	return (&X)[PointIndex];
}

inline int32& FIntPoint::operator()(int32 PointIndex)
{
	return (&X)[PointIndex];
}

inline bool FIntPoint::operator==(const FIntPoint& Other) const
{
	return X == Other.X && Y == Other.Y;
}

inline bool FIntPoint::operator!=(const FIntPoint& Other) const
{
	return (X != Other.X) || (Y != Other.Y);
}

inline FIntPoint& FIntPoint::operator*=(int32 Scale)
{
	X *= Scale;
	Y *= Scale;

	return *this;
}

inline FIntPoint& FIntPoint::operator/=(int32 Divisor)
{
	X /= Divisor;
	Y /= Divisor;

	return *this;
}

inline int32& FIntPoint::operator[](int32 Index)
{
	check(Index >= 0 && Index < 2);
	return ((Index == 0) ? X : Y);
}

inline int32 FIntPoint::operator[](int32 Index) const
{
	check(Index >= 0 && Index < 2);
	return ((Index == 0) ? X : Y);
}

inline int32 FIntPoint::Size() const
{
	int64 X64 = (int64)X;
	int64 Y64 = (int64)Y;
	return int32(FMath::Sqrt(float(X64 * X64 + Y64 * Y64)));
}

inline int32 FIntPoint::SizeSquared() const
{
	return X * X + Y * Y;
}

inline FIntPoint& FIntPoint::operator+=(const FIntPoint& Other)
{
	X += Other.X;
	Y += Other.Y;

	return *this;
}

inline FIntPoint& FIntPoint::operator-=(const FIntPoint& Other)
{
	X -= Other.X;
	Y -= Other.Y;

	return *this;
}

inline FIntPoint& FIntPoint::operator/=(const FIntPoint& Other)
{
	X /= Other.X;
	Y /= Other.Y;

	return *this;
}

inline FIntPoint& FIntPoint::operator=(const FIntPoint& Other)
{
	X = Other.X;
	Y = Other.Y;

	return *this;
}

inline FIntPoint FIntPoint::operator*(int32 Scale) const
{
	return FIntPoint(*this) *= Scale;
}

inline FIntPoint FIntPoint::operator/(int32 Divisor) const
{
	return FIntPoint(*this) /= Divisor;
}

inline FIntPoint FIntPoint::operator+(const FIntPoint& Other) const
{
	return FIntPoint(*this) += Other;
}

inline FIntPoint FIntPoint::operator-(const FIntPoint& Other) const
{
	return FIntPoint(*this) -= Other;
}

inline FIntPoint FIntPoint::operator/(const FIntPoint& Other) const
{
	return FIntPoint(*this) /= Other;
}
