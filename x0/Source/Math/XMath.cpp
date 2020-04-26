#include "Math/XMath.h"
#include "Math/RandomStream.h"


X0_API FVector FRotator::Euler() const
{
	return FVector(Roll, Pitch, Yaw);
}

X0_API FVector FRotator::Vector() const
{
	float CP, SP, CY, SY;
	FMath::SinCos(&SP, &CP, FMath::DegreesToRadians(Pitch));
	FMath::SinCos(&SY, &CY, FMath::DegreesToRadians(Yaw));
	FVector V = FVector(CP*CY, CP*SY, SP);

	return V;
}

X0_API FRotator::FRotator(const FQuat& Quat)
{
	*this = Quat.Rotator();
}

X0_API FRotator FRotator::GetInverse() const
{
	return Quaternion().Inverse().Rotator();
}

X0_API FQuat FRotator::Quaternion() const
{
	return FQuat();
}

X0_API FVector FRotator::RotateVector(const FVector& V) const
{
	return FVector();
}

X0_API FVector FRotator::UnrotateVector(const FVector& V) const
{
	return FVector();
}


X0_API FRotator FQuat::Rotator() const
{
	return FRotator();
}

FRotator FVector::Rotation() const
{
	return ToOrientationRotator();
}

FRotator FVector::ToOrientationRotator() const
{
	FRotator R;

	// Find yaw.
	R.Yaw = FMath::Atan2(Y, X) * (180.f / PI);

	// Find pitch.
	R.Pitch = FMath::Atan2(Z, FMath::Sqrt(X*X + Y * Y)) * (180.f / PI);

	// Find roll.
	R.Roll = 0;

#if ENABLE_NAN_DIAGNOSTIC
	if (R.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("FVector::Rotation(): Rotator result %s contains NaN! Input FVector = %s"), *R.ToString(), *this->ToString());
		R = FRotator::ZeroRotator;
	}
#endif

	return R;
}

FMatrix FRotationMatrix::Make(FQuat const& Rot)
{
	return FQuatRotationTranslationMatrix(Rot, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromX(FVector const& XAxis)
{
	FVector const NewX = XAxis.GetSafeNormal();

	// try to use up if possible
	FVector const UpVector = (FMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

	const FVector NewY = (UpVector ^ NewX).GetSafeNormal();
	const FVector NewZ = NewX ^ NewY;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromY(FVector const& YAxis)
{
	FVector const NewY = YAxis.GetSafeNormal();

	// try to use up if possible
	FVector const UpVector = (FMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

	const FVector NewZ = (UpVector ^ NewY).GetSafeNormal();
	const FVector NewX = NewY ^ NewZ;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromZ(FVector const& ZAxis)
{
	FVector const NewZ = ZAxis.GetSafeNormal();

	// try to use up if possible
	FVector const UpVector = (FMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

	const FVector NewX = (UpVector ^ NewZ).GetSafeNormal();
	const FVector NewY = NewZ ^ NewX;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromXY(FVector const& XAxis, FVector const& YAxis)
{
	FVector NewX = XAxis.GetSafeNormal();
	FVector Norm = YAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewX | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewZ = (NewX ^ Norm).GetSafeNormal();
	const FVector NewY = NewZ ^ NewX;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromXZ(FVector const& XAxis, FVector const& ZAxis)
{
	FVector const NewX = XAxis.GetSafeNormal();
	FVector Norm = ZAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewX | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewY = (Norm ^ NewX).GetSafeNormal();
	const FVector NewZ = NewX ^ NewY;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromYX(FVector const& YAxis, FVector const& XAxis)
{
	FVector const NewY = YAxis.GetSafeNormal();
	FVector Norm = XAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewY | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewZ = (Norm ^ NewY).GetSafeNormal();
	const FVector NewX = NewY ^ NewZ;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromYZ(FVector const& YAxis, FVector const& ZAxis)
{
	FVector const NewY = YAxis.GetSafeNormal();
	FVector Norm = ZAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewY | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewX = (NewY ^ Norm).GetSafeNormal();
	const FVector NewZ = NewX ^ NewY;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromZX(FVector const& ZAxis, FVector const& XAxis)
{
	FVector const NewZ = ZAxis.GetSafeNormal();
	FVector Norm = XAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewZ | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewY = (NewZ ^ Norm).GetSafeNormal();
	const FVector NewX = NewY ^ NewZ;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}

FMatrix FRotationMatrix::MakeFromZY(FVector const& ZAxis, FVector const& YAxis)
{
	FVector const NewZ = ZAxis.GetSafeNormal();
	FVector Norm = YAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if (FMath::IsNearlyEqual(FMath::Abs(NewZ | Norm), 1.f))
	{
		// make sure we don't ever pick the same as NewX
		Norm = (FMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);
	}

	const FVector NewX = (Norm ^ NewZ).GetSafeNormal();
	const FVector NewY = NewZ ^ NewX;

	return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
}
