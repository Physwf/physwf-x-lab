#pragma once

#include "X0.h"

#include "CoreTypes.h"
#include "Math/XMath.h"

struct FVector;

struct FVector2D
{
	float X, Y;

public:

	/** Global 2D zero vector constant (0,0) */
	static X0_API const FVector2D ZeroVector;

	/** Global 2D unit vector constant (1,1) */
	static X0_API const FVector2D UnitVector;

	FVector2D() { }

	FVector2D(float InX, float InY) :X(InX), Y(InY) { }

	explicit inline FVector2D(const FVector& V);

	FVector2D operator+(const FVector2D& V) const
	{
		return FVector2D(X + V.X, Y + V.Y);
	}

	FVector2D operator-(const FVector2D& V) const
	{
		return FVector2D(X - V.X, Y - V.Y);
	}

	FVector2D operator*(float Scale) const
	{
		return FVector2D(X * Scale, Y * Scale);
	}

	FVector2D operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return FVector2D(X * RScale, Y * RScale);
	}

	FVector2D operator+(float A) const
	{
		return FVector2D(X + A, Y + A);
	}

	FVector2D operator-(float A) const
	{
		return FVector2D(X - A, Y - A);
	}

	FVector2D operator*(const FVector2D& V) const
	{
		return FVector2D(X * V.X, Y * V.Y);
	}

	FVector2D operator/(const FVector2D& V) const
	{
		return FVector2D(X / V.X, Y / V.Y);
	}

	float operator|(const FVector2D& V) const
	{
		return X * V.X + Y * V.Y;
	}

	float operator^(const FVector2D& V) const
	{
		return X * V.Y - Y * V.X;
	}

	bool operator==(const FVector2D& V) const
	{
		return X == V.X && Y == V.Y;
	}

	bool operator!=(const FVector2D& V) const
	{
		return X != V.X || Y != V.Y;
	}

	bool operator<(const FVector2D& Other) const
	{
		return X < Other.X && Y < Other.Y;
	}

	bool operator>(const FVector2D& Other) const
	{
		return X > Other.X && Y > Other.Y;
	}

	bool operator<=(const FVector2D& Other) const
	{
		return X <= Other.X && Y <= Other.Y;
	}

	bool operator>=(const FVector2D& Other) const
	{
		return X >= Other.X && Y >= Other.Y;
	}

	FVector2D operator-() const
	{
		return FVector2D(-X, -Y);
	}

	FVector2D operator+=(const FVector2D& V)
	{
		X += V.X; Y += V.Y;
		return *this;
	}

	FVector2D operator-=(const FVector2D& V)
	{
		X -= V.X; Y -= V.Y;
		return *this;
	}

	FVector2D operator*=(float Scale)
	{
		X *= Scale; Y *= Scale;
		return *this;
	}

	FVector2D operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV;
		return *this;
	}

	FVector2D operator*=(const FVector2D& V)
	{
		X *= V.X; Y *= V.Y;
		return *this;
	}

	FVector2D operator/=(const FVector2D& V)
	{
		X /= V.X; Y /= V.Y;
		return *this;
	}

	static float DotProduct(const FVector2D& A, const FVector2D& B)
	{
		return A | B;
	}

	static float DistSquared(const FVector2D& V1, const FVector2D& V2)
	{
		return FMath::Square(V2.X - V1.X) + FMath::Square(V2.Y - V1.Y);
	}

	static float Distance(const FVector2D& V1, const FVector2D& V2)
	{
		return FMath::Sqrt(FVector2D::DistSquared(V1, V2));
	}

	static float CrossProduct(const FVector2D& A, const FVector2D& B)
	{
		return A ^ B;
	}

	//bool Equals(const XVector2D& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	float Size() const
	{
		return FMath::Sqrt(X*X + Y * Y);
	}

	float SizeSquared() const
	{
		return X * X + Y * Y;
	}

	void inline Normalize(float Tolerance = SMALL_NUMBER)
	{
		const float SquareSum = X * X + Y * Y;
		if (SquareSum > Tolerance)
		{
			const float Scale = FMath::InvSqrt(SquareSum);
			X *= Scale;
			Y *= Scale;
			return;
		}
		X = 0.0f;
		Y = 0.0f;
	}
};