#pragma once

#include "X0.h"
struct FVector;
struct FQuat;

struct FRotator
{
public:
	float Pitch;
	float Yaw;
	float Roll;

public:
	static X0_API const FRotator ZeroRotator;

public:
	FRotator() { }
	explicit inline FRotator(float InF);
	inline FRotator(float InPitch, float InYaw, float InRoll);
	explicit X0_API FRotator(const FQuat& Quat);

public:
	FRotator operator+(const FRotator& R) const;
	FRotator operator-(const FRotator& R) const;
	FRotator operator*(float Scale) const;
	FRotator operator*=(float Scale);
	bool operator==(const FRotator& R) const;
	bool operator!=(const FRotator& V) const;
	FRotator operator+=(const FRotator& R);
	FRotator operator-=(const FRotator& R);

	FRotator Add(float DeltaPitch, float DeltaYaw, float DeltaRoll);

	X0_API FRotator GetInverse() const;

	X0_API FVector Vector() const;

	X0_API FQuat Quaternion() const;

	X0_API FVector Euler() const;

	X0_API FVector RotateVector(const FVector& V) const;

	X0_API FVector UnrotateVector(const FVector& V) const;

	FRotator Clamp() const;

	FRotator GetNormalized() const;

	FRotator GetDenormalized() const;

	void Normalize();

	float GetManhattanDistance(const FRotator & Rotator) const;

	FRotator GetEquivalentRotator() const;
};

inline FRotator::FRotator(float InF)
	:Pitch(InF),Yaw(InF),Roll(InF)
{

}

FRotator::FRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{

}

FRotator FRotator::operator+(const FRotator& R) const
{
	return FRotator(Pitch + R.Pitch, Yaw + R.Yaw, Roll + R.Roll);
}

FRotator FRotator::operator-(const FRotator& R) const
{
	return FRotator(Pitch - R.Pitch, Yaw - R.Yaw, Roll - R.Roll);
}

FRotator FRotator::operator*(float Scale) const
{
	return FRotator(Pitch*Scale, Yaw*Scale, Roll*Scale);
}

FRotator FRotator::operator*=(float Scale)
{
	Pitch = Pitch * Scale; Yaw = Yaw * Scale; Roll = Roll * Scale;
	return *this;
}

bool FRotator::operator==(const FRotator& R) const
{
	return Pitch == R.Pitch && Yaw == R.Yaw && Roll == R.Roll;
}

bool FRotator::operator!=(const FRotator& V) const
{
	return Pitch != V.Pitch || Yaw != V.Yaw || Roll != V.Roll;
}

FRotator FRotator::operator+=(const FRotator& R)
{
	Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
	return *this;
}

FRotator FRotator::operator-=(const FRotator& R)
{
	Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
	return *this;
}

FRotator FRotator::Add(float DeltaPitch, float DeltaYaw, float DeltaRoll)
{
	Yaw += DeltaYaw;
	Pitch += DeltaPitch;
	Roll += DeltaRoll;
	return *this;
}

X0_API FRotator FRotator::GetInverse() const
{

}

X0_API FVector FRotator::Euler() const
{
	return FVector(Roll, Pitch, Yaw);
}

