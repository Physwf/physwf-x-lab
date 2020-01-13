#pragma once

#include "X0.h"
#include "Math/XMathUtility.h"

struct FColor;

struct FLinearColor
{
	float R, G, B, A;

	static float Pow22OneOver255Table[256];

	static float sRGBToLinearTable[256];

	inline FLinearColor() {}
	inline FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

	X0_API FLinearColor(const FColor& Color);

	X0_API FLinearColor(const FVector& Vector);

	X0_API explicit FLinearColor(const FVector4& Vector);

	FColor ToRGBE() const;

	static FLinearColor FromSRGBColor(const FColor& Color);

	static FLinearColor FromPow22Color(const FColor& Color);

	inline FLinearColor operator+(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R + ColorB.R,
			this->G + ColorB.G,
			this->B + ColorB.B,
			this->A + ColorB.A
		);
	}
	inline FLinearColor& operator+=(const FLinearColor& ColorB)
	{
		R += ColorB.R;
		G += ColorB.G;
		B += ColorB.B;
		A += ColorB.A;
		return *this;
	}

	inline FLinearColor operator-(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R - ColorB.R,
			this->G - ColorB.G,
			this->B - ColorB.B,
			this->A - ColorB.A
		);
	}
	inline FLinearColor& operator-=(const FLinearColor& ColorB)
	{
		R -= ColorB.R;
		G -= ColorB.G;
		B -= ColorB.B;
		A -= ColorB.A;
		return *this;
	}

	inline FLinearColor operator*(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R * ColorB.R,
			this->G * ColorB.G,
			this->B * ColorB.B,
			this->A * ColorB.A
		);
	}
	inline FLinearColor& operator*=(const FLinearColor& ColorB)
	{
		R *= ColorB.R;
		G *= ColorB.G;
		B *= ColorB.B;
		A *= ColorB.A;
		return *this;
	}

	inline FLinearColor operator*(float Scalar) const
	{
		return FLinearColor(
			this->R * Scalar,
			this->G * Scalar,
			this->B * Scalar,
			this->A * Scalar
		);
	}

	inline FLinearColor& operator*=(float Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	inline FLinearColor operator/(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R / ColorB.R,
			this->G / ColorB.G,
			this->B / ColorB.B,
			this->A / ColorB.A
		);
	}
	inline FLinearColor& operator/=(const FLinearColor& ColorB)
	{
		R /= ColorB.R;
		G /= ColorB.G;
		B /= ColorB.B;
		A /= ColorB.A;
		return *this;
	}

	inline FLinearColor operator/(float Scalar) const
	{
		const float	InvScalar = 1.0f / Scalar;
		return FLinearColor(
			this->R * InvScalar,
			this->G * InvScalar,
			this->B * InvScalar,
			this->A * InvScalar
		);
	}
	inline FLinearColor& operator/=(float Scalar)
	{
		const float	InvScalar = 1.0f / Scalar;
		R *= InvScalar;
		G *= InvScalar;
		B *= InvScalar;
		A *= InvScalar;
		return *this;
	}

	// clamped in 0..1 range
	inline FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
	{
		FLinearColor Ret;

		Ret.R = FMath::Clamp(R, InMin, InMax);
		Ret.G = FMath::Clamp(G, InMin, InMax);
		Ret.B = FMath::Clamp(B, InMin, InMax);
		Ret.A = FMath::Clamp(A, InMin, InMax);

		return Ret;
	}

	inline bool operator==(const FLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	inline bool operator!=(const FLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}

	static FLinearColor FGetHSV(uint8 H, uint8 S, uint8 V);

	/**
	* Makes a random but quite nice color.
	*/
	static FLinearColor MakeRandomColor();

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static FLinearColor MakeFromColorTemperature(float Temp);

	/**
	 * Euclidean distance between two points.
	 */
	static inline float Dist(const FLinearColor &V1, const FLinearColor &V2)
	{
		return FMath::Sqrt(FMath::Square(V2.R - V1.R) + FMath::Square(V2.G - V1.G) + FMath::Square(V2.B - V1.B) + FMath::Square(V2.A - V1.A));
	}

	FLinearColor LinearRGBToHSV() const;

	FLinearColor HSVToLinearRGB() const;

	static FLinearColor LerpUsingHSV(const FLinearColor& From, const FLinearColor& To, const float Progress);

	FColor Quantize() const;

	FColor QuantizeRound() const;

	FColor ToXColor(const bool bSRGB) const;

	FLinearColor Desaturate(float Desaturation) const;

	inline float ComputeLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	inline float GetLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	X0_API static const FLinearColor White;
	X0_API static const FLinearColor Gray;
	X0_API static const FLinearColor Black;
	X0_API static const FLinearColor Transparent;
	X0_API static const FLinearColor Red;
	X0_API static const FLinearColor Green;
	X0_API static const FLinearColor Blue;
	X0_API static const FLinearColor Yellow;
};

struct FColor
{
	union { struct { uint8 B, G, R, A; }; uint32 AlignmentDummy; };

	uint32& DWColor(void) { return *((uint32*)this); }
	const uint32& DWColor(void) const { return *((uint32*)this); }

	inline FColor() {}

	inline FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
	{
		R = InR;
		G = InG;
		B = InB;
		A = InA;
	}
	inline explicit FColor(uint32 InColor)
	{
		DWColor() = InColor;
	}

	// Operators.
	inline bool operator==(const FColor &C) const
	{
		return DWColor() == C.DWColor();
	}

	inline bool operator!=(const FColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	inline void operator+=(const FColor& C)
	{
		R = (uint8)FMath::Min((int32)R + (int32)C.R, 255);
		G = (uint8)FMath::Min((int32)G + (int32)C.G, 255);
		B = (uint8)FMath::Min((int32)B + (int32)C.B, 255);
		A = (uint8)FMath::Min((int32)A + (int32)C.A, 255);
	}

	 FLinearColor FromRGBE() const;

	static FColor MakeRandomColor();

	/**
	 * Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
	 */
	static FColor MakeRedToGreenColorFromScalar(float Scalar);

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static FColor MakeFromColorTemperature(float Temp);

	/**
	 *	@return a new XColor based of this color with the new alpha value.
	 *	Usage: const XColor& MyColor = XColorList::Green.WithAlpha(128);
	 */
	FColor WithAlpha(uint8 Alpha) const
	{
		return FColor(R, G, B, Alpha);
	}

	/**
	 * Reinterprets the color as a linear color.
	 *
	 * @return The linear color representation.
	 */
	inline FLinearColor ReinterpretAsLinear() const
	{
		return FLinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	}

	inline uint32 ToPackedARGB() const
	{
		return (A << 24) | (R << 16) | (G << 8) | (B << 0);
	}

	/**
	 * Gets the color in a packed uint32 format packed in the order ABGR.
	 */
	inline uint32 ToPackedABGR() const
	{
		return (A << 24) | (B << 16) | (G << 8) | (R << 0);
	}

	/**
	 * Gets the color in a packed uint32 format packed in the order RGBA.
	 */
	inline uint32 ToPackedRGBA() const
	{
		return (R << 24) | (G << 16) | (B << 8) | (A << 0);
	}

	/**
	 * Gets the color in a packed uint32 format packed in the order BGRA.
	 */
	inline uint32 ToPackedBGRA() const
	{
		return (B << 24) | (G << 16) | (R << 8) | (A << 0);
	}

	/** Some pre-inited colors, useful for debug code */
	X0_API static const FColor White;
	X0_API static const FColor Black;
	X0_API static const FColor Transparent;
	X0_API static const FColor Red;
	X0_API static const FColor Green;
	X0_API static const FColor Blue;
	X0_API static const FColor Yellow;
	X0_API static const FColor Cyan;
	X0_API static const FColor Magenta;
	X0_API static const FColor Orange;
	X0_API static const FColor Purple;
	X0_API static const FColor Turquoise;
	X0_API static const FColor Silver;
	X0_API static const FColor Emerald;
};

inline FLinearColor operator*(float Scalar, const FLinearColor& Color)
{
	return Color.operator*(Scalar);
}