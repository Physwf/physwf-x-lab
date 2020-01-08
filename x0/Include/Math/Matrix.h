#pragma once

#include "X0.h"

#include "CoreTypes.h"
#include "Math/XMathUtility.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Plane.h"
#include "Math/Rotator.h"
#include "Math/Axis.h"
#include "Misc/AssertionMacros.h"

struct FMatrix
{
public:
	union
	{
		alignas(16) float M[4][4];
	};

	alignas(16) static X0_API const FMatrix Identity;

	inline FMatrix();

	inline FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW);

	inline FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW);

	inline void SetIdentity();

	inline FMatrix operator* (const FMatrix& Other) const;

	inline void operator*=(const FMatrix& Other);

	inline FMatrix operator+ (const FMatrix& Other) const;

	inline void operator+=(const FMatrix& Other);

	inline FMatrix operator* (float Other) const;

	inline void operator*=(float Other);

	inline bool operator==(const FMatrix& Other) const;

	inline bool Equals(const FMatrix& Other, float Tolerance = KINDA_SMALL_NUMBER) const;

	inline bool operator!=(const FMatrix& Other) const;

	inline FVector4 TransformFVector4(const FVector4& V) const;

	inline FVector4 TransformPosition(const FVector &V) const;

	inline FVector InverseTransformPosition(const FVector &V) const;

	inline FVector4 TransformVector(const FVector& V) const;

	inline FVector InverseTransformVector(const FVector &V) const;

	inline FMatrix GetTransposed() const;

	inline float Determinant() const;

	inline float RotDeterminant() const;

	inline FMatrix InverseFast() const;

	inline FMatrix Inverse() const;

	inline FMatrix TransposeAdjoint() const;

	inline void RemoveScaling(float Tolerance = SMALL_NUMBER);

	inline FMatrix GetMatrixWithoutScale(float Tolerance = SMALL_NUMBER) const;

	inline FVector ExtractScaling(float Tolerance = SMALL_NUMBER);

	inline FVector GetScaleVector(float Tolerance = SMALL_NUMBER) const;

	inline FMatrix RemoveTranslation() const;

	inline FMatrix ConcatTranslation(const FVector& Translation) const;

	inline bool ContainsNaN() const;

	inline void ScaleTranslation(const FVector& Scale3D);

	inline float GetMaximumAxisScale() const;

	inline FMatrix ApplyScale(float Scale);

	inline FVector GetOrigin() const;

	inline FVector GetScaledAxis(EAxis::Type Axis) const;

	inline void GetScaledAxes(FVector &X, FVector &Y, FVector &Z) const;

	inline FVector GetUnitAxis(EAxis::Type Axis) const;

	inline void GetUnitAxes(FVector &X, FVector &Y, FVector &Z) const;

	inline void SetAxis(int32 i, const FVector& Axis);

	inline void SetOrigin(const FVector& NewOrigin);

	inline void SetAxes(FVector* Axis0 = NULL, FVector* Axis1 = NULL, FVector* Axis2 = NULL, FVector* Origin = NULL);

	inline FVector GetColumn(int32 i) const;

	//X0_API FRotator Rotator() const;

	//X0_API FQuat ToQuat() const;

	inline bool GetFrustumNearPlane(FPlane& OutPlane) const;

	inline bool GetFrustumFarPlane(FPlane& OutPlane) const;

	inline bool GetFrustumLeftPlane(FPlane& OutPlane) const;

	inline bool GetFrustumRightPlane(FPlane& OutPlane) const;

	inline bool GetFrustumTopPlane(FPlane& OutPlane) const;

	inline bool GetFrustumBottomPlane(FPlane& OutPlane) const;

	inline void Mirror(EAxis::Type MirrorAxis, EAxis::Type FlipAxis);

	//X0_API uint32 ComputeHash() const;

	void To3x4MatrixTranspose(float* Out) const
	{
		const float* __restrict Src = &(M[0][0]);
		float* __restrict Dest = Out;

		Dest[0] = Src[0];   // [0][0]
		Dest[1] = Src[4];   // [1][0]
		Dest[2] = Src[8];   // [2][0]
		Dest[3] = Src[12];  // [3][0]

		Dest[4] = Src[1];   // [0][1]
		Dest[5] = Src[5];   // [1][1]
		Dest[6] = Src[9];   // [2][1]
		Dest[7] = Src[13];  // [3][1]

		Dest[8] = Src[2];   // [0][2]
		Dest[9] = Src[6];   // [1][2]
		Dest[10] = Src[10]; // [2][2]
		Dest[11] = Src[14]; // [3][2]
	}

};

#include "Math/Matrix.inl"
