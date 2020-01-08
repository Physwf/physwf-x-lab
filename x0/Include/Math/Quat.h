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
