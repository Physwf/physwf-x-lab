#pragma once

#include "CoreTypes.h"
#include "Vector2D.h"
#include "Vector4.h"
#include "Color.h"

struct XVector
{
	float X, Y, Z;

	XVector() {}

	explicit XVector(float Value) :X(Value), Y(Value), Z(Value) {}

	XVector(float InX, float InY, float InZ) :X(InX), Y(InY), Z(InZ) {}

	explicit XVector(const XVector2D V, float InZ) :X(V.X), Y(V.Y), Z(InZ) {}

	XVector(const XVector4& V) :X(V.X), Y(V.Y), Z(V.Z) {}

	explicit XVector(const XLinearColor& InColor) :X(InColor.R), Y(InColor.G), Z(InColor.B) {}


	FVector operator^(const XVector& V) const
	{
		return FVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
	static XVector CrossProduct(const XVector& A, const XVector& B)
	{
		return A ^ B;
	}

	float operator|(const XVector& V) const
	{
		return X * V.X + Y * V.Y + Z * V.Z;
	}
	static float DotProduct(const XVector& A, const XVector& B)
	{
		return A | B;
	}

	XVector operator+(const XVector& V) const
	{
		return XVector(X + V.X, Y + V.Y, Z + V.Z);
	}
	XVector operator-(const XVector& V) const
	{
		return XVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	XVector operator-(float Bias) const
	{
		return XVector(X - Bias, Y - Bias, Z - Bias);
	}

	XVector operator+(float Bias) const
	{
		return XVector(X + Bias, Y + Bias, Z + Bias);
	}

	XVector operator*(float Scale) const
	{
		return XVector(X * Scale, Y * Scale, Z * Scale);
	}

	XVector operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return XVector(X * RScale, Y * RScale, Z * RScale);
	}

	XVector operator*(const XVector& V) const
	{
		return XVector(X * V.X, Y * V.Y, Z * V.Z);
	}

	XVector operator/(const XVector& V) const
	{
		return XVector(X / V.X, Y / V.Y, Z / V.Z);
	}

	bool operator==(const XVector& V) const
	{
		return X == V.X && Y == V.Y && Z == V.Z;
	}

	bool operator!=(const XVector& V) const
	{
		return X != V.X || Y != V.Y || Z != V.Z;
	}

	//bool Equals(const XVector& V, float Tolerance = KINDA_SMALL_NUMBER) const;


	XVector operator-() const
	{
		return XVector(-X, -Y, -Z);
	}

	XVector operator+=(const XVector& V)
	{
		X += V.X; Y += V.Y; Z += V.Z;
		return *this;
	}
	
	XVector operator-=(const XVector& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z;
		return *this;
	}

	XVector operator*=(float Scale)
	{
		X *= Scale; Y *= Scale; Z *= Scale;
		return *this;
	}

	XVector operator/=(float V)
	{
		const float RV = 1.f / V;
		X *= RV; Y *= RV; Z *= RV;
		return *this;
	}

	XVector operator*=(const XVector& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z;
		return *this;
	}

	XVector operator/=(const XVector& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z;
		return *this;
	}
};