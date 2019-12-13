#pragma once

#include "CoreTypes.h"
#include "Vector2D.h"
#include "Vector4.h"
#include "Color.h"

struct FVector
{
	float X, Y, Z;

	FVector() {}

	explicit FVector(float Value) :X(Value), Y(Value), Z(Value) {}

	FVector(float InX, float InY, float InZ) :X(InX), Y(InY), Z(InZ) {}

	explicit FVector(const FVector2D V, float InZ) :X(V.X), Y(V.Y), Z(InZ) {}

	FVector(const FVector4& V) :X(V.X), Y(V.Y), Z(V.Z) {}

	explicit FVector(const FLinearColor& InColor) :X(InColor.R), Y(InColor.G), Z(InColor.B) {}


	FVector operator^(const FVector& V) const
	{
		return FVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
	static FVector CrossProduct(const FVector& A, const FVector& B)
	{
		return A ^ B;
	}

	float operator|(const FVector& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z;
	}
	static float DotProduct(const FVector& A, const FVector& B)
	{
		return A | B;
	}

	FVector operator+(const FVector& V) const
	{
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}
	FVector operator-(const FVector& V) const
	{
		return FVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	FVector operator-(float Bias) const
	{
		return FVector(X - Bias, Y - Bias, Z - Bias);
	}

	FVector operator+(float Bias) const
	{
		return FVector(X + Bias, Y + Bias, Z + Bias);
	}

	FVector operator*(float Scale) const
	{
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	FVector operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return FVector(X * RScale, Y * RScale, Z * RScale);
	}

	FVector operator*(const FVector& V) const
	{
		return FVector(X * V.X, Y * V.Y, Z * V.Z);
	}

	FVector operator/(const FVector& V) const
	{
		return FVector(X / V.X, Y / V.Y, Z / V.Z);
	}

	bool operator==(const FVector& V) const
	{
		return X == V.X && Y == V.Y && Z == V.Z;
	}

	bool operator!=(const FVector& V) const
	{
		return X != V.X || Y != V.Y || Z != V.Z;
	}

	//bool Equals(const XVector& V, float Tolerance = KINDA_SMALL_NUMBER) const;


	FVector operator-() const
	{
		return FVector(-X, -Y, -Z);
	}

	FVector operator+=(const FVector& V)
	{
		X += V.X; Y += V.Y; Z += V.Z;
		return *this;
	}
	
	FVector operator-=(const FVector& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z;
		return *this;
	}

	FVector operator*=(float Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale;
		return *this;
	}

	FVector operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV; Z *= RV;
		return *this;
	}

	FVector operator*=(const FVector& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z;
		return *this;
	}

	FVector operator/=(const FVector& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z;
		return *this;
	}

	float Size() const
	{
		return FMath::Sqrt(X*X + Y * Y + Z * Z);
	}

	float SizeSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	bool inline Normalize(float Tolerance = SMALL_NUMBER)
	{
		const float SquareSum = X * X + Y * Y + Z * Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = FMath::InvSqrt(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale;
			return true;
		}
		return false;
	}

	static inline float DistSquared(const FVector &V1, const FVector &V2)
	{
		return FMath::Sqrt(FVector::DistSquared(V1, V2));
	}
};

inline FVector2D::FVector2D(const FVector& V)
	: X(V.X), Y(V.Y)
{
}

FVector4::FVector4(const FVector& InVector, float InW /*= 1.0f*/)
	: X(InVector.X)
	, Y(InVector.Y)
	, Z(InVector.Z)
	, W(InW)
{
}

inline FVector operator*(float Scale, const FVector& V)
{
	return V.operator*(Scale);
}

