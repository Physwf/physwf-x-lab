#pragma once
#include <math.h>
#include <cstdlib>

typedef double Float;

struct Vec 
{
	union
	{
		struct { Float X, Y, Z; };
		struct { Float R, G, B; };
	};

	Float& operator[](const unsigned int index)
	{
		if (index == 0) return X;
		else if (index == 1) return Y;
		else return Z;
	}
	Float operator[](const unsigned int index) const
	{
		if (index == 0) return X;
		else if (index == 1) return Y;
		else return Z;
	}

	Vec operator-()
	{
		return Vec(-X,-Y,-Z);
	}

	Vec() :X(.0f), Y(.0f), Z(.0f) {}
	Vec(Float x, Float y, Float z) :X(x), Y(y), Z(z) {}
};

typedef Vec	Point;
typedef Vec Color;

inline void Add(const Vec& lfs, const Vec& rhs, Vec& Result)
{
	Result[0] = lfs[0] + rhs[0];
	Result[1] = lfs[1] + rhs[1];
	Result[2] = lfs[2] + rhs[2];
}
inline Vec operator+(const Vec& lfs, const Vec& rhs)
{
	return Vec(lfs.X + rhs.X, lfs.Y + rhs.Y, lfs.Z + rhs.Z);
}

inline void Subtract(const Vec& lfs, const Vec& rhs, Vec& Result)
{
	Result[0] = lfs[0] - rhs[0];
	Result[1] = lfs[1] - rhs[1];
	Result[2] = lfs[2] - rhs[2];
}
inline Vec operator-(const Vec& lfs, const Vec& rhs)
{
	return Vec(lfs.X - rhs.X, lfs.Y - rhs.Y, lfs.Z - rhs.Z);
}

inline Float Dot(const Vec& lfs, const Vec& rhs)
{
	return lfs[0] * rhs[0] + lfs[1] * rhs[1] + lfs[2] * rhs[2];
}


inline void Multiply(const Vec& A, Float v, Vec& Result)
{
	Result[0] = A[0] * v;
	Result[1] = A[1] * v;
	Result[2] = A[2] * v;
}
inline Vec operator*(const Vec& lfs, Float v)
{
	return Vec(lfs.X * v, lfs.Y * v, lfs.Z * v);
}

inline Float Lenght(const Vec& v)
{
	return sqrtl(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

inline void Copy(const Vec& From, Vec& To)
{
	To[0] = From[0];
	To[1] = From[1];
	To[2] = From[2];
}

inline void MultiplyAdd(Float a, const Vec& A, Float b, const Vec& B, Vec& Result)
{
	Result[0] = a * A[0] + b * B[0];
	Result[1] = a * A[1] + b * B[1];
	Result[2] = a * A[2] + b * B[2];
}

inline void MultiplyAdd(Float a, const Vec& A, const Vec& B, Vec& Result)
{
	MultiplyAdd(a, A, 1.0, B, Result);
}

inline void Normalize(const Vec& A, Vec& Result)
{
	Float L = Lenght(A);
	Result[0] = A[0] / L;
	Result[1] = A[1] / L;
	Result[2] = A[2] / L;
}
inline void Normalize(Vec& V)
{
	Float L = Lenght(V);
	V[0] = V[0] / L;
	V[1] = V[1] / L;
	V[2] = V[2] / L;
}

inline void Negate(const Vec& A, Vec& Result)
{
	Multiply(A, -1.0, Result);
}

int VecRead(const char* desc, std::size_t size, Vec& Result);

inline unsigned int ToRGBA(const Color& color)
{
	Float r = color.R > 1.0 ? 1.0 : color.R < 0.0 ? 0.0 : color.R;
	Float g = color.G > 1.0 ? 1.0 : color.G < 0.0 ? 0.0 : color.G;
	Float b = color.B > 1.0 ? 1.0 : color.B < 0.0 ? 0.0 : color.B;
	unsigned int R = unsigned int(r * 255.0);
	unsigned int G = unsigned int(g * 255.0);
	unsigned int B = unsigned int(b * 255.0);
	return 0xff << 24 | R << 16 | G << 8 | B;
}

inline void SpecularDirection(const Point& I, const Point& N, Point& R)
{
	MultiplyAdd(-2.0 * Dot(I, N), N, I, R);
}

int TransmissionDirection(const struct Surf* m1, const struct Surf* m2, const Point& I, const Point& N, Point& T);