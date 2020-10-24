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

struct Bounds2D
{
	Vector2D Min;
	Vector2D Max;
	char IsValid;

	Bounds2D() :IsValid(false) {}

	Bounds2D(const Vector2D& InMin, const Vector2D& InMax)
		:Min(InMin)
		, Max(InMax)
		, IsValid(1)
	{ }

	inline Bounds2D& operator+=(const Vector2D& Other)
	{
		if (IsValid)
		{
			Min.X = fminf(Min.X, Other.X);
			Min.Y = fminf(Min.Y, Other.Y);

			Max.X = fmaxf(Min.X, Other.X);
			Max.Y = fmaxf(Min.Y, Other.Y);
		}
		else
		{
			Min = Max = Other;
			IsValid = 1;
		}
		return *this;
	}

	inline Bounds2D& operator+=(const Bounds2D& Other)
	{
		if (IsValid && Other.IsValid)
		{
			Min.X = fminf(Min.X, Other.Min.X);
			Min.Y = fminf(Min.Y, Other.Min.Y);

			Max.X = fmaxf(Min.X, Other.Max.X);
			Max.Y = fmaxf(Min.Y, Other.Max.Y);
		}
		else if (Other.IsValid)
		{
			*this = Other;
		}
		return *this;
	}

	inline Bounds2D operator+(const Vector2D& Other)
	{
		return Bounds2D(*this) += Other;
	}

	inline Bounds2D operator+(const Bounds2D& Other)
	{
		return Bounds2D(*this) += Other;
	}

	Bounds2D Offset(const Bounds2D& Other) const
	{
		Bounds2D Result;
		Result.Min.X = Other.Min.X / (Max.X - Min.X);
		Result.Min.Y = Other.Min.Y / (Max.X - Min.X);
		Result.Max.X = Other.Max.X / (Max.X - Min.X);
		Result.Max.Y = Other.Max.Y / (Max.X - Min.X);
		return Result;
	}
	void Reset()
	{
		IsValid = 0;
	}
};

struct Circle
{
	Vector2D Center;
	float Radius;
	char IsValid;

	Circle() :IsValid(false) {}

	Circle(const Vector2D& InCenter, float InRadius)
		:Center(InCenter)
		, Radius(InRadius)
		, IsValid(1)
	{ }

	bool IsInside(const Circle& Other) const
	{
		return false;
	}

	inline Circle& operator+=(const Circle& Other)
	{

	}

	inline Circle operator+(const Circle& Other)
	{
		return Circle(*this) += Other;
	}

	Bounds2D ToBounds2D() const
	{
		return Bounds2D(Center - Vector2D(Radius, Radius), Center + Vector2D(Radius, Radius));
	}
};

template<typename T>
T Clamp(T Value, T Min, T Max)
{
	if (Value < Min) return Min;
	if (Value > Max) return Max;
	return Value;
}