#pragma once

#include "XMath.h"

struct XColor;
struct XVector;
struct XVector4;

struct XLinearColor
{
	float R, G, B, A;

	static float Pow22OneOver255Table[256];

	static float sRGBToLinearTable[256];

	inline XLinearColor() {}
	inline XLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

	XLinearColor(const XColor& Color);

	XLinearColor(const XVector& Vector);

	explicit XLinearColor(const XVector4& Vector);

	XColor ToRGBE() const;

	static XLinearColor FromSRGBColor(const XColor& Color);

	static XLinearColor FromPow22Color(const XColor& Color);

	inline XLinearColor operator+(const XLinearColor& ColorB) const
	{
		return XLinearColor(
			this->R + ColorB.R,
			this->G + ColorB.G,
			this->B + ColorB.B,
			this->A + ColorB.A
		);
	}
	inline XLinearColor& operator+=(const XLinearColor& ColorB)
	{
		R += ColorB.R;
		G += ColorB.G;
		B += ColorB.B;
		A += ColorB.A;
		return *this;
	}

	inline XLinearColor operator-(const XLinearColor& ColorB) const
	{
		return XLinearColor(
			this->R - ColorB.R,
			this->G - ColorB.G,
			this->B - ColorB.B,
			this->A - ColorB.A
		);
	}
	inline XLinearColor& operator-=(const XLinearColor& ColorB)
	{
		R -= ColorB.R;
		G -= ColorB.G;
		B -= ColorB.B;
		A -= ColorB.A;
		return *this;
	}

	inline XLinearColor operator*(const XLinearColor& ColorB) const
	{
		return XLinearColor(
			this->R * ColorB.R,
			this->G * ColorB.G,
			this->B * ColorB.B,
			this->A * ColorB.A
		);
	}
	inline XLinearColor& operator*=(const XLinearColor& ColorB)
	{
		R *= ColorB.R;
		G *= ColorB.G;
		B *= ColorB.B;
		A *= ColorB.A;
		return *this;
	}

	inline XLinearColor operator*(float Scalar) const
	{
		return XLinearColor(
			this->R * Scalar,
			this->G * Scalar,
			this->B * Scalar,
			this->A * Scalar
		);
	}

	inline XLinearColor& operator*=(float Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	inline XLinearColor operator/(const XLinearColor& ColorB) const
	{
		return XLinearColor(
			this->R / ColorB.R,
			this->G / ColorB.G,
			this->B / ColorB.B,
			this->A / ColorB.A
		);
	}
	inline XLinearColor& operator/=(const XLinearColor& ColorB)
	{
		R /= ColorB.R;
		G /= ColorB.G;
		B /= ColorB.B;
		A /= ColorB.A;
		return *this;
	}

	inline XLinearColor operator/(float Scalar) const
	{
		const float	InvScalar = 1.0f / Scalar;
		return XLinearColor(
			this->R * InvScalar,
			this->G * InvScalar,
			this->B * InvScalar,
			this->A * InvScalar
		);
	}
	inline XLinearColor& operator/=(float Scalar)
	{
		const float	InvScalar = 1.0f / Scalar;
		R *= InvScalar;
		G *= InvScalar;
		B *= InvScalar;
		A *= InvScalar;
		return *this;
	}

	// clamped in 0..1 range
	inline XLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
	{
		XLinearColor Ret;

		Ret.R = XMath::Clamp(R, InMin, InMax);
		Ret.G = XMath::Clamp(G, InMin, InMax);
		Ret.B = XMath::Clamp(B, InMin, InMax);
		Ret.A = XMath::Clamp(A, InMin, InMax);

		return Ret;
	}

	inline bool operator==(const XLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	inline bool operator!=(const XLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}

	static XLinearColor FGetHSV(uint8 H, uint8 S, uint8 V);

	/**
	* Makes a random but quite nice color.
	*/
	static XLinearColor MakeRandomColor();

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static XLinearColor MakeFromColorTemperature(float Temp);

	/**
	 * Euclidean distance between two points.
	 */
	static inline float Dist(const XLinearColor &V1, const XLinearColor &V2)
	{
		return XMath::Sqrt(XMath::Square(V2.R - V1.R) + XMath::Square(V2.G - V1.G) + XMath::Square(V2.B - V1.B) + XMath::Square(V2.A - V1.A));
	}

	XLinearColor LinearRGBToHSV() const;

	XLinearColor HSVToLinearRGB() const;

	static XLinearColor LerpUsingHSV(const XLinearColor& From, const XLinearColor& To, const float Progress);

	XColor Quantize() const;

	XColor QuantizeRound() const;

	XColor ToXColor(const bool bSRGB) const;

	XLinearColor Desaturate(float Desaturation) const;

	inline float ComputeLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	inline float GetLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	static const XLinearColor White;
	static const XLinearColor Gray;
	static const XLinearColor Black;
	static const XLinearColor Transparent;
	static const XLinearColor Red;
	static const XLinearColor Green;
	static const XLinearColor Blue;
	static const XLinearColor Yellow;
};

struct XColor
{
	union { struct { uint8 B, G, R, A; }; uint32 AlignmentDummy; };

	uint32& DWColor(void) { return *((uint32*)this); }
	const uint32& DWColor(void) const { return *((uint32*)this); }

	inline XColor() {}

	inline XColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
	{
		R = InR;
		G = InG;
		B = InB;
		A = InA;
	}
	inline explicit XColor(uint32 InColor)
	{
		DWColor() = InColor;
	}

	// Operators.
	inline bool operator==(const XColor &C) const
	{
		return DWColor() == C.DWColor();
	}

	inline bool operator!=(const XColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	inline void operator+=(const XColor& C)
	{
		R = (uint8)XMath::Min((int32)R + (int32)C.R, 255);
		G = (uint8)XMath::Min((int32)G + (int32)C.G, 255);
		B = (uint8)XMath::Min((int32)B + (int32)C.B, 255);
		A = (uint8)XMath::Min((int32)A + (int32)C.A, 255);
	}

	 XLinearColor FromRGBE() const;

	static XColor MakeRandomColor();

	/**
	 * Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
	 */
	static XColor MakeRedToGreenColorFromScalar(float Scalar);

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static XColor MakeFromColorTemperature(float Temp);

	/**
	 *	@return a new XColor based of this color with the new alpha value.
	 *	Usage: const XColor& MyColor = XColorList::Green.WithAlpha(128);
	 */
	XColor WithAlpha(uint8 Alpha) const
	{
		return XColor(R, G, B, Alpha);
	}

	/**
	 * Reinterprets the color as a linear color.
	 *
	 * @return The linear color representation.
	 */
	inline XLinearColor ReinterpretAsLinear() const
	{
		return XLinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
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
	static const XColor White;
	static const XColor Black;
	static const XColor Transparent;
	static const XColor Red;
	static const XColor Green;
	static const XColor Blue;
	static const XColor Yellow;
	static const XColor Cyan;
	static const XColor Magenta;
	static const XColor Orange;
	static const XColor Purple;
	static const XColor Turquoise;
	static const XColor Silver;
	static const XColor Emerald;
};

inline XLinearColor operator*(float Scalar, const XLinearColor& Color)
{
	return Color.operator*(Scalar);
}