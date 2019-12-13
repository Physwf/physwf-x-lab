#pragma once

#include "CoreTypes.h"
#include "Math/XMath.h"

struct XVector;

struct XVector2D
{
	float X, Y;

	XVector2D() { }

	XVector2D(float InX, float InY) :X(InX), Y(InY) { }

	explicit inline XVector2D(const XVector& V);

	XVector2D operator+(const XVector2D& V) const
	{
		return XVector2D(X + V.X, Y + V.Y);
	}

	XVector2D operator-(const XVector2D& V) const
	{
		return XVector2D(X - V.X, Y - V.Y);
	}

	XVector2D operator*(float Scale) const
	{
		return XVector2D(X * Scale, Y * Scale);
	}

	XVector2D operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return XVector2D(X * RScale, Y * RScale);
	}

	XVector2D operator+(float A) const
	{
		return XVector2D(X + A, Y + A);
	}

	XVector2D operator-(float A) const
	{
		return XVector2D(X - A, Y - A);
	}

	XVector2D operator*(const XVector2D& V) const
	{
		return XVector2D(X * V.X, Y * V.Y);
	}

	XVector2D operator/(const XVector2D& V) const
	{
		return XVector2D(X / V.X, Y / V.Y);
	}

	float operator|(const XVector2D& V) const
	{
		return X * V.X + Y * V.Y;
	}

	float operator^(const XVector2D& V) const
	{
		return X * V.Y - Y * V.X;
	}

	bool operator==(const XVector2D& V) const
	{
		return X == V.X && Y == V.Y;
	}

	bool operator!=(const XVector2D& V) const
	{
		return X != V.X || Y != V.Y;
	}

	bool operator<(const XVector2D& Other) const
	{
		return X < Other.X && Y < Other.Y;
	}

	bool operator>(const XVector2D& Other) const
	{
		return X > Other.X && Y > Other.Y;
	}

	bool operator<=(const XVector2D& Other) const
	{
		return X <= Other.X && Y <= Other.Y;
	}

	bool operator>=(const XVector2D& Other) const
	{
		return X >= Other.X && Y >= Other.Y;
	}

	XVector2D operator-() const
	{
		return XVector2D(-X, -Y);
	}

	XVector2D operator+=(const XVector2D& V)
	{
		X += V.X; Y += V.Y;
		return *this;
	}

	XVector2D operator-=(const XVector2D& V)
	{
		X -= V.X; Y -= V.Y;
		return *this;
	}

	XVector2D operator*=(float Scale)
	{
		X *= Scale; Y *= Scale;
		return *this;
	}

	XVector2D operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV;
		return *this;
	}

	XVector2D operator*=(const XVector2D& V)
	{
		X *= V.X; Y *= V.Y;
		return *this;
	}

	XVector2D operator/=(const XVector2D& V)
	{
		X /= V.X; Y /= V.Y;
		return *this;
	}

	static float DotProduct(const XVector2D& A, const XVector2D& B)
	{
		return A | B;
	}

	static float DistSquared(const XVector2D& V1, const XVector2D& V2)
	{
		return XMath::Square(V2.X - V1.X) + XMath::Square(V2.Y - V1.Y);
	}

	static float Distance(const XVector2D& V1, const XVector2D& V2)
	{
		return XMath::Sqrt(XVector2D::DistSquared(V1, V2));
	}

	static float CrossProduct(const XVector2D& A, const XVector2D& B)
	{
		return A ^ B;
	}

	//bool Equals(const XVector2D& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	float Size() const
	{
		return XMath::Sqrt(X*X + Y * Y);
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
			const float Scale = XMath::InvSqrt(SquareSum);
			X *= Scale;
			Y *= Scale;
			return;
		}
		X = 0.0f;
		Y = 0.0f;
	}
};