#pragma once

struct FLinearColor
{
	float	R,
		G,
		B,
		A;
};

struct FColor
{
public:
	// Variables.
#if PLATFORM_LITTLE_ENDIAN
#ifdef _MSC_VER
	// Win32 x86
	union { struct { uint8 B, G, R, A; }; uint32 AlignmentDummy; };
#else
	// Linux x86, etc
	uint8 B GCC_ALIGN(4);
	uint8 G, R, A;
#endif
#else // PLATFORM_LITTLE_ENDIAN
	union { struct { uint8 A, R, G, B; }; uint32 AlignmentDummy; };
#endif

	uint32& DWColor(void) { return *((uint32*)this); }
	const uint32& DWColor(void) const { return *((uint32*)this); }

	// Constructors.
	FColor() {}
	explicit FColor(EForceInit)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = G = B = A = 0;
	}
	FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = InR;
		G = InG;
		B = InB;
		A = InA;
	}
};