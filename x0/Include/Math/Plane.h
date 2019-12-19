#pragma once

#include "Vector.h"
#include "Vector4.h"
#include "XMath.h"

struct alignas(16) FPlane
	: public FVector
{
public:

	/** The w-component. */
	float W;

public:
	/** Default constructor (no initialization). */
	inline FPlane() {}

	inline FPlane(const FPlane& P) : FVector(P), W(P.W) {}

	inline FPlane(const FVector4& V) : FVector(V), W(V.W) {}

	inline FPlane(float InX, float InY, float InZ, float InW)
		: FVector(InX, InY, InZ)
		, W(InW)
	{}

	inline FPlane(FVector InNormal, float InW)
		: FVector(InNormal), W(InW)
	{}

	inline FPlane(FVector InBase, const FVector &InNormal)
		: FVector(InNormal)
		, W(InBase | InNormal)
	{}

	FPlane(FVector A, FVector B, FVector C)
		: FVector(((B - A) ^ (C - A)).GetSafeNormal())
	{
		W = A | (FVector)(*this);
	}


	inline float PlaneDot(const FVector &P) const
	{
		return X * P.X + Y * P.Y + Z * P.Z - W;
	}

	bool Normalize(float Tolerance = SMALL_NUMBER)
	{
		const float SquareSum = X * X + Y * Y + Z * Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = FMath::InvSqrt(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
			return true;
		}
		return false;
	}

	FPlane Flip() const
	{
		return FPlane(-X, -Y, -Z, -W);
	}

	FPlane TransformBy(const FMatrix& M) const
	{
		return (X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W);
	}

	FPlane TransformByUsingAdjointT(const FMatrix& M, float DetM, const FMatrix& TA) const;

	bool operator!=(const FPlane& V) const
	{
		return (X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W);
	}

	bool Equals(const FPlane& V, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return (FMath::Abs(X - V.X) < Tolerance) && (FMath::Abs(Y - V.Y) < Tolerance) && (FMath::Abs(Z - V.Z) < Tolerance) && (FMath::Abs(W - V.W) < Tolerance);
	}

	inline float operator|(const FPlane& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z + W * V.W;
	}

	FPlane operator+(const FPlane& V) const
	{
		return FPlane(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}

	FPlane operator-(const FPlane& V) const
	{
		return FPlane(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}

	FPlane operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return FPlane(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}

	FPlane operator*(float Scale) const
	{
		return FPlane(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}

	FPlane operator*(const FPlane& V)
	{
		return FPlane(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}

	FPlane operator+=(const FPlane& V)
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}

	FPlane operator-=(const FPlane& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}

	FPlane operator*=(float Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
		return *this;
	}

	FPlane operator*=(const FPlane& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}

	FPlane operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV; Z *= RV; W *= RV;
		return *this;
	}
};
