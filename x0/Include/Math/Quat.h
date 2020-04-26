#pragma once

#include "X0.h"
#include "Math/Vector.h"
#include "Math/VectorRegister.h"
#include "Math/Rotator.h"
#include "Math/Matrix.h"

struct alignas(16) FQuat
{
	float X, Y, Z, W;
public:
	static X0_API const FQuat Identity;
public:
	inline FQuat() { }

	inline FQuat(float InX, float InY, float InZ, float InW);

	inline FQuat(const FQuat& Q);

	explicit FQuat(const FMatrix& M);

	explicit FQuat(const FRotator& R);

	FQuat(FVector Axis, float AngleRad);

public:
	inline FQuat& operator=(const FQuat& Other);

	inline FQuat operator+(const FQuat& Q) const;

	inline FQuat operator+=(const FQuat& Q);

	inline FQuat operator-(const FQuat& Q) const;

	inline FQuat operator-=(const FQuat& Q);

	inline FQuat operator*(const FQuat& Q) const;

	inline FQuat operator*=(const FQuat& Q);

	FVector operator*(const FVector& V) const;

	FMatrix operator*(const FMatrix& M) const;

	inline FQuat operator*=(const float Scale);

	inline FQuat operator*(const float Scale) const;

	inline FQuat operator/=(const float Scale);

	inline FQuat operator/(const float Scale) const;

	bool operator==(const FQuat& Q) const;

	bool operator!=(const FQuat& Q) const;

	float operator|(const FQuat& Q) const;

public:
	/**
	 * Convert a vector of floating-point Euler angles (in degrees) into a Quaternion.
	 *
	 * @param Euler the Euler angles
	 * @return constructed FQuat
	 */
	static X0_API FQuat MakeFromEuler(const FVector& Euler);

	/** Convert a Quaternion into floating-point Euler angles (in degrees). */
	X0_API FVector Euler() const;

	/**
	 * Normalize this quaternion if it is large enough.
	 * If it is too small, returns an identity quaternion.
	 *
	 * @param Tolerance Minimum squared length of quaternion for normalization.
	 */
	FORCEINLINE void Normalize(float Tolerance = SMALL_NUMBER);

	/**
	 * Get a normalized copy of this quaternion.
	 * If it is too small, returns an identity quaternion.
	 *
	 * @param Tolerance Minimum squared length of quaternion for normalization.
	 */
	FORCEINLINE FQuat GetNormalized(float Tolerance = SMALL_NUMBER) const;

	// Return true if this quaternion is normalized
	bool IsNormalized() const;

	/**
	 * Get the length of this quaternion.
	 *
	 * @return The length of this quaternion.
	 */
	FORCEINLINE float Size() const;

	/**
	 * Get the length squared of this quaternion.
	 *
	 * @return The length of this quaternion.
	 */
	FORCEINLINE float SizeSquared() const;

	FVector RotateVector(FVector V) const;

	inline FQuat Inverse() const;

	X0_API FRotator Rotator() const;
};

inline FQuat::FQuat(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{

}

inline FQuat::FQuat(const FQuat& Q)
	: X(Q.X)
	, Y(Q.Y)
	, Z(Q.Z)
	, W(Q.W)
{

}

inline FQuat::FQuat(const FRotator& R)
{
	*this = R.Quaternion();
}

inline FQuat::FQuat(const FMatrix& M)
{

}

inline FQuat::FQuat(FVector Axis, float AngleRad)
{
	const float half_a = 0.5f * AngleRad;
	float s, c;
	FMath::SinCos(&s, &c, half_a);

	X = s * Axis.X;
	Y = s * Axis.Y;
	Z = s * Axis.Z;
	W = c;
}

inline FQuat FQuat::operator+(const FQuat& Q) const
{
	return FQuat(X + Q.X, Y + Q.Y, Z + Q.Z, W + Q.W);
}

inline FQuat FQuat::operator+=(const FQuat& Q)
{
	X += Q.X;
	Y += Q.Y;
	Z += Q.Z;
	W += Q.W;

	return *this;
}

inline FQuat FQuat::operator-(const FQuat& Q) const
{
	return FQuat(X - Q.X, Y - Q.Y, Z - Q.Z, W - Q.W);
}

inline FQuat FQuat::operator-=(const FQuat& Q)
{
	X -= Q.X;
	Y -= Q.Y;
	Z -= Q.Z;
	W -= Q.W;

	return *this;
}

inline FQuat FQuat::operator*(const FQuat& Q) const
{
	FQuat Result;
	VectorQuaternionMultiply(&Result, this, &Q);
	return Result;
}

inline FQuat FQuat::operator*=(const FQuat& Q)
{
	VectorRegister A = VectorLoadAligned(this);
	VectorRegister B = VectorLoadAligned(&Q);
	VectorRegister Result;
	VectorQuaternionMultiply(&Result, &A, &B);
	VectorStoreAligned(Result, this);

	return *this;
}

inline FVector FQuat::operator*(const FVector& V) const
{
	return RotateVector(V);
}

inline FMatrix FQuat::operator*(const FMatrix& M) const
{
	FMatrix Result;
	FQuat VT, VR;
	FQuat Inv = Inverse();
	for (int32 I = 0; I < 4; ++I)
	{
		FQuat VQ(M.M[I][0], M.M[I][1], M.M[I][2], M.M[I][3]);
		VectorQuaternionMultiply(&VT, this, &VQ);
		VectorQuaternionMultiply(&VR, &VT, &Inv);
		Result.M[I][0] = VR.X;
		Result.M[I][1] = VR.Y;
		Result.M[I][2] = VR.Z;
		Result.M[I][3] = VR.W;
	}

	return Result;
}

inline FQuat FQuat::operator*=(const float Scale)
{
	X *= Scale;
	Y *= Scale;
	Z *= Scale;
	W *= Scale;

	return *this;
}

inline FQuat FQuat::operator*(const float Scale) const
{
	return FQuat(Scale * X, Scale * Y, Scale * Z, Scale * W);
}

inline FQuat FQuat::operator/=(const float Scale)
{
	const float Recip = 1.0f / Scale;
	X *= Recip;
	Y *= Recip;
	Z *= Recip;
	W *= Recip;

	return *this;
}

inline FQuat FQuat::operator/(const float Scale) const
{
	const float Recip = 1.0f / Scale;
	return FQuat(X * Recip, Y * Recip, Z * Recip, W * Recip);
}

inline float FQuat::operator|(const FQuat& Q) const
{
	return X * Q.X + Y * Q.Y + Z * Q.Z + W * Q.W;
}

FORCEINLINE void FQuat::Normalize(float Tolerance)
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister Vector = VectorLoadAligned(this);

	const VectorRegister SquareSum = VectorDot4(Vector, Vector);
	const VectorRegister NonZeroMask = VectorCompareGE(SquareSum, VectorLoadFloat1(&Tolerance));
	const VectorRegister InvLength = VectorReciprocalSqrtAccurate(SquareSum);
	const VectorRegister NormalizedVector = VectorMultiply(InvLength, Vector);
	VectorRegister Result = VectorSelect(NonZeroMask, NormalizedVector, GlobalVectorConstants::Float0001);

	VectorStoreAligned(Result, this);
#else
	const float SquareSum = X * X + Y * Y + Z * Z + W * W;

	if (SquareSum >= Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);

		X *= Scale;
		Y *= Scale;
		Z *= Scale;
		W *= Scale;
	}
	else
	{
		*this = FQuat::Identity;
	}
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
}

FORCEINLINE FQuat FQuat::GetNormalized(float Tolerance) const
{
	FQuat Result(*this);
	Result.Normalize(Tolerance);
	return Result;
}

FORCEINLINE bool FQuat::IsNormalized() const
{
	return (FMath::Abs(1.f - SizeSquared()) < THRESH_QUAT_NORMALIZED);
}

FORCEINLINE float FQuat::Size() const
{
	return FMath::Sqrt(X * X + Y * Y + Z * Z + W * W);
}


FORCEINLINE float FQuat::SizeSquared() const
{
	return (X * X + Y * Y + Z * Z + W * W);
}

inline FVector FQuat::RotateVector(FVector V) const
{
	const FVector Q(X, Y, Z);
	const FVector T = 2.f * FVector::CrossProduct(Q, V);
	const FVector Result = V + (W * T) + FVector::CrossProduct(Q, T);
	return Result;
}

inline FQuat FQuat::Inverse() const
{
	return FQuat(-X, -Y, -Z, W);
}

inline bool FQuat::operator!=(const FQuat& Q) const
{
	return X != Q.X || Y != Q.Y || Z != Q.Z || W != Q.W;
}

inline bool FQuat::operator==(const FQuat& Q) const
{
	return X == Q.X && Y == Q.Y && Z == Q.Z && W == Q.W;
}

inline FQuat& FQuat::operator=(const FQuat& Other)
{
	X = Other.X;
	Y = Other.Y;
	Z = Other.Z;
	W = Other.W;

	return *this;
}
