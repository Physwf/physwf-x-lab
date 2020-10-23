#pragma once

struct Vector2D
{
	float X, Y;

	Vector2D() : X(0.0f), Y(0.0f) {}
	explicit Vector2D(float V) : X(V), Y(V) {}
	Vector2D(float InX, float InY) : X(InX), Y(InY) {}
	Vector2D(std::initializer_list<float> list)
	{
		int i = 0;
		for (auto&& v : list)
		{
			(*this)[i] = v;
			if (i++ > 1) break;
		}
	}
	Vector2D operator+(const Vector2D& rhs) const { return { X + rhs.X, Y + rhs.Y }; }
	Vector2D operator-(const Vector2D& rhs) const { return { X - rhs.X, Y - rhs.Y }; }
	Vector2D operator*(const Vector2D& rhs) const { return { X * rhs.X, Y * rhs.Y }; }
	Vector2D operator*(float Value) const { return { X * Value, Y * Value }; }
	Vector2D operator/(float Value) const { return { X / Value, Y / Value }; }
	float operator^(const Vector2D& rhs) const { return X * rhs.Y - Y * rhs.X; }
	float operator|(const Vector2D& V) const { return X * V.X + Y * V.Y; }
	Vector2D operator+=(const Vector2D& rhs) { X += rhs.X; Y += rhs.Y; return *this; }
	Vector2D operator-=(const Vector2D& rhs) { X -= rhs.X; Y -= rhs.Y; return *this; }
	Vector2D operator*=(const Vector2D& rhs) { X *= rhs.X; Y *= rhs.Y; return *this; }
	Vector2D operator*=(float Value) { X *= Value; Y *= Value; return *this; }
	Vector2D operator/=(float Value) { X /= Value; Y /= Value; return *this; }
	Vector2D operator-() const { return { -X,-Y }; }
	bool operator==(const Vector2D& rhs) const { return (X != rhs.X || Y != rhs.Y); }
	bool operator!=(const Vector2D& rhs) const { return (X != rhs.X || Y != rhs.Y); }
	float& operator[](int Index) { return (&X)[Index]; }
	float operator[](int Index) const { return (&X)[Index]; }
	void Normalize()
	{
		float InvSqrt = 1.0f / sqrtf(X * X + Y * Y);
		X *= InvSqrt;
		Y *= InvSqrt;
	}
	float Size() { return sqrtf(X * X + Y * Y); }
	float SizeSquared() { return X * X + Y * Y; }
	friend inline Vector2D operator*(float Value, const Vector2D& rhs) { return rhs.operator*(Value); }
};


struct Matrix2D
{
	float M[2][2];
};