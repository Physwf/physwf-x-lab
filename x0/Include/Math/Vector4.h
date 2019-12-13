#pragma once

struct XVector;
struct XLinearColor;

struct alignas(16) XVector4
{
	float X, Y, Z, W;

	XVector4(const XVector& InVector, float InW = 1.0f);

	XVector4(const XLinearColor& InColor);

	explicit XVector4(float InX = 0.0f, float InY = 0.0f, float InZ = 0.0f, float InW = 1.0f) : X(InX), Y(InY), Z(InZ), W(InW) {}

	explicit XVector4(XVector2D InXY, XVector2D InZW) : X(InXY.X), Y(InXY.Y), Z(InZW.X), W(InZW.Y) {}

	inline XVector4 operator-() const
	{
		return XVector4(-X, -Y, -Z, -W);
	}

	inline XVector4 operator+(const XVector4& V) const
	{
		return XVector4(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}

	inline XVector4 operator+=(const XVector4& V)
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}

	inline XVector4 operator-(const XVector4& V) const
	{
		return XVector4(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}

	inline XVector4 operator-=(const XVector4& V)
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}

	inline XVector4 operator*(float Scale) const
	{
		return XVector4(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}

	XVector4 operator/(float Scale) const
	{
		const float RScale = 1.f / Scale;
		return XVector4(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}

	XVector4 operator/(const XVector4& V) const
	{
		return XVector4(X / V.X, Y / V.Y, Z / V.Z, W / V.W);
	}

	XVector4 operator*(const XVector4& V) const
	{
		return XVector4(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}

	XVector4 operator*=(const XVector4& V)
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}

	XVector4 operator/=(const XVector4& V)
	{
		X /= V.X; Y /= V.Y; Z /= V.Z; W /= V.W;
		return *this;
	}

	XVector4 operator*=(float S)
	{
		X *= S; Y *= S; Z *= S; W *= S;
		return *this;
	}

	friend inline float Dot3(const XVector4& V1, const XVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z;
	}

	friend inline float Dot4(const XVector4& V1, const XVector4& V2)
	{
		return V1.X*V2.X + V1.Y*V2.Y + V1.Z*V2.Z + V1.W*V2.W;
	}

	friend inline XVector4 operator*(float Scale, const XVector4& V)
	{
		return V.operator*(Scale);
	}

	bool operator==(const XVector4& V) const
	{
		return ((X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W));
	}

	bool operator!=(const XVector4& V) const
	{
		return ((X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W));
	}

	XVector4 operator^(const XVector4& V) const
	{
		return XVector4(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X,
			0.0f
		);
	}
};


