#pragma once

#include <initializer_list>
#include <cassert>
#include <cmath>
#include <memory>

typedef	signed char			int8;	
typedef signed short int	int16;	
typedef signed int	 		int32;	
typedef signed long long	int64;		

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

class Math
{
public:
	template<typename T>
	static void Swap(T&A, T& B)
	{
		T Temp = A;
		A = B;
		B = Temp;
	}
};
struct Vector;

struct alignas(16) Vector4
{
	float X, Y, Z, W;

	Vector4(): X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}
	explicit Vector4(float V) : X(V), Y(V), Z(V), W(V) {}
	explicit Vector4(const Vector& V,float InW);
	Vector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
	Vector4(std::initializer_list<float> list);

	Vector4 operator+(const Vector4& rhs) const;
	Vector4 operator-(const Vector4& rhs) const;
	Vector4 operator*(const Vector4& rhs) const;
	Vector4 operator*(float Value) const;
	Vector4 operator/(float Value) const;
	Vector4 operator^(const Vector4& rhs) const;
	Vector4 operator+=(const Vector4& rhs);
	Vector4 operator-=(const Vector4& rhs);
	Vector4 operator*=(const Vector4& rhs);
	Vector4 operator*=(float Value);
	Vector4 operator/=(float Value);
	Vector4 operator-() const;

	bool operator==(const Vector4& rhs) const;
	bool operator!=(const Vector4& rhs) const;

	float& operator[](int32 Index);
	float operator[](int32 Index) const;

	friend inline float Dot3(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
	}
	friend inline float Dot4(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W;
	}
	friend inline Vector4 operator*(float Value, const Vector4& rhs)
	{
		return rhs.operator*(Value);
	}
};

inline Vector4::Vector4(std::initializer_list<float> list)
{
	int32 i = 0;
	for (auto&& v : list)
	{
		(*this)[i] = v;
		if(i++ > 3) break;
	}
}

inline Vector4 Vector4::operator+(const Vector4& rhs) const
{
	return { X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W };
}

inline Vector4 Vector4::operator-(const Vector4& rhs) const
{
	return { X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W };
}

inline Vector4 Vector4::operator-() const
{
	return { -X,-Y,-Z,-W };
}

inline float& Vector4::operator[](int32 Index)
{
	return (&X)[Index];
}

inline float Vector4::operator[](int32 Index) const
{
	return (&X)[Index];
}

inline Vector4 Vector4::operator*(const Vector4& rhs) const
{
	return { X * rhs.X, Y * rhs.Y, Z * rhs.Z, W * rhs.W };
}

inline Vector4 Vector4::operator*(float Value) const
{
	return { X * Value, Y * Value, Z * Value, W * Value };
}

inline Vector4 Vector4::operator/(float Value) const
{
	return { X / Value, Y / Value, Z / Value, W / Value };
}

inline Vector4 Vector4::operator^(const Vector4& rhs) const
{
	return 
	{
		Y * rhs.Z - Z * rhs.Y,
		Z * rhs.X - X * rhs.Z,
		X * rhs.Y - Y * rhs.X,
		0.0f
	};
}

inline Vector4 Vector4::operator+=(const Vector4& rhs)
{
	X += rhs.X; Y += rhs.Y; Z += rhs.Z; W += rhs.W;
	return *this;
}

inline Vector4 Vector4::operator-=(const Vector4& rhs)
{
	X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; W -= rhs.W;
	return *this;
}


inline Vector4 Vector4::operator*=(const Vector4& rhs)
{
	X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; W *= rhs.W;
	return *this;
}

inline Vector4 Vector4::operator*=(float Value)
{
	X *= Value; Y *= Value; Z *= Value; W *= Value;
	return *this;
}

inline Vector4 Vector4::operator/=(float Value)
{
	X /= Value; Y /= Value; Z /= Value; W /= Value;
	return *this;
}

inline bool Vector4::operator==(const Vector4& rhs) const
{
	return (X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W);
}

inline bool Vector4::operator!=(const Vector4& rhs) const
{
	return (X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W);
}


struct Vector
{
	float X, Y, Z;

	Vector() : X(0.0f), Y(0.0f), Z(0.0f) {}
	explicit Vector(float V) : X(V), Y(V), Z(V) {}
	Vector(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}
	Vector(std::initializer_list<float> list);

	Vector operator+(const Vector& rhs) const;
	Vector operator-(const Vector& rhs) const;
	Vector operator*(const Vector& rhs) const;
	Vector operator*(float Value) const;
	Vector operator/(float Value) const;
	Vector operator^(const Vector& rhs) const;
	float operator|(const Vector& V) const;
	Vector operator+=(const Vector& rhs);
	Vector operator-=(const Vector& rhs);
	Vector operator*=(const Vector& rhs);
	Vector operator*=(float Value);
	Vector operator/=(float Value);
	Vector operator-() const;

	bool operator==(const Vector& rhs) const;
	bool operator!=(const Vector& rhs) const;

	float& operator[](int32 Index);
	float operator[](int32 Index) const;

	inline void Normalize();

	friend inline Vector operator*(float Value, const Vector& rhs)
	{
		return rhs.operator*(Value);
	}
};

inline Vector::Vector(std::initializer_list<float> list)
{
	int32 i = 0;
	for (auto&& v : list)
	{
		(*this)[i] = v;
		if (i++ > 2) break;
	}
}

inline Vector Vector::operator+(const Vector& rhs) const
{
	return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
}

inline Vector Vector::operator-(const Vector& rhs) const
{
	return { X - rhs.X, Y - rhs.Y, Z - rhs.Z };
}

inline Vector Vector::operator*(const Vector& rhs) const
{
	return { X * rhs.X, Y * rhs.Y, Z * rhs.Z };
}

inline Vector Vector::operator*(float Value) const
{
	return { X * Value, Y * Value, Z * Value };
}

inline Vector Vector::operator/(float Value) const
{
	return { X / Value, Y / Value, Z / Value };
}

inline Vector Vector::operator+=(const Vector& rhs)
{
	X += rhs.X; Y += rhs.Y; Z += rhs.Z;;
	return *this;
}

inline Vector Vector::operator-=(const Vector& rhs)
{
	X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z;
	return *this;
}

inline Vector Vector::operator*=(const Vector& rhs)
{
	X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z;;
	return *this;
}

inline Vector Vector::operator*=(float Value)
{
	X *= Value; Y *= Value; Z *= Value;
	return *this;
}

inline Vector Vector::operator/=(float Value)
{
	X /= Value; Y /= Value; Z /= Value;
	return *this;
}

inline Vector Vector::operator-() const
{
	return { -X,-Y,-Z };
}

inline bool Vector::operator!=(const Vector& rhs) const
{
	return (X != rhs.X || Y != rhs.Y || Z != rhs.Z);
}

inline bool Vector::operator==(const Vector& rhs) const
{
	return (X == rhs.X && Y == rhs.Y && Z == rhs.Z);
}


inline Vector Vector::operator^(const Vector& rhs) const
{
	return
	{
		Y * rhs.Z - Z * rhs.Y,
		Z * rhs.X - X * rhs.Z,
		X * rhs.Y - Y * rhs.X,
	};
}

inline float Vector::operator|(const Vector& V) const
{
	return X * V.X + Y * V.Y + Z * V.Z;
}

inline float& Vector::operator[](int32 Index)
{
	return (&X)[Index];
}

inline float Vector::operator[](int32 Index) const
{
	return (&X)[Index];
}

inline void Vector::Normalize()
{
	float InvSqrt = 1.0f / sqrtf(X * X + Y * Y + Z * Z);
	X *= InvSqrt;
	Y *= InvSqrt;
	Z *= InvSqrt;
}

inline Vector4::Vector4(const Vector& V, float InW) : X(V.X), Y(V.Y), Z(V.Z), W(InW)
{

}

struct Vector2
{
	float X, Y;

	Vector2() : X(0.0f), Y(0.0f) {}
	explicit Vector2(float V) : X(V), Y(V) {}
	Vector2(float InX, float InY) : X(InX), Y(InY) {}
	Vector2(std::initializer_list<float> list);

	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator*(const Vector2& rhs) const;
	Vector2 operator*(float Value) const;
	Vector2 operator/(float Value) const;
	float operator^(const Vector2& rhs) const;
	float operator|(const Vector2& V) const;
	Vector2 operator+=(const Vector2& rhs);
	Vector2 operator-=(const Vector2& rhs);
	Vector2 operator*=(const Vector2& rhs);
	Vector2 operator*=(float Value);
	Vector2 operator/=(float Value);
	Vector2 operator-() const;

	bool operator==(const Vector2& rhs) const;
	bool operator!=(const Vector2& rhs) const;

	float& operator[](int32 Index);
	float operator[](int32 Index) const;

	void Normalize();
	float Size();
	float SizeSquared();

	friend inline Vector2 operator*(float Value, const Vector2& rhs)
	{
		return rhs.operator*(Value);
	}
};


inline Vector2::Vector2(std::initializer_list<float> list)
{
	int32 i = 0;
	for (auto&& v : list)
	{
		(*this)[i] = v;
		if (i++ > 1) break;
	}
}

inline Vector2 Vector2::operator+(const Vector2& rhs) const
{
	return { X + rhs.X, Y + rhs.Y };
}

inline Vector2 Vector2::operator-(const Vector2& rhs) const
{
	return { X - rhs.X, Y - rhs.Y };
}

inline Vector2 Vector2::operator*(const Vector2& rhs) const
{
	return { X * rhs.X, Y * rhs.Y };
}

inline Vector2 Vector2::operator*(float Value) const
{
	return { X * Value, Y * Value };
}

inline Vector2 Vector2::operator/(float Value) const
{
	return { X / Value, Y / Value};
}

inline Vector2 Vector2::operator+=(const Vector2& rhs)
{
	X += rhs.X; Y += rhs.Y;
	return *this;
}

inline Vector2 Vector2::operator-=(const Vector2& rhs)
{
	X -= rhs.X; Y -= rhs.Y;
	return *this;
}

inline Vector2 Vector2::operator*=(const Vector2& rhs)
{
	X *= rhs.X; Y *= rhs.Y;
	return *this;
}

inline Vector2 Vector2::operator*=(float Value)
{
	X *= Value; Y *= Value;
	return *this;
}

inline Vector2 Vector2::operator/=(float Value)
{
	X /= Value; Y /= Value;
	return *this;
}

inline Vector2 Vector2::operator-() const
{
	return { -X,-Y };
}

inline bool Vector2::operator!=(const Vector2& rhs) const
{
	return (X != rhs.X || Y != rhs.Y);
}

inline bool Vector2::operator==(const Vector2& rhs) const
{
	return (X == rhs.X && Y == rhs.Y);
}


inline float Vector2::operator^(const Vector2& rhs) const
{
	return X * rhs.Y - Y * rhs.X;
}

inline float Vector2::operator|(const Vector2& V) const
{
	return X * V.X + Y * V.Y;
}

inline float& Vector2::operator[](int32 Index)
{
	return (&X)[Index];
}

inline float Vector2::operator[](int32 Index) const
{
	return (&X)[Index];
}

inline void Vector2::Normalize()
{
	float InvSqrt = 1.0f / sqrtf(X * X + Y * Y);
	X *= InvSqrt;
	Y *= InvSqrt;
}

inline float Vector2::Size()
{
	return sqrtf(X * X + Y * Y);
}

inline float Vector2::SizeSquared()
{
	return X * X + Y * Y;
}

struct LinearColor
{
	float R, G, B, A;
};

struct Color
{
	uint8 R, G, B, A;
};

struct Matrix
{
	union
	{
		alignas(16) float M[4][4];
	};

	Matrix() {}

	void SetIndentity();

	void Transpose();

	Matrix operator* (const Matrix& Other) const;

	void operator*=(const Matrix& Other);

	Matrix operator+ (const Matrix& Other) const;

	void operator+=(const Matrix& Other);

	Matrix operator* (float Other) const;

	void operator*=(float Other);

	 bool operator==(const Matrix& Other) const;

	inline bool operator!=(const Matrix& Other) const;

	Vector Transform(Vector InVector);

	static Matrix	FromScale(float Scale);
	static Matrix	DXFromPitch(float fPitch);
	static Matrix	DXFromYaw(float fYaw);
	static Matrix	DXFromRoll(float fRoll);
	static Matrix	DXFormRotation(Vector Rotation);
	static Matrix	DXFromTranslation(Vector Translation);
	static Matrix	DXLooAtLH(const Vector& Eye,const Vector& LookAtPosition, const Vector& Up);
	static Matrix	DXFromPerspectiveFovLH(float fieldOfViewY, float aspectRatio, float znearPlane, float zfarPlane);
	static Matrix	DXReversedZFromPerspectiveFovLH(float fieldOfViewY, float aspectRatio, float znearPlane, float zfarPlane);
	static Matrix	DXFromPerspectiveLH(float w, float h, float zn, float zf);
};

inline void Matrix::SetIndentity()
{
	M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
	M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
	M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
	M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
}


inline void Matrix::Transpose()
{
	Math::Swap(M[0][1], M[1][0]);
	Math::Swap(M[0][2], M[2][0]);
	Math::Swap(M[0][3], M[3][0]);
	Math::Swap(M[2][1], M[1][2]);
	Math::Swap(M[3][1], M[1][3]);
	Math::Swap(M[3][2], M[2][3]);
}

inline Matrix Matrix::operator+(const Matrix& Other) const
{
	Matrix Result;
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			Result.M[X][Y] = M[X][Y] + Other.M[X][Y];
		}
	}
	return Result;
}

inline void Matrix::operator+=(const Matrix& Other)
{
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			M[X][Y] += Other.M[X][Y];
		}
	}
}

inline Matrix Matrix::operator*(float Other) const
{
	Matrix Result;
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			Result.M[X][Y] = M[X][Y] * Other;
		}
	}
	return Result;
}

inline void Matrix::operator*=(float Other)
{
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			M[X][Y] *= Other;
		}
	}
}

inline Matrix Matrix::operator*(const Matrix& Other) const
{
	Matrix Result;
	//coloum major
// 	Result.M[0][0] = M[0][0] * Other.M[0][0] + M[1][0] * Other.M[0][1] + M[2][0] * Other.M[0][2] + M[3][0] * Other.M[0][3];
// 	Result.M[1][0] = M[0][1] * Other.M[0][0] + M[1][1] * Other.M[0][1] + M[2][1] * Other.M[0][2] + M[3][1] * Other.M[0][3];
// 	Result.M[2][0] = M[0][2] * Other.M[0][0] + M[1][2] * Other.M[0][1] + M[2][2] * Other.M[0][2] + M[3][2] * Other.M[0][3];
// 	Result.M[3][0] = M[0][3] * Other.M[0][0] + M[1][3] * Other.M[0][1] + M[2][3] * Other.M[0][2] + M[3][3] * Other.M[0][3];
// 
// 	Result.M[0][1] = M[0][0] * Other.M[1][0] + M[1][0] * Other.M[1][1] + M[2][0] * Other.M[1][2] + M[3][0] * Other.M[1][3];
// 	Result.M[1][1] = M[0][1] * Other.M[1][0] + M[1][1] * Other.M[1][1] + M[2][1] * Other.M[1][2] + M[3][1] * Other.M[1][3];
// 	Result.M[2][1] = M[0][2] * Other.M[1][0] + M[1][2] * Other.M[1][1] + M[2][2] * Other.M[1][2] + M[3][2] * Other.M[1][3];
// 	Result.M[3][1] = M[0][3] * Other.M[1][0] + M[1][3] * Other.M[1][1] + M[2][3] * Other.M[1][2] + M[3][3] * Other.M[1][3];
// 
// 	Result.M[0][2] = M[0][0] * Other.M[2][0] + M[1][0] * Other.M[2][1] + M[2][0] * Other.M[2][2] + M[3][0] * Other.M[2][3];
// 	Result.M[1][3] = M[0][1] * Other.M[2][0] + M[1][1] * Other.M[2][1] + M[2][1] * Other.M[2][2] + M[3][1] * Other.M[2][3];
// 	Result.M[2][2] = M[0][2] * Other.M[2][0] + M[1][2] * Other.M[2][1] + M[2][2] * Other.M[2][2] + M[3][2] * Other.M[2][3];
// 	Result.M[3][2] = M[0][3] * Other.M[2][0] + M[1][3] * Other.M[2][1] + M[2][3] * Other.M[2][2] + M[3][3] * Other.M[2][3];
// 
// 	Result.M[0][3] = M[0][0] * Other.M[3][0] + M[1][0] * Other.M[3][1] + M[2][0] * Other.M[3][2] + M[3][0] * Other.M[3][3];
// 	Result.M[1][3] = M[0][1] * Other.M[3][0] + M[1][1] * Other.M[3][1] + M[2][1] * Other.M[3][2] + M[3][1] * Other.M[3][3];
// 	Result.M[2][3] = M[0][2] * Other.M[3][0] + M[1][2] * Other.M[3][1] + M[2][2] * Other.M[3][2] + M[3][2] * Other.M[3][3];
// 	Result.M[3][3] = M[0][3] * Other.M[3][0] + M[1][3] * Other.M[3][1] + M[2][3] * Other.M[3][2] + M[3][3] * Other.M[3][3];


	//column major
// 	Result.M[0][0] = M[0][0] * Other.M[0][0] + M[1][0] * Other.M[0][1] + M[2][0] * Other.M[0][2] + M[3][0] * Other.M[0][3];
// 	Result.M[1][0] = M[0][0] * Other.M[1][0] + M[1][0] * Other.M[1][1] + M[2][0] * Other.M[1][2] + M[3][0] * Other.M[1][3];
// 	Result.M[2][0] = M[0][0] * Other.M[2][0] + M[1][0] * Other.M[2][1] + M[2][0] * Other.M[1][2] + M[3][0] * Other.M[2][3];
// 	Result.M[3][0] = M[0][0] * Other.M[3][0] + M[1][0] * Other.M[3][1] + M[2][0] * Other.M[1][2] + M[3][0] * Other.M[3][3];
// 
// 	Result.M[0][1] = M[0][1] * Other.M[0][0] + M[1][1] * Other.M[0][1] + M[2][1] * Other.M[0][2] + M[3][1] * Other.M[0][3];
// 	Result.M[1][1] = M[0][1] * Other.M[1][0] + M[1][1] * Other.M[1][1] + M[2][1] * Other.M[1][2] + M[3][1] * Other.M[1][3];
// 	Result.M[2][1] = M[0][1] * Other.M[2][0] + M[1][1] * Other.M[2][1] + M[2][1] * Other.M[1][2] + M[3][1] * Other.M[2][3];
// 	Result.M[3][1] = M[0][1] * Other.M[3][0] + M[1][1] * Other.M[3][1] + M[2][1] * Other.M[1][2] + M[3][1] * Other.M[3][3];
// 
// 	Result.M[0][2] = M[0][2] * Other.M[0][0] + M[1][2] * Other.M[0][1] + M[2][2] * Other.M[0][2] + M[3][2] * Other.M[0][3];
// 	Result.M[1][2] = M[0][2] * Other.M[1][0] + M[1][2] * Other.M[1][1] + M[2][2] * Other.M[1][2] + M[3][2] * Other.M[1][3];
// 	Result.M[2][2] = M[0][2] * Other.M[2][0] + M[1][2] * Other.M[2][1] + M[2][2] * Other.M[1][2] + M[3][2] * Other.M[2][3];
// 	Result.M[3][2] = M[0][2] * Other.M[3][0] + M[1][2] * Other.M[3][1] + M[2][2] * Other.M[1][2] + M[3][2] * Other.M[3][3];
// 
// 	Result.M[0][3] = M[0][3] * Other.M[0][0] + M[1][3] * Other.M[0][1] + M[2][3] * Other.M[0][2] + M[3][3] * Other.M[0][3];
// 	Result.M[1][3] = M[0][3] * Other.M[1][0] + M[1][3] * Other.M[1][1] + M[2][3] * Other.M[1][2] + M[3][3] * Other.M[1][3];
// 	Result.M[2][3] = M[0][3] * Other.M[2][0] + M[1][3] * Other.M[2][1] + M[2][3] * Other.M[1][2] + M[3][3] * Other.M[2][3];
// 	Result.M[3][3] = M[0][3] * Other.M[3][0] + M[1][3] * Other.M[3][1] + M[2][3] * Other.M[1][2] + M[3][3] * Other.M[3][3];

	//row major
// 	Result.M[0][0] = M[0][0] * Other.M[0][0] + M[0][1] * Other.M[1][0] + M[0][2] * Other.M[2][0] + M[0][3] * Other.M[3][0];
// 	Result.M[1][0] = M[1][0] * Other.M[0][0] + M[1][1] * Other.M[1][0] + M[1][2] * Other.M[2][0] + M[1][3] * Other.M[3][0];
// 	Result.M[2][0] = M[2][0] * Other.M[0][0] + M[2][1] * Other.M[1][0] + M[2][2] * Other.M[2][0] + M[2][3] * Other.M[3][0];
// 	Result.M[3][0] = M[3][0] * Other.M[0][0] + M[3][1] * Other.M[1][0] + M[3][2] * Other.M[2][0] + M[3][3] * Other.M[3][0];
// 
// 	Result.M[0][1] = M[0][0] * Other.M[0][1] + M[0][1] * Other.M[1][1] + M[0][2] * Other.M[2][1] + M[0][3] * Other.M[3][1];
// 	Result.M[1][1] = M[1][0] * Other.M[0][1] + M[1][1] * Other.M[1][1] + M[1][2] * Other.M[2][1] + M[1][3] * Other.M[3][1];
// 	Result.M[2][1] = M[2][0] * Other.M[0][1] + M[2][1] * Other.M[1][1] + M[2][2] * Other.M[2][1] + M[2][3] * Other.M[3][1];
// 	Result.M[3][1] = M[3][0] * Other.M[0][1] + M[3][1] * Other.M[1][1] + M[3][2] * Other.M[2][1] + M[3][3] * Other.M[3][1];
// 
// 	Result.M[0][2] = M[0][0] * Other.M[0][2] + M[0][1] * Other.M[1][2] + M[0][2] * Other.M[2][2] + M[0][3] * Other.M[3][2];
// 	Result.M[1][2] = M[1][0] * Other.M[0][2] + M[1][1] * Other.M[1][2] + M[1][2] * Other.M[2][2] + M[1][3] * Other.M[3][2];
// 	Result.M[2][2] = M[2][0] * Other.M[0][2] + M[2][1] * Other.M[1][2] + M[2][2] * Other.M[2][2] + M[2][3] * Other.M[3][2];
// 	Result.M[3][2] = M[3][0] * Other.M[0][2] + M[3][1] * Other.M[1][2] + M[3][2] * Other.M[2][2] + M[3][3] * Other.M[3][2];
// 
// 	Result.M[0][3] = M[0][0] * Other.M[0][3] + M[0][1] * Other.M[1][3] + M[0][2] * Other.M[2][3] + M[0][3] * Other.M[3][3];
// 	Result.M[1][3] = M[1][0] * Other.M[0][3] + M[1][1] * Other.M[1][3] + M[1][2] * Other.M[2][3] + M[1][3] * Other.M[3][3];
// 	Result.M[2][3] = M[2][0] * Other.M[0][3] + M[2][1] * Other.M[1][3] + M[2][2] * Other.M[2][3] + M[2][3] * Other.M[3][3];
// 	Result.M[3][3] = M[3][0] * Other.M[0][3] + M[3][1] * Other.M[1][3] + M[3][2] * Other.M[2][3] + M[3][3] * Other.M[3][3];

	typedef float Float4x4[4][4];
	const Float4x4& A = *((const Float4x4*)this);
	const Float4x4& B = *((const Float4x4*)&Other);
	Float4x4 Temp;
	Temp[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
	Temp[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
	Temp[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
	Temp[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

	Temp[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
	Temp[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
	Temp[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
	Temp[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

	Temp[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
	Temp[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
	Temp[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
	Temp[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

	Temp[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
	Temp[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
	Temp[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
	Temp[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];
	memcpy(&Result, &Temp, 16 * sizeof(float));

	return Result;
}

inline void Matrix::operator*=(const Matrix& Other)
{
	*this = operator*(Other);
}

inline Vector Matrix::Transform(Vector InVector)
{
	return 
	{ 
		InVector.X * M[0][0] + InVector.Y * M[1][0] + InVector.Z * M[2][0],
		InVector.X * M[0][1] + InVector.Y * M[1][1] + InVector.Z * M[2][1],
		InVector.X * M[0][2] + InVector.Y * M[1][2] + InVector.Z * M[2][2],
	};
}

inline Matrix Matrix::FromScale(float Scale)
{
	Matrix Result;
	Result.M[0][0] = Scale;				Result.M[0][1] = 0.0f;			Result.M[0][2] = 0.0f;						Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;				Result.M[1][1] = Scale;			Result.M[1][2] = 0.0f;						Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;				Result.M[2][1] = 0.0f;			Result.M[2][2] = Scale;						Result.M[2][3] = 0.0f;
	Result.M[3][0] = 0.0f;				Result.M[3][1] = 0.0f;			Result.M[3][2] = 0.0f;						Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXFromPitch(float fPitch)
{
	Matrix Result;
	float CosTheta = cosf(fPitch);
	float SinTheta = sinf(fPitch);
	Result.M[0][0] = 1.0f;	Result.M[0][1] = 0.0f;		Result.M[0][2] = 0.0f;		Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;	Result.M[1][1] = CosTheta;	Result.M[1][2] = SinTheta;	Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;	Result.M[2][1] = -SinTheta;	Result.M[2][2] = CosTheta;	Result.M[2][3] = 0.0f;
	Result.M[3][0] = 0.0f;	Result.M[3][1] = 0.0f;		Result.M[3][2] = 0.0f;		Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXFromYaw(float fYaw)
{
	Matrix Result;
	float CosTheta = cosf(fYaw);
	float SinTheta = sinf(fYaw);
	Result.M[0][0] = CosTheta;	Result.M[0][1] = 0.0f;		Result.M[0][2] = -SinTheta;		Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;		Result.M[1][1] = 1.0f;		Result.M[1][2] = 0.0f;			Result.M[1][3] = 0.0f;
	Result.M[2][0] = SinTheta;	Result.M[2][1] = 0.0f;		Result.M[2][2] = CosTheta;		Result.M[2][3] = 0.0f;
	Result.M[3][0] = 0.0f;		Result.M[3][1] = 0.0f;		Result.M[3][2] = 0.0f;			Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXFromRoll(float fRoll)
{
	Matrix Result;
	float CosTheta = cosf(fRoll);
	float SinTheta = sinf(fRoll);
	Result.M[0][0] = CosTheta;	Result.M[0][1] = SinTheta;		Result.M[0][2] = 0.0f;		Result.M[0][3] = 0.0f;
	Result.M[1][0] = -SinTheta;	Result.M[1][1] = CosTheta;		Result.M[1][2] = 0.0f;		Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;		Result.M[2][1] = 0.0f;			Result.M[2][2] = 1.0f;		Result.M[2][3] = 0.0f;
	Result.M[3][0] = 0.0f;		Result.M[3][1] = 0.0f;			Result.M[3][2] = 0.0f;		Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXFormRotation(Vector Rotation)
{
	Matrix Pitch =	DXFromPitch(Rotation.X);
	Matrix Yaw =	DXFromYaw(Rotation.Y);
	Matrix Roll =	DXFromRoll(Rotation.Z);
	return Pitch * Yaw * Roll;
}

inline Matrix Matrix::DXFromTranslation(Vector Translation)
{
	Matrix Result;
	Result.M[0][0] = 1;					Result.M[0][1] = 0.0f;			Result.M[0][2] = 0.0f;						Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;				Result.M[1][1] = 1.0f;			Result.M[1][2] = 0.0f;						Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;				Result.M[2][1] = 0.0f;			Result.M[2][2] = 1.0f;						Result.M[2][3] = 0.0f;
	Result.M[3][0] = Translation.X;		Result.M[3][1] = Translation.Y;	Result.M[3][2] = Translation.Z;				Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXLooAtLH(const Vector& Eye, const Vector& LookAtPosition, const Vector& Up)
{
	Matrix Result;
	Vector Z = LookAtPosition - Eye;
	Z.Normalize();
	Vector X = Up ^ Z;
	X.Normalize();
	Vector Y = Z ^ X;
	Y.Normalize();
	Result.M[0][0] = X.X;			Result.M[0][1] = Y.X;			Result.M[0][2] = Z.X;				Result.M[0][3] = 0.0f;
	Result.M[1][0] = X.Y;			Result.M[1][1] = Y.Y;			Result.M[1][2] = Z.Y;				Result.M[1][3] = 0.0f;
	Result.M[2][0] = X.Z;			Result.M[2][1] = Y.Z;			Result.M[2][2] = Z.Z;				Result.M[2][3] = 0.0f;
	Result.M[3][0] = X | (-Eye);	Result.M[3][1] = Y | (-Eye);	Result.M[3][2] = Z | (-Eye);		Result.M[3][3] = 1.0f;
	return Result;
}

inline Matrix Matrix::DXFromPerspectiveFovLH(float fieldOfViewY, float aspectRatio, float znearPlane, float zfarPlane)
{
	/*
		xScale     0          0               0
		0        yScale       0               0
		0          0       zf/(zf-zn)         1
		0          0       -zn*zf/(zf-zn)     0
		where:
		yScale = cot(fovY/2)

		xScale = yScale / aspect ratio
	*/
	Matrix Result;
	float Cot = 1.0f / tanf(0.5f * fieldOfViewY);
	float InverNF = zfarPlane / (zfarPlane - znearPlane);

	Result.M[0][0] = Cot / aspectRatio;			Result.M[0][1] = 0.0f;		Result.M[0][2] = 0.0f;									Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;						Result.M[1][1] = Cot;		Result.M[1][2] = 0.0f;									Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;						Result.M[2][1] = 0.0f;		Result.M[2][2] =  InverNF;								Result.M[2][3] = 1.0f;
	Result.M[3][0] = 0.0f;						Result.M[3][1] = 0.0f;		Result.M[3][2] = -InverNF * znearPlane;					Result.M[3][3] = 0.0f;
	return Result;
}

inline Matrix Matrix::DXReversedZFromPerspectiveFovLH(float fieldOfViewY, float aspectRatio, float znearPlane, float zfarPlane)
{
	/*
	xScale     0          0               0
	0        yScale       0               0
	0          0       zn/(zn-zf)         1
	0          0       -zn*zf/(zn-zf)     0
	where:
	yScale = cot(fovY/2)

	xScale = yScale / aspect ratio
	*/
	Matrix Result;
	float Cot = 1.0f / tanf(0.5f * fieldOfViewY);
	float InverNF = znearPlane / (znearPlane - zfarPlane);

	Result.M[0][0] = Cot / aspectRatio;			Result.M[0][1] = 0.0f;		Result.M[0][2] = 0.0f;									Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;						Result.M[1][1] = Cot;		Result.M[1][2] = 0.0f;									Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;						Result.M[2][1] = 0.0f;		Result.M[2][2] = InverNF;								Result.M[2][3] = 1.0f;
	Result.M[3][0] = 0.0f;						Result.M[3][1] = 0.0f;		Result.M[3][2] = -InverNF * zfarPlane;					Result.M[3][3] = 0.0f;
	return Result;
}

inline Matrix Matrix::DXFromPerspectiveLH(float w, float h, float zn, float zf)
{
	/*
		2 * zn / w			0				0					0
		0					2 * zn / h		0					0
		0					0				zf / (zf - zn)		1
		0					0				zn*zf / (zn - zf)	0
	*/

	Matrix Result;
	Result.M[0][0] = 2 * zn / w;		Result.M[0][1] = 0.0f;				Result.M[0][2] = 0.0f;									Result.M[0][3] = 0.0f;
	Result.M[1][0] = 0.0f;				Result.M[1][1] = 2.0f * zn / h;		Result.M[1][2] = 0.0f;									Result.M[1][3] = 0.0f;
	Result.M[2][0] = 0.0f;				Result.M[2][1] = 0.0f;				Result.M[2][2] = zf / (zf - zn);						Result.M[2][3] = 1.0f;
	Result.M[3][0] = 0.0f;				Result.M[3][1] = 0.0f;				Result.M[3][2] = zn * zf / (zn - zf);					Result.M[3][3] = 0.0f;
	return Result;
}

inline bool Matrix::operator==(const Matrix& Other) const
{
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			if (M[X][Y] != Other.M[X][Y])
			{
				return false;
			}
		}
	}
	return true;
}

inline bool Matrix::operator!=(const Matrix& Other) const
{
	for (int32 X = 0; X < 4; ++X)
	{
		for (int32 Y = 0; Y < 4; ++Y)
		{
			if (M[X][Y] != Other.M[X][Y])
			{
				return true;
			}
		}
	}
	return false;
}

struct Box
{
	Vector Min;
	Vector Max;
	uint8 IsValid;

	Box() {}

	Box(const Vector& InMin, const Vector& InMax)
		:Min(InMin)
		,Max(InMax)
		,IsValid(1)
	{ }

	inline Box& operator+=(const Vector& Other)
	{
		if (IsValid)
		{
			Min.X = fminf(Min.X, Other.X);
			Min.Y = fminf(Min.Y, Other.Y);
			Min.Z = fminf(Min.Z, Other.Z);

			Max.X = fmaxf(Min.X, Other.X);
			Max.Y = fmaxf(Min.Y, Other.Y);
			Max.Z = fmaxf(Min.Z, Other.Z);
		}
		else
		{
			Min = Max = Other;
			IsValid = 1;
		}
		return *this;
	}

	inline Box& operator+=(const Box& Other)
	{
		if (IsValid && Other.IsValid)
		{
			Min.X = fminf(Min.X, Other.Min.X);
			Min.Y = fminf(Min.Y, Other.Min.Y);
			Min.Z = fminf(Min.Z, Other.Min.Z);

			Max.X = fmaxf(Min.X, Other.Max.X);
			Max.Y = fmaxf(Min.Y, Other.Max.Y);
			Max.Z = fmaxf(Min.Z, Other.Max.Z);
		}
		else if (Other.IsValid)
		{
			*this = Other;
		}
		return *this;
	}

	inline Box operator+(const Vector& Other)
	{
		return Box(*this) += Other;
	}

	inline Box operator+(const Box& Other)
	{
		return Box(*this) += Other;
	}
};