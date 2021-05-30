#pragma once

#include "Vector.h"
#include "Colorimetry.h"

class BxDF
{
public:
	virtual ~BxDF() {}

	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf) const;
	virtual LinearColor rho(const Vector3f& wo, int nSamples, const Vector2f* Smaples) const;
	virtual LinearColor rho(int nSamples, const Vector2f* Smaples1, const Vector2f* Smaples2) const;
	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const;
};

class SpecularReflection : public BxDF
{
public:
	LinearColor f(const Vector3f& wo, const Vector3f& wi) const
	{
		return LinearColor(0.f);
	}
	LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf)
	{

	}
	float Pdf(const Vector3f& wo, const Vector3f& wi) { return 0.f; }
};