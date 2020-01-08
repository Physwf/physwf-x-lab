#include "Math/XMath.h"


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


