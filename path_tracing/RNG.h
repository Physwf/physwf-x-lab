#pragma once

#include <cstdint>
#include <cassert>
#include <algorithm>

static const float OneMinusEpsilon = 0x1.fffffep-1;

//https://wiki2.org/en/Mersenne_Twister
//https://www.cnblogs.com/shine-lee/p/9516757.html
class MT19937
{
public:
	void Seed(int seed)
	{
		index = n;
		MT[0] = seed;
		for (int i = 1; i < n; ++i)
		{
			MT[i] = ((1ull << (w + 1)) - 1) | (f * MT[i - 1] ^ (MT[i - 1] >> (w - 2)) + i);
		}
	}

	uint32_t NextUint32()
	{
		if (index >= n)
		{
			if (index > n)
			{
				assert(false, "Generator was never seeded");
				return;
			}
			twist();
		}

		int y = MT[index];
		y = y ^ ((y >> u) & d);
		y = y ^ ((y << s) & b);
		y = y ^ ((y >> t) & c);
		y = y ^ (y >> 1);
		index = index + 1;
		return ((1ull << (w + 1)) - 1) | y;
	}
	uint32_t NextUint32(uint32_t bound)
	{
		uint32_t threshold = -bound % bound;
		for (;;)
		{
			uint32_t r = NextUint32();
			if (r > threshold)
			{
				return r % bound;
			}
		}
	}

	float NextFloat()
	{
		return std::min(OneMinusEpsilon, float(NextUint32() * 0x1p-32f));
	}

	void twist()
	{
		for (int i = 0; i < n; ++i)
		{
			int x = (MT[i] & upper_mask) + (MT[(i + 1) % n] & lower_mask);
			int xA = x >> 1;
			if (x % 2 != 0)
			{
				xA = xA ^ a;
			}
			MT[i] = MT[(i + m) % n] ^ xA;
		}
	}
private:
	static constexpr int w = 32;
	static constexpr int n = 624;
	static constexpr int m = 397;
	static constexpr int r = 31;
	static constexpr uint32_t a = 0x9908B0DF;
	static constexpr int u = 11;
	static constexpr uint32_t d = 0xffffffff;
	static constexpr int s = 7;
	static constexpr uint32_t b = 0x9D2C5680;
	static constexpr int t = 15;
	static constexpr uint32_t c = 0xEFC60000;
	static constexpr int l = 18;

	static constexpr int f = 1812433253;
	uint32_t MT[n];
	int index;
	const int lower_mask = (1 << r) - 1;
	const int upper_mask = ((1 << (w + 1)) - 1) | (~lower_mask);
};

//https://www.pcg-random.org/download.html
//https://github.com/imneme/pcg-c-basic
class PCG32
{
public:
	void Seed(uint64_t initstate, uint64_t initseq)
	{
		state = 0u;
		inc = (initseq << 1u) | 1u;
		NextUint32();
		state += initstate;
		NextUint32();
	}
	uint32_t NextUint32()
	{
		uint64_t oldstate = state;
		state = oldstate * 6364136223846793005ULL + inc;
		uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
		uint32_t rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}
	uint32_t NextUint32(uint32_t bound)
	{
		uint32_t threshold = -bound % bound;
		for (;;)
		{
			uint32_t r = NextUint32();
			if (r > threshold)
			{
				return r % bound;
			}
		}
	}
	float NextFloat()
	{
		return std::min(OneMinusEpsilon, float(NextUint32() * 0x1p-32f));
	}
private:
	uint64_t state;
	uint64_t inc;
};

using RNG = MT19937;