#pragma once

#include "Vector.h"
#include "Colorimetry.h"
#include "Fresnel.h"

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
		*wi = Vector3f(-wo.X,-wo.Y,wo.Z);
		*pdf = 1.f;

	}
	float Pdf(const Vector3f& wo, const Vector3f& wi) { return 0.f; }
private:
	const LinearColor R;
	const Fresnel* fresnel;
};

class LambertainReflection : public BxDF
{
public:
	LambertainReflection(const LinearColor& InR) : R(InR) {}
	LinearColor f(const Vector3f& wo, const Vector3f& wi) const;
	LinearColor rho(const Vector3f& wo, int nSamples, const Vector2f* Smaples) const { return R; }
	LinearColor rho(int nSamples, const Vector2f* Smaples1, const Vector2f* Smaples2) const { return R; }
private:
	const LinearColor R;
};

class FresnelSpecular : public BxDF
{

};

class MicrofacetReflection : public BxDF
{

};

class BSDF
{

};