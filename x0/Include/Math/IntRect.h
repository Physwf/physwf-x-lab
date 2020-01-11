#pragma once

#include "CoreTypes.h"
#include "Math/XMathUtility.h"
#include "Math/IntPoint.h"
#include "Math/Vector2D.h"

struct FIntRect
{
	FIntPoint Min;

	FIntPoint Max;

public:
	FIntRect();

	FIntRect(int32 X0, int32 Y0, int32 X1, int32 Y1);

	FIntRect(FIntPoint InMin, FIntPoint InMax);
public:
	const FIntPoint& operator()(int32 PointIndex) const;
	FIntPoint& operator()(int32 PointIndex);

	bool operator==(const FIntRect& Other) const;
	bool operator!=(const FIntRect& Other) const;
	FIntRect& operator*=(int32 Scale);
	FIntRect& operator+=(const FIntPoint& Point);
	FIntRect& operator-=(const FIntPoint& Point);
	FIntRect operator*(int32 Scale) const;
	FIntRect operator/(int32 Div) const;
	FIntRect operator+(const FIntPoint& Point) const;
	FIntRect operator/(const FIntPoint& Point) const;
	FIntRect operator-(const FIntPoint& Point) const;
	FIntRect operator+(const FIntRect& Other) const;
	FIntRect operator-(const FIntRect& Other) const;
public:
	int32 Area() const;
	FIntRect Bottom(int32 InHeight) const;
	void Clip(const FIntRect& Other);
	void Union(const FIntRect& Other);
	bool Contains(FIntPoint Point) const;
	void GetCenterAndExtents(FIntPoint& OutCenter, FIntPoint& OutExtent) const;
	int32 Height() const;
	void InflateRect(int32 Amount);
	void Include(FIntPoint Point);
	FIntRect Inner(FIntPoint Shrink) const;
	FIntRect Right(int32 InWidth) const;
	FIntRect Scale(float Fraction) const;
	FIntPoint Size() const;
	int32 Width() const;
	bool IsEmpty() const;
};

inline FIntRect::FIntRect()
{

}

inline FIntRect::FIntRect(int32 X0, int32 Y0, int32 X1, int32 Y1)
	: Min(X0, Y0)
	, Max(X1, Y1)
{
}

inline FIntRect::FIntRect(FIntPoint InMin, FIntPoint InMax)
	: Min(InMin)
	, Max(InMax)
{
}

inline const FIntPoint& FIntRect::operator()(int32 PointIndex) const
{
	return (&Min)[PointIndex];
}

inline FIntPoint& FIntRect::operator()(int32 PointIndex)
{
	return (&Min)[PointIndex];
}

inline bool FIntRect::operator==(const FIntRect& Other) const
{
	return Min == Other.Min && Max == Other.Max;
}

inline bool FIntRect::operator!=(const FIntRect& Other) const
{
	return Min != Other.Min || Max != Other.Max;
}

inline FIntRect& FIntRect::operator*=(int32 Scale)
{
	Min *= Scale;
	Max *= Scale;

	return *this;
}

inline FIntRect& FIntRect::operator+=(const FIntPoint& Point)
{
	Min += Point;
	Max += Point;

	return *this;
}

inline FIntRect& FIntRect::operator-=(const FIntPoint& Point)
{
	Min -= Point;
	Max -= Point;

	return *this;
}

inline FIntRect FIntRect::operator*(int32 Scale) const
{
	return FIntRect(Min * Scale, Max * Scale);
}

inline FIntRect FIntRect::operator/(int32 Div) const
{
	return FIntRect(Min / Div, Max / Div);
}

inline FIntRect FIntRect::operator+(const FIntPoint& Point) const
{
	return FIntRect(Min + Point, Max + Point);
}

inline FIntRect FIntRect::operator/(const FIntPoint& Point) const
{
	return FIntRect(Min / Point, Max / Point);
}

inline FIntRect FIntRect::operator+(const FIntRect& Other) const
{
	return FIntRect(Min + Other.Min, Max + Other.Max);
}

inline FIntRect FIntRect::operator-(const FIntPoint& Point) const
{
	return FIntRect(Min - Point, Max - Point);
}

inline FIntRect FIntRect::operator-(const FIntRect& Other) const
{
	return FIntRect(Min - Other.Min, Max - Other.Max);

}

inline int32 FIntRect::Area() const
{
	return (Max.X - Min.X) * (Max.Y - Min.Y);
}

inline FIntRect FIntRect::Bottom(int32 InHeight) const
{
	return FIntRect(Min.X, FMath::Max(Min.Y, Max.Y - InHeight), Max.X, Max.Y);
}

inline void FIntRect::Clip(const FIntRect& R)
{
	Min.X = FMath::Max<int32>(Min.X, R.Min.X);
	Min.Y = FMath::Max<int32>(Min.Y, R.Min.Y);
	Max.X = FMath::Min<int32>(Max.X, R.Max.X);
	Max.Y = FMath::Min<int32>(Max.Y, R.Max.Y);

	// return zero area if not overlapping
	Max.X = FMath::Max<int32>(Min.X, Max.X);
	Max.Y = FMath::Max<int32>(Min.Y, Max.Y);
}

inline void FIntRect::Union(const FIntRect& R)
{
	Min.X = FMath::Min<int32>(Min.X, R.Min.X);
	Min.Y = FMath::Min<int32>(Min.Y, R.Min.Y);
	Max.X = FMath::Max<int32>(Max.X, R.Max.X);
	Max.Y = FMath::Max<int32>(Max.Y, R.Max.Y);
}

inline bool FIntRect::Contains(FIntPoint Point) const
{
	return Point.X >= Min.X && Point.X < Max.X && Point.Y >= Min.Y && Point.Y < Max.Y;
}

inline void FIntRect::GetCenterAndExtents(FIntPoint& OutCenter, FIntPoint& OutExtent) const
{
	OutExtent.X = (Max.X - Min.X) / 2;
	OutExtent.Y = (Max.Y - Min.Y) / 2;

	OutCenter.X = Min.X + OutExtent.X;
	OutCenter.Y = Min.Y + OutExtent.Y;
}

inline int32 FIntRect::Height() const
{
	return (Max.Y - Min.Y);
}

inline void FIntRect::InflateRect(int32 Amount)
{
	Min.X -= Amount;
	Min.Y -= Amount;
	Max.X += Amount;
	Max.Y += Amount;
}

inline void FIntRect::Include(FIntPoint Point)
{
	Min.X = FMath::Min(Min.X, Point.X);
	Min.Y = FMath::Min(Min.Y, Point.Y);
	Max.X = FMath::Max(Max.X, Point.X);
	Max.Y = FMath::Max(Max.Y, Point.Y);
}

inline FIntRect FIntRect::Inner(FIntPoint Shrink) const
{
	return FIntRect(Min + Shrink, Max - Shrink);
}

inline FIntRect FIntRect::Right(int32 InWidth) const
{
	return FIntRect(FMath::Max(Min.X, Max.X - InWidth), Min.Y, Max.X, Max.Y);
}

inline FIntRect FIntRect::Scale(float Fraction) const
{
	FVector2D Min2D = FVector2D((float)Min.X, (float)Min.Y) * Fraction;
	FVector2D Max2D = FVector2D((float)Max.X, (float)Max.Y) * Fraction;

	return FIntRect(FMath::FloorToInt(Min2D.X), FMath::FloorToInt(Min2D.Y), FMath::CeilToInt(Max2D.X), FMath::CeilToInt(Max2D.Y));
}

inline FIntPoint FIntRect::Size() const
{
	return FIntPoint(Max.X - Min.X, Max.Y - Min.Y);
}

inline int32 FIntRect::Width() const
{
	return Max.X - Min.X;
}

inline bool FIntRect::IsEmpty() const
{
	return Width() == 0 && Height() == 0;
}
