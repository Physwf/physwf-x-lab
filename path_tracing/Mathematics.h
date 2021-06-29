#pragma once

#include <cmath>
#include "Vector.h"

#define PI 3.1415926f

class Math
{
public:

	static bool IsNearlyEqual(float a, float b) 
	{
		return std::abs(a - b) < 0.0000001;
	}

	template <typename T>
	static T Clamp(T Value, T Min, T Max)
	{
		if (Value < Min) return Min;
		if (Value > Max) return Max;
		return Value;
	}

	template <typename T>
	T EuclidGCD(T a, T b)
	{
		return a == 0 ? b : EuclidGCD(a, a % b);
	}

	template <typename T>
	T ExtentedEuclidGCD(T a, T b, T& x, T& y)
	{
		if (b == 0)
		{
			x = 1; y = 0;
			return a;
		}
		T gcd = ExtentedEuclidGCD(b, a % b, y, x);
		y -= a / b * x;
		return gcd;
	}

	uint32_t ReverseBits32(uint32_t n)
	{
		n = (n << 16) | (n >> 16);
		n = ((n & 0x00ff00ff) << 8 | (n & 0xff00ff00) >> 8);
		n = ((n & 0x0f0f0f0f) << 4 | (n & 0xf0f0f0f0) >> 4);
		n = ((n & 0x33333333) << 2 | (n & 0x33333333) >> 2);
		n = ((n & 0x55555555) << 1 | (n & 0x55555555) >> 1);
		return n;
	}

	uint64_t ReverseBits64(uint64_t n)
	{
		uint64_t n1 = ReverseBits32((uint32_t)n);
		uint64_t n2 = ReverseBits32((uint32_t)(n>>32));
		return (n1 << 32) | n2;
	}

	template<int Base>
	float RadicalInverseSpecialized(uint64_t n)
	{
		const float invBase = 1.f / Base;
		uint64_t reversedDigits = 0;
		float invBaseN = 1.f;
		while (n)
		{
			uint64_t next = n / Base;
			uint64_t digit = n - next * Base;
			reversedDigits = reversedDigits * Base + digit;
			invBaseN *= invBase;
			n = next;
		}
		return reversedDigits * invBaseN;
	}

	float RadicalInverse(int Base, uint64_t n)
	{
		switch (Base)
		{
		case 2:return ReverseBits64(n) * 0x1p-64;
		case 3:return RadicalInverseSpecialized<3>(n);
		case 5:return RadicalInverseSpecialized<5>(n);
		case 7:return RadicalInverseSpecialized<7>(n);
		case 11:return RadicalInverseSpecialized<11>(n);
		}
	}

	template<int Base>
	static uint64_t InverseRadicalInverse(uint64_t inverse, int nDigits)
	{
		uint64_t index = 0;
		for (int i = 0; i < nDigits; ++i) {
			uint64_t digit = inverse % Base;
			inverse /= Base;
			index = index * Base + digit;
		}
		return index;
	}

	static bool Quadratic(float a, float b, float c,float* t0, float* t1)
	{
		float discrim = b * b - 4 * a * c;
		if (discrim < 0) return false;
		float rootDiscrim = std::sqrt(discrim);

		*t0 = 0.5f * (-b - rootDiscrim) / a;
		*t1 = 0.5f * (-b + rootDiscrim) / a;
		if (*t0 > *t1) std::swap(*t0, *t1);
		return true;
	}
	//https://blog.csdn.net/zhouschina/article/details/8784908
	static bool Plane(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float* a, float* b, float* c, float* d)
	{
		if (p1.IsNearlyEqual(p2) || p1.IsNearlyEqual(p3) || p2.IsNearlyEqual(p3)) return false;
		
		float A = (p2.Y - p1.Y) * (p3.Z - p1.Z) - (p3.Y - p1.Y) * (p2.Z - p1.Z);
		float B = (p2.Z - p1.Z) * (p3.X - p1.X) - (p3.Z - p1.Z) * (p2.X - p1.X);
		float C = (p2.X - p1.X) * (p3.Y - p1.Y) - (p3.X - p1.X) * (p2.Y - p1.Y);
		float D = -A * (p1.X) - B * p1.Y - C * p1.Z;
		*a = A; *b = B; *c = C; *d = D;
		return true;
	}

	static bool IsInsideTriangle(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector3f& p)
	{
		if (Dot(Cross((p2 - p1), (p - p1)) , Cross((p2 - p1), (p3 - p1))) < 0) return false;
		if (Dot(Cross((p3 - p2), (p - p2)) , Cross((p3 - p2), (p1 - p2))) < 0) return false;
		if (Dot(Cross((p1 - p3), (p - p3)) , Cross((p1 - p3), (p2 - p3))) < 0) return false;
		return true;
	}
};