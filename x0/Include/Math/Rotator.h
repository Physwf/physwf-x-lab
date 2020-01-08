#pragma once

#include "X0.h"

#include "CoreTypes.h"
#include "Math/XMathUtility.h"
#include "Math/Vector.h"
#include "Math/VectorRegister.h"

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

	static float ClampAxis(float Angle);

	static float NormalizeAxis(float Angle);
};

inline FRotator::FRotator(float InF)
	:Pitch(InF),Yaw(InF),Roll(InF)
{

}

inline FRotator::FRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{

}

inline FRotator FRotator::operator+(const FRotator& R) const
{
	return FRotator(Pitch + R.Pitch, Yaw + R.Yaw, Roll + R.Roll);
}

inline FRotator FRotator::operator-(const FRotator& R) const
{
	return FRotator(Pitch - R.Pitch, Yaw - R.Yaw, Roll - R.Roll);
}

inline FRotator FRotator::operator*(float Scale) const
{
	return FRotator(Pitch*Scale, Yaw*Scale, Roll*Scale);
}

inline FRotator FRotator::operator*=(float Scale)
{
	Pitch = Pitch * Scale; Yaw = Yaw * Scale; Roll = Roll * Scale;
	return *this;
}

inline bool FRotator::operator==(const FRotator& R) const
{
	return Pitch == R.Pitch && Yaw == R.Yaw && Roll == R.Roll;
}

inline bool FRotator::operator!=(const FRotator& V) const
{
	return Pitch != V.Pitch || Yaw != V.Yaw || Roll != V.Roll;
}

inline FRotator FRotator::operator+=(const FRotator& R)
{
	Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
	return *this;
}

inline FRotator FRotator::operator-=(const FRotator& R)
{
	Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
	return *this;
}

inline FRotator FRotator::Add(float DeltaPitch, float DeltaYaw, float DeltaRoll)
{
	Yaw += DeltaYaw;
	Pitch += DeltaPitch;
	Roll += DeltaRoll;
	return *this;
}


inline FRotator FRotator::Clamp() const
{
	return FRotator(ClampAxis(Pitch), ClampAxis(Yaw), ClampAxis(Roll));
}

inline FRotator FRotator::GetNormalized() const
{
	FRotator Rot = *this;
	Rot.Normalize();
	return Rot;
}

inline FRotator FRotator::GetDenormalized() const
{
	FRotator Rot = *this;
	Rot.Pitch = ClampAxis(Rot.Pitch);
	Rot.Yaw = ClampAxis(Rot.Yaw);
	Rot.Roll = ClampAxis(Rot.Roll);
	return Rot;
}

inline void FRotator::Normalize()
{
	Pitch = NormalizeAxis(Pitch);
	Yaw = NormalizeAxis(Yaw);
	Roll = NormalizeAxis(Roll);
}

inline float FRotator::GetManhattanDistance(const FRotator & Rotator) const
{
	return FMath::Abs<float>(Yaw - Rotator.Yaw) + FMath::Abs<float>(Pitch - Rotator.Pitch) + FMath::Abs<float>(Roll - Rotator.Roll);
}

inline FRotator FRotator::GetEquivalentRotator() const
{
	return FRotator(180.0f - Pitch, Yaw + 180.0f, Roll + 180.0f);
}

inline float FRotator::ClampAxis(float Angle)
{
	Angle = FMath::Fmod(Angle,360.0f);
	if (Angle < 0.0f)
	{
		Angle += 360.0f;
	}
	return Angle;
}

inline float FRotator::NormalizeAxis(float Angle)
{
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		Angle -= 360.0f;
	}
	return Angle;
}

