#pragma once

#include "Math/Matrix.h"
/**
* Constructs a basis matrix for the axis vectors and returns the sign of the determinant
*
* @param XAxis - x axis (tangent)
* @param YAxis - y axis (binormal)
* @param ZAxis - z axis (normal)
* @return sign of determinant either -1 or +1
*/
inline float GetBasisDeterminantSign(const FVector& XAxis, const FVector& YAxis, const FVector& ZAxis)
{
	FMatrix Basis(
		FPlane(XAxis, 0),
		FPlane(YAxis, 0),
		FPlane(ZAxis, 0),
		FPlane(0, 0, 0, 1)
	);
	return (Basis.Determinant() < 0) ? -1.0f : +1.0f;
}
