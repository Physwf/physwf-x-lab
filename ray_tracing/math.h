#pragma once
#include <math.h>
#include <cstdlib>

typedef double Float;
typedef Float Vec[3];
typedef Vec	Point;
typedef Vec Color;

inline void Add(const Vec lfs, const Vec rhs, Vec Result)
{
	Result[0] = lfs[0] + rhs[0];
	Result[1] = lfs[1] + rhs[1];
	Result[2] = lfs[2] + rhs[2];
}

inline void Subtract(const Vec lfs, const Vec rhs, Vec Result)
{
	Result[0] = lfs[0] - rhs[0];
	Result[1] = lfs[1] - rhs[1];
	Result[2] = lfs[2] - rhs[2];
}

inline Float Dot(const Vec lfs, const Vec rhs)
{
	return lfs[0] * rhs[0] + lfs[1] * rhs[1] + lfs[2] * rhs[2];
}

inline void Multiply(const Vec A, Float v, Vec Result)
{
	Result[0] = A[0] * v;
	Result[1] = A[1] * v;
	Result[2] = A[2] * v;
}

inline Float Lenght(const Vec v)
{
	return sqrtl(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

inline void Copy(const Vec From, Vec To)
{
	To[0] = From[0];
	To[1] = From[1];
	To[2] = From[2];
}

inline void MultiplyAdd(Float a, const Vec A, Float b, const Vec B, Vec Result)
{
	Result[0] = a * A[0] + b * B[0];
	Result[1] = a * A[1] + b * B[1];
	Result[2] = a * A[2] + b * B[2];
}

inline void MultiplyAdd(Float a, const Vec A, const Vec B, Vec Result)
{
	MultiplyAdd(a, A, 1.0, B, Result);
}

inline void Normalize(const Vec A, Vec Result)
{
	Float L = Lenght(A);
	Result[0] = A[0] / L;
	Result[1] = A[1] / L;
	Result[2] = A[2] / L;
}

inline void Negate(const Vec A, Vec Result)
{
	Multiply(A, -1.0, Result);
}

int VecRead(const char* desc, std::size_t size, Vec Result);

inline void SpecularDirection(Point I, Point N, Point R)
{
	MultiplyAdd(-2.0 * Dot(I, N), N, I, R);
}

struct Surf;
int TransmissionDirection(const Surf* m1, const Surf* m2, Point I, Point N, Point T);