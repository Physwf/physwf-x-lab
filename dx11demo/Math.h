#pragma once

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

struct Vector4
{
	float X, Y, Z, W;

	Vector4 operator+(const Vector4& rhs);
	Vector4 operator-(const Vector4& rhs);
	Vector4 operator*(const Vector4& rhs);
	Vector4 operator*(float Value);
	Vector4 operator/(float Value);
	Vector4 operator^(const Vector4& rhs);
	Vector4 operator+=(const Vector4& rhs);
	Vector4 operator-=(const Vector4& rhs);
	Vector4 operator*=(const Vector4& rhs);
	Vector4 operator*=(float Value);
	Vector4 operator/=(float Value);

	bool operator==(const Vector4& rhs);
	bool operator!=(const Vector4& rhs);
};

inline Vector4 Vector4::operator+(const Vector4& rhs)
{
	return { X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W };
}

inline Vector4 Vector4::operator-(const Vector4& rhs)
{
	return { X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W };
}

inline Vector4 Vector4::operator*(const Vector4& rhs)
{
	return { X * rhs.X, Y * rhs.Y, Z * rhs.Z, W * rhs.W };
}

inline Vector4 Vector4::operator*(float Value)
{
	return { X * Value, Y * Value, Z * Value, W * Value };
}

inline Vector4 Vector4::operator/(float Value)
{
	return { X / Value, Y / Value, Z / Value, W / Value };
}

inline Vector4 Vector4::operator^(const Vector4& rhs)
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

inline bool Vector4::operator==(const Vector4& rhs)
{
	return (X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W);
}

inline bool Vector4::operator!=(const Vector4& rhs)
{
	return (X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W);
}

struct Vector
{
	float X, Y, Z;
};

struct Vector2
{
	float X, Y;
};

struct LinearColor
{
	float R, G, B, A;
};

struct Color
{
	uint8 R, G, B, A;
};

