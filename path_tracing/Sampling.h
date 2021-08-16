#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include "Vector.h"
#include "RNG.h"
#include "Math.h"

template <typename T>
void Shuffle(T* samp, int count, int nDimensions, RNG& rng) {
	for (int i = 0; i < count; ++i) {
		int other = i + rng.NextUint32(count - i);
		for (int j = 0; j < nDimensions; ++j)
			std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
	}
}

void StratifiedSample1D(float* samples, int nSamples, RNG& rng, bool bJitter = true);
void StratifiedSample2D(Vector2f* samples, int nx, int ny, RNG& rng, bool bJitter = true);
void LatinHypercube(float* samples, int nSamples, int nDim, RNG& rng);

struct Distribution1D
{
	Distribution1D(const float* f, int n) :func(f, f + n), cdf(n + 1)
	{
		cdf[0] = 0;
		for (int i = 1; i < n + 1; ++i) cdf[i] = cdf[i - 1] + func[i - 1] / n;
		funcIntegral = cdf[n];

		if (funcIntegral == 0)
		{
			for (int i = 1; i < n + 1; ++i) cdf[i] = float(i) / float(n);
		}
		else
		{
			for (int i = 1; i < n + 1; ++i) cdf[i] /= funcIntegral;
		}
	}

	int Count() const { return (int)func.size(); }

	float SampleContinuous(float u, float* pdf, int* off = nullptr) const
	{
		int offset = Math::FindInterval((int)cdf.size(), [&](int index) { return cdf[index] <= u; });
		if (off) *off = offset;

		float du = u - cdf[offset];
		if ((cdf[offset + 1] - cdf[offset]) > 0)
		{
			du /= (cdf[offset + 1] - cdf[offset]);
		}

		if (pdf) *pdf = (funcIntegral > 0) ? func[offset] / funcIntegral : 0;

		return (offset + du) / Count();
	}
	int SampleDiscrete(float u, float* pdf = nullptr, float* uRemapped = nullptr) const
	{
		int offset = Math::FindInterval((int)cdf.size(), [&](int index) { return cdf[index] <= u; });
		if (pdf) *pdf = funcIntegral > 0 ? func[offset] / (funcIntegral * Count()) : 0;
		if (uRemapped) *uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
		return offset;
	}
	float DiscretePDF(int index) const
	{
		return func[index] / (funcIntegral * Count());
	}

	std::vector<float> func, cdf;
	float funcIntegral;
};

class Distribution2D
{
public:
	Distribution2D(const float* data, int nu, int nv);
	Vector2f SampleContinuous(const Vector2f& u, float* pdf) const
	{
		float pdfs[2];
		int v;
		float d1 = pMarginal->SampleContinuous(u[1], &pdfs[1], &v);
		float d0 = pConditionalV[v]->SampleContinuous(u[0], &pdfs[0]);
		*pdf = pdfs[0] * pdfs[1];
		return Vector2f(d0, d1);
	}
	float Pdf(const Vector2f& p) const
	{
		int iu = Math::Clamp(int(p[0] * pConditionalV[0]->Count() - 1), 0, pConditionalV[0]->Count() - 1);
		int iv = Math::Clamp(int(p[1] * pMarginal->Count()), 0, pMarginal->Count() - 1);
		return pConditionalV[iv]->func[iu] / pMarginal->funcIntegral;
	}
private:
	std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
	std::unique_ptr<Distribution1D> pMarginal;
};


Vector2f RejectionSampleDisk(RNG& rng);

Vector3f UniformSampleHemisphere(const Vector2f& u);
float UniformHemispherePdf();

Vector3f UniformSampleSphere(const Vector2f& u);
float UniformSpherePdf();

Vector3f UniformSampleCone(const Vector2f& u, float thetamax);
Vector3f UniformSampleCone(const Vector2f& u, float thetamax, const Vector3f& x, const Vector3f& y, const Vector3f& z);
float UniformConePdf(float thetamax);

Vector2f UniformSampleDisk(const Vector2f& u);
Vector2f ConcentricSampleDisk(const Vector2f& u);
Vector2f UniformSampleTriangle(const Vector2f& u);

inline Vector3f CosineSampleHemisphere(const Vector2f& u)
{
	Vector2f d = ConcentricSampleDisk(u);
	float z = std::sqrt(std::max(0.f, 1 - d.X * d.X - d.Y * d.Y));
	return Vector3f(d.X, d.Y, z);
}

inline float CosineHemispherePdf(float cosTheta)
{
	return cosTheta * 1.f / PI;
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
	float f = nf * fPdf, g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}

