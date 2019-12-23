#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/XMath.h"
#include "Math/XMathSSE.h"
#include "Math/NumericLimits.h"

struct FPackedNormal
{
	union
	{
		struct
		{
			int8	X, Y, Z, W;

		};
		uint32		Packed;
	}				Vector;

	// Constructors.

	FPackedNormal() { Vector.Packed = 0; }
	FPackedNormal(const FVector& InVector) { *this = InVector; }
	FPackedNormal(const FVector4& InVector) { *this = InVector; }

	void operator=(const FVector& InVector);
	void operator=(const FVector4& InVector);
	VectorRegister GetVectorRegister() const;

	FVector ToFVector() const;
	FVector4 ToFVector4() const;

	// Set functions.
	void Set(const FVector& InVector) { *this = InVector; }

	// Equality operator.

	bool operator==(const FPackedNormal& B) const;
	bool operator!=(const FPackedNormal& B) const;
};


inline void FPackedNormal::operator=(const FVector& InVector)
{
	const float Scale = MAX_int8;
	Vector.X = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.X * Scale), MIN_int8, MAX_int8);
	Vector.Y = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Y * Scale), MIN_int8, MAX_int8);
	Vector.Z = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Z * Scale), MIN_int8, MAX_int8);
	Vector.W = MAX_int8;
}

inline void FPackedNormal::operator=(const FVector4& InVector)
{
	const float Scale = MAX_int8;
	Vector.X = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.X * Scale), MIN_int8, MAX_int8);
	Vector.Y = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Y * Scale), MIN_int8, MAX_int8);
	Vector.Z = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Z * Scale), MIN_int8, MAX_int8);
	Vector.W = (int8)FMath::Clamp<int32>(FMath::RoundToInt(InVector.W * Scale), MIN_int8, MAX_int8);
}

inline bool FPackedNormal::operator==(const FPackedNormal& B) const
{
	return Vector.Packed == B.Vector.Packed;
}

inline bool FPackedNormal::operator!=(const FPackedNormal& B) const
{
	return !(*this == B);
}

inline FVector FPackedNormal::ToFVector() const
{
	VectorRegister VectorToUnpack = GetVectorRegister();
	// Write to FVector and return it.
	FVector UnpackedVector;
	VectorStoreFloat3(VectorToUnpack, &UnpackedVector);
	return UnpackedVector;
}

inline FVector4 FPackedNormal::ToFVector4() const
{
	VectorRegister VectorToUnpack = GetVectorRegister();
	// Write to FVector4 and return it.
	FVector4 UnpackedVector;
	VectorStore(VectorToUnpack, &UnpackedVector);
	return UnpackedVector;
}

inline VectorRegister FPackedNormal::GetVectorRegister() const
{
	// Rescale [0..255] range to [-1..1]
	VectorRegister VectorToUnpack = VectorLoadSignedByte4(this);
	VectorToUnpack = VectorMultiply(VectorToUnpack, VectorSetFloat1(1.0f / 127.0f));
	// Return unpacked vector register.
	return VectorToUnpack;
}

/** A vector, quantized and packed into 32-bits. */
struct FPackedRGBA16N
{
	struct
	{
		int16 X;
		int16 Y;
		int16 Z;
		int16 W;
	};

	// Constructors.

	FPackedRGBA16N() { X = 0; Y = 0; Z = 0; W = 0; }
	FPackedRGBA16N(const FVector& InVector) { *this = InVector; }
	FPackedRGBA16N(const FVector4& InVector) { *this = InVector; }
	//FPackedRGBA16N(uint16 InX, uint16 InY, uint16 InZ, uint16 InW) { X = InX; Y = InY; Z = InZ; W = InW; }

	// Conversion operators.

	void operator=(const FVector& InVector);
	void operator=(const FVector4& InVector);

	FVector ToFVector() const;
	FVector4 ToFVector4() const;

	VectorRegister GetVectorRegister() const;

	// Set functions.
	void Set(const FVector& InVector) { *this = InVector; }
	void Set(const FVector4& InVector) { *this = InVector; }

	// Equality operator.

	bool operator==(const FPackedRGBA16N& B) const;
	bool operator!=(const FPackedRGBA16N& B) const;
};

inline void FPackedRGBA16N::operator=(const FVector& InVector)
{
	const float Scale = MAX_int16;
	X = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.X * Scale), MIN_int16, MAX_int16);
	Y = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Y * Scale), MIN_int16, MAX_int16);
	Z = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Z * Scale), MIN_int16, MAX_int16);
	W = MAX_int16;
}

inline void FPackedRGBA16N::operator=(const FVector4& InVector)
{
	const float Scale = MAX_int16;
	X = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.X * Scale), MIN_int16, MAX_int16);
	Y = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Y * Scale), MIN_int16, MAX_int16);
	Z = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.Z * Scale), MIN_int16, MAX_int16);
	W = (int16)FMath::Clamp<int32>(FMath::RoundToInt(InVector.W * Scale), MIN_int16, MAX_int16);
}

inline bool FPackedRGBA16N::operator==(const FPackedRGBA16N& B) const
{
	return (X == B.X) && (Y == B.Y) && (Z == B.Z) && (W == B.W);
}

inline bool FPackedRGBA16N::operator!=(const FPackedRGBA16N& B) const
{
	return !(*this == B);
}

inline FVector FPackedRGBA16N::ToFVector() const
{
	VectorRegister VectorToUnpack = GetVectorRegister();
	// Write to FVector and return it.
	FVector UnpackedVector;
	VectorStoreFloat3(VectorToUnpack, &UnpackedVector);
	return UnpackedVector;
}

inline FVector4 FPackedRGBA16N::ToFVector4() const
{
	VectorRegister VectorToUnpack = GetVectorRegister();
	// Write to FVector4 and return it.
	FVector4 UnpackedVector;
	VectorStore(VectorToUnpack, &UnpackedVector);
	return UnpackedVector;
}

inline VectorRegister FPackedRGBA16N::GetVectorRegister() const
{
	VectorRegister VectorToUnpack = VectorLoadSRGBA16N((void*)this);
	VectorToUnpack = VectorMultiply(VectorToUnpack, VectorSetFloat1(1.0f / 32767.0f));
	// Return unpacked vector register.
	return VectorToUnpack;
}
