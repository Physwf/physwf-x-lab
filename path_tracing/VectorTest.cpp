#include "Vector.h"

void TestMain()
{
	Vector3i v3;
	v3.X = 1;
	v3.Y = 2;
	v3.Z = 3;
	Vector3i v1 = v3 * v3;
	Vector3i v2 = v3 / v3;

	Vector3i CP = v3.Cross(v1);
}