#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"

class FTranslationMatrix : public FMatrix
{
public:

	FTranslationMatrix(const FVector& Delta);

	static FMatrix Make(FVector const& Delta)
	{
		return FTranslationMatrix(Delta);
	}
};

inline FTranslationMatrix::FTranslationMatrix(const FVector& Delta)
	: FMatrix(
		FPlane(1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, 1.0f, 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, 1.0f, 0.0f),
		FPlane(Delta.X, Delta.Y, Delta.Z, 1.0f)
	)
{
}

