#include "Mathematics.h"
#include "Vector.h"

bool Math::Plane(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float* a, float* b, float* c, float* d)
{
	if (p1.IsNearlyEqual(p2) || p1.IsNearlyEqual(p3) || p2.IsNearlyEqual(p3)) return false;

	float A = (p2.Y - p1.Y) * (p3.Z - p1.Z) - (p3.Y - p1.Y) * (p2.Z - p1.Z);
	float B = (p2.Z - p1.Z) * (p3.X - p1.X) - (p3.Z - p1.Z) * (p2.X - p1.X);
	float C = (p2.X - p1.X) * (p3.Y - p1.Y) - (p3.X - p1.X) * (p2.Y - p1.Y);
	float D = -A * (p1.X) - B * p1.Y - C * p1.Z;
	*a = A; *b = B; *c = C; *d = D;
	return true;
}

bool Math::IsInsideTriangle(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector3f& p)
{
	if (Dot(Cross((p2 - p1), (p - p1)), Cross((p2 - p1), (p3 - p1))) < 0) return false;
	if (Dot(Cross((p3 - p2), (p - p2)), Cross((p3 - p2), (p1 - p2))) < 0) return false;
	if (Dot(Cross((p1 - p3), (p - p3)), Cross((p1 - p3), (p2 - p3))) < 0) return false;
	return true;
}

