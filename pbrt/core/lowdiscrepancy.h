#pragma once

#include "pbrt.h"
#include "rng.h"
#include "sampling.h"


Float RadicalInverse(int baseIndex, uint64_t a);
std::vector<uint16_t> ComputeRadicalInversePermutations(RNG& rng);
static constexpr int PrimeTableSize = 1000;
extern const int Primes[PrimeTableSize];
Float ScrambledRadicalInverse(int baseIndex, uint64_t a, const uint16_t* perm);
extern const int PrimeSums[PrimeTableSize];

inline uint32_t ReverseBits32(uint32_t n)
{
	n = (n << 16) | (n >> 16);
	n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
	n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
	n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
	n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
	return n;
}

inline uint64_t ReverseBit64(uint64_t n)
{
	uint64_t n0 = ReverseBits32((uint32_t)n);
	uint64_t n1 = ReverseBits32((uint32_t)(n>>32));
	return (n0 << 32) | n1;
}

template <int base>
inline uint64_t InverseRadicalInverse(uint64_t inverse, int nDigits)
{
	uint64_t index = 0;
	for (int i = 0; i < nDigits; ++i)
	{
		uint64_t digit = inverse % base;
		inverse /= base;
		index = index * base + digit;
	}
	return index;
}