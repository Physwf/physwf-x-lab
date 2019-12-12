#pragma once

#include "CoreTypes.h"

struct XVector2D
{
	float X, Y;

	FVector2D() { }

	XVector2D(float InX, float InY) :X(InX), Y(InY) { }

	explicit XVector2D(const XVector& V) :X(V.X), Y(V.Y) { }

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

	}

	static float Distance(const XVector2D& V1, const XVector2D& V2)
	{

	}

	static float CrossProduct(const XVector2D& A, const XVector2D& B)
	{
		return A ^ B;
	}

	//bool Equals(const XVector2D& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	float Size() const
	{

	}

	float SizeSquared() const
	{

	}
};