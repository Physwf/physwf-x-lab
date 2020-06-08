
#include <math.h>

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

inline Float Dot(const Vec lfs, const Vec rhs, Vec Result)
{
	return lfs[0] * rhs[0] + lfs[1] * rhs[1] + lfs[2] * rhs[2];
}

inline Float Lenght(Vec v)
{
	return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

inline void Copy(const Vec From, const Vec To)
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
	MultiplyAdd(a,A,1.0,B,Result);
}

inline Normalize(const Vec A, const Vec Result)
{
	Float L = Lenght(A);
	Result[0] = A[0] / L;
	Result[1] = A[1] / L;
	Result[2] = A[2] / L;
}


