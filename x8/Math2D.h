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
	bool operator==(const Vector2D& rhs) const { return (X == rhs.X && Y == rhs.Y); }
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


struct Matrix3x3
{
	float M[3][3];

	void Translate(float X, float Y) 
	{
		M[0][0] = 1;					M[0][1] = 0;					M[0][2] = 0;
		M[1][0] = 0;;					M[1][1] = 1;					M[1][2] = 0;
		M[2][0] = X;					M[2][1] = Y;					M[2][2] = 1.f;
	}

	void Rotate(float fRadian)
	{
		M[0][0] = std::cos(fRadian);	M[0][1] = std::sin(fRadian);	M[0][2] = 0;
		M[1][0] = -std::sin(fRadian);	M[1][1] = std::cos(fRadian);	M[1][2] = 0;
		M[2][0] = 0;					M[2][1] = 0;					M[2][2] = 1.f;
	}

	Vector2D Transform(const Vector2D& V)
	{
		Vector2D Result;
		Result.X = M[0][0] * V.X + M[1][0] * V.Y + M[2][0];
		Result.Y = M[0][1] * V.X + M[1][1] * V.Y + M[2][1];
		return Result;
	}

	Matrix3x3 operator*(const Matrix3x3& Other) const
	{
		Matrix3x3 Result;

		typedef float Float3x3[3][3];
		const Float3x3& A = *((const Float3x3*)this);
		const Float3x3& B = *((const Float3x3*)&Other);
		Float3x3 Temp;
		Temp[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0];
		Temp[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1];
		Temp[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2];

		Temp[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0];
		Temp[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1];
		Temp[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2];

		Temp[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0];
		Temp[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1];
		Temp[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2];

		memcpy(&Result, &Temp, 9 * sizeof(float));

		return Result;
	}
};

struct Matrix4x4
{
	union
	{
		alignas(16) float M[4][4];
	};

	void SetIndentity()
	{
		M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
		M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
		M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
		M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
	}

	Matrix4x4 operator*(float Other) const
	{
		Matrix4x4 Result;
		for (int X = 0; X < 4; ++X)
		{
			for (int Y = 0; Y < 4; ++Y)
			{
				Result.M[X][Y] = M[X][Y] * Other;
			}
		}
		return Result;
	}

	void operator*=(float Other)
	{
		for (int X = 0; X < 4; ++X)
		{
			for (int Y = 0; Y < 4; ++Y)
			{
				M[X][Y] *= Other;
			}
		}
	}
	inline void Transpose()
	{
		std::swap(M[0][1], M[1][0]);
		std::swap(M[0][2], M[2][0]);
		std::swap(M[0][3], M[3][0]);
		std::swap(M[2][1], M[1][2]);
		std::swap(M[3][1], M[1][3]);
		std::swap(M[3][2], M[2][3]);
	}

	static Matrix4x4 DXFromOrthognalLH(float l, float r, float b, float t, float zn, float zf)
	{
		Matrix4x4 Result;
		Result.M[0][0] = 2.0f / (r - l);		Result.M[0][1] = 0.0f;					Result.M[0][2] = 0.0f;							Result.M[0][3] = 0.0f;
		Result.M[1][0] = 0.0f;					Result.M[1][1] = 2.0f / (t - b);		Result.M[1][2] = 0.0f;							Result.M[1][3] = 0.0f;
		Result.M[2][0] = 0.0f;					Result.M[2][1] = 0.0f;					Result.M[2][2] = 1.0f / (zf - zn);				Result.M[2][3] = 0.0f;
		Result.M[3][0] = -(r + l) / (r - l);	Result.M[3][1] = -(t + b) / (t - b);		Result.M[3][2] = -zn / (zf - zn);				Result.M[3][3] = 1.0f;
		return Result;
	}
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

			Max.X = fmaxf(Max.X, Other.X);
			Max.Y = fmaxf(Max.Y, Other.Y);
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

			Max.X = fmaxf(Max.X, Other.Max.X);
			Max.Y = fmaxf(Max.Y, Other.Max.Y);
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