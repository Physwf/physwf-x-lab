#include "Sampling.h"

void StratifiedSample1D(float* samples, int nSamples, RNG& rng, bool bJitter /*= true*/)
{
	float invNSamples = 1.f / nSamples;
	for (int i = 0; i < nSamples; ++i)
	{
		float delta = bJitter ? rng.NextFloat() : 0.5f;
		samples[i] = std::min((i + delta) * invNSamples, FloatOneMinusEpsilon);
	}
}

void StratifiedSample2D(Vector2f* samples, int nx, int ny, RNG& rng, bool bJitter /*= true*/)
{
	float dx = 1.f / nx;
	float dy = 1.f / ny;
	for (int y = 0; y < ny; ++y)
	{
		for (int x = 0; x < nx; ++x)
		{
			float jx = bJitter ? rng.NextFloat() : 0.5f;
			float jy = bJitter ? rng.NextFloat() : 0.5f;
			samples->X = std::min((x + jx) * dx, FloatOneMinusEpsilon);
			samples->Y = std::min((y + jy) * dy, FloatOneMinusEpsilon);
			++samples;
		}
	}
}

void LatinHypercube(float* samples, int nSamples, int nDim, RNG& rng)
{
	float invNSamples = 1.f / nSamples;
	for (int i = 0; i < nSamples; ++i)
	{
		for (int j = 0; i < nDim; ++j)
		{
			float sj = (i + rng.NextFloat()) * invNSamples;
			samples[nDim * i + j] = std::min(sj, FloatOneMinusEpsilon);
		}
	}

	for (int i = 0; i < nDim; ++i)
	{
		for (int j = 0; j < nSamples; ++j)
		{
			int other = j + rng.NextUint32(nSamples - j);
			std::swap(samples[nDim * j + i], samples[nDim * other + i]);
		}
	}
}

Vector2f RejectionSampleDisk(RNG& rng)
{
	Vector2f p;
	do {
		p.X = 1 - 2 * rng.NextFloat();
		p.Y = 1 - 2 * rng.NextFloat();
	} while (p.X * p.X + p.Y * p.Y > 1.f);
	return p;
}

Vector3f UniformSampleHemisphere(const Vector2f& u)
{
	float z = u[0];
	float r = std::sqrt(std::max(0.f,1.0f - z*z));
	float phi = 2 * PI * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

float UniformHemispherePdf()
{
	return 0.5f / PI;
}

Vector3f UniformSampleSphere(const Vector2f& u)
{
	float z = 1.0f - 2.0f * u[0];
	float r = std::sqrt(std::max(0.f,1.0f - z*z));
	float phi = 2.f * PI * u[1];
	return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

float UniformSpherePdf()
{
	return 0.25f / PI;
}

Vector3f UniformSampleCone(const Vector2f& u, float cosThetaMax)
{
	float cosTheta = Math::Lerp(u[0], cosThetaMax, 1.0f);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	float phi = u[1] * 2.f * PI;
	return Vector3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
}

Vector3f UniformSampleCone(const Vector2f& u, float cosThetaMax, const Vector3f& x, const Vector3f& y, const Vector3f& z)
{
	float cosTheta = Math::Lerp(u[0], cosThetaMax, 1.0f);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	float phi = u[1] * 2.f * PI;
	return std::cos(phi) * sinTheta * x +  std::sin(phi) * sinTheta * y + cosTheta * z;
}

float UniformConePdf(float cosThetaMax)
{
	return 1.f / (2.f * PI * (1.f - cosThetaMax));
}

Vector2f UniformSampleDisk(const Vector2f& u)
{
	float r = std::sqrt(u[0]);
	float theta = 2.0f * PI * u[1];
	return Vector2f(r * std::cos(theta), r * std::sin(theta));
}

Vector2f ConcentricSampleDisk(const Vector2f& u)
{
	Vector2f uOffset = 2.f * u - Vector2f(1.0f, 1.0f);

	if (uOffset.X == 0 && uOffset.Y == 0) return Vector2f(0, 0);

	float theta, r;
	if (std::abs(uOffset.X) > std::abs(uOffset.Y))
	{
		r = uOffset.X;
		theta = PI_4 * (uOffset.Y / uOffset.X);
	}
	else
	{
		r = uOffset.Y;
		theta = PI_2 - PI_4 * (uOffset.X / uOffset.Y);
	}
	return r * Vector2f(std::cos(theta), std::sin(theta));
}

Vector2f UniformSampleTriangle(const Vector2f& u)
{
	float su0 = std::sqrt(u[0]);
	return Vector2f(1 - su0, u[1] * su0);
}

Distribution2D::Distribution2D(const float* data, int nu, int nv)
{
	pConditionalV.reserve(nv);
	for (int v = 0; v < nv; ++v)
		pConditionalV.emplace_back(new Distribution1D(&data[v * nu], nu));

	std::vector<float> marginalFunc;
	for (int v = 0; v < nv; ++v)
		marginalFunc.push_back(pConditionalV[v]->funcIntegral);
	pMarginal.reset(new Distribution1D(&marginalFunc[0],nv));
}
