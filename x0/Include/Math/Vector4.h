#pragma once

struct FVector;
struct FLinearColor;

struct alignas(16) FVector4
{
	float X, Y, Z, W;

	inline FVector4(const FVector& InVector, float InW = 1.0f);

	inline FVector4(const FLinearColor& InColor);

	explicit FVector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f) : X(InX), Y(InY), Z(InZ), W(InW) {}

	explicit FVector4(FVector2D InXY, FVector2D InZW) : X(InXY.X), Y(InXY.Y), Z(InZW.X), W(InZW.Y) {}

	inline FVector4 operator-() const
	{
		return FVector4(-X, -Y, -Z, -W);
	}

	inline FVector4 operator+(const FVector4& V) const
	{
		return FVector4(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}

	inline FVector4 operator+=(const FVector4& V)
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}

	inline FVector4 operator-(const FVector4& V) const
	{
		return FVector4(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}

	inline FVector4 operator-=(const FVector4& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}

	inline FVector4 operator*(float Scale) const
	{
		return FVector4(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}

	FVector4 operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return FVector4(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}

	FVector4 operator/(const FVector4& V) const
	{
		return FVector4(X / V.X, Y / V.Y, Z / V.Z, W / V.W);
	}

	FVector4 operator*(const FVector4& V) const
	{
		return FVector4(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}

	FVector4 operator*=(const FVector4& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}

	FVector4 operator/=(const FVector4& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z; W /= V.W;
		return *this;
	}

	FVector4 operator*=(float S)
	{
		X *= S; Y *= S; Z *= S; W *= S;
		return *this;
	}

	friend inline float Dot3(const FVector4& V1, const FVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z;
	}

	friend inline float Dot4(const FVector4& V1, const FVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z + V1.W*V2.W;
	}

	friend inline FVector4 operator*(float Scale, const FVector4& V)
	{
		return V.operator*(Scale);
	}

	bool operator==(const FVector4& V) const
	{
		return ((X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W));
	}

	bool operator!=(const FVector4& V) const
	{
		return ((X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W));
	}

	FVector4 operator^(const FVector4& V) const
	{
		return FVector4(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X,
			0.0f
		);
	}

	inline FVector4 FVector4::GetSafeNormal(float Tolerance = SMALL_NUMBER) const
	{
		const float SquareSum = X * X + Y * Y + Z * Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = FMath::InvSqrt(SquareSum);
			return FVector4(X*Scale, Y*Scale, Z*Scale, 0.0f);
		}
		return FVector4(0.f);
	}


	inline FVector4 FVector4::GetUnsafeNormal3() const
	{
		const float Scale = FMath::InvSqrt(X*X + Y * Y + Z * Z);
		return FVector4(X*Scale, Y*Scale, Z*Scale, 0.0f);
	}
};


