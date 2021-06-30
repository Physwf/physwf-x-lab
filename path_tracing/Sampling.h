#pragma once

#include <vector>
#include <algorithm>

#include "Vector.h"
#include "RNG.h"

struct Distribution1D
{

	Distribution1D(const float* f, int n):func(f,f+n), cdf(n+1)
	{
		cdf[0] = 0;
		for (int i = 1; i < n + 1; ++i) cdf[i] = cdf[i - 1] + func[i - 1] / n;
		funcInt = cdf[n];

		if (funcInt == 0)
		{
			for (int i = 1; i < n + 1; ++i) cdf[i] = float(i) / float(n);
		}
		else
		{
			for (int i = 1; i < n + 1; ++i) cdf[i] /= funcInt;
		}
	}

	int Count() const { return (int)func.size(); }

	float SampleContinuous(float u, float* pdf, int* off = nullptr) const
	{

	}
	int SampleDiscrete(float u, float* pdf = nullptr, float* uRemapped = nullptr) const
	{

	}
	float DiscretePDF(int index) const
	{

	}

	std::vector<float> func, cdf;
	float funcInt;
};

class Distribution2D
{

};

Vector2f RejectionSampleDisk(RNG& rng);

Vector3f UniformSampleHemisphere(const Vector2f& u);
float UniformSampleHemispherePdf();

Vector3f UniformSampleSphere(const Vector2f& u);
float UniformSpherePdf();

Vector3f UniformSampleCone(const Vector2f& u, float thetamax);
Vector3f UniformSampleCone(const Vector2f& u, float thetamax, const Vector3f& x, const Vector3f& y, const Vector3f& z);
float UniformConePdf(float thetamax);

Vector2f UniformSampleDisk(const Vector2f& u);
Vector2f ConcentircSampleDisk(const Vector2f& u);
Vector2f UniformSampleTriangle(const Vector2f& u);

inline Vector3f CosineSampleHemisphere(const Vector2f& u)
{
	Vector2f d = ConcentircSampleDisk(u);
	float z = std::sqrt(std::max(0.f, 1 - d.X * d.X - d.Y * d.Y));
	return Vector3f(d.X, d.Y, z);
}

inline float CosineHemispherePdf(float cosTheta)
{
	return cosTheta * 1.f / PI;
}


