#pragma once

#include "Vector.h"
#include "Colorimetry.h"
#include "Fresnel.h"
#include "Vector.h"
#include "Microfacet.h"
#include "RayTracing.h"

enum BxDFType
{
	BSDF_REFLECTION		= 1 << 0,
	BSDF_TRANSMISSION	= 1 << 1,
	BSDF_DIFFUSE		= 1 << 2,
	BSDF_GLOSSY			= 1 << 3,
	BSDF_SPECULAR		= 1 << 4,
	BSDF_ALL			= BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

class BxDF
{
public:
	virtual ~BxDF() {}

	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf, BxDFType* sampledType = nullptr) const;
	virtual LinearColor rho(const Vector3f& wo, int nSamples, const Vector2f* Smaples) const;
	virtual LinearColor rho(int nSamples, const Vector2f* Smaples1, const Vector2f* Smaples2) const;
	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const;
};

class SpecularReflection : public BxDF
{
public:
	SpecularReflection(const LinearColor& R, Fresnel* fresnel);
	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const override
	{
		return LinearColor(0.f);
	}
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf, BxDFType* sampledType) const override
	{
		*wi = Vector3f(-wo.X,-wo.Y,wo.Z);
		*pdf = 1.f;
		return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
	}
	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0.f; }
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
public:
	MicrofacetReflection(const LinearColor& R, const MicrofacetDistribution* distribution, Fresnel* fresnel)
		: R(R), distribution(distribution), fresnel(fresnel) {}
	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const override;
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf, BxDFType* sampledType) const override;
	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const override;
private:
	const LinearColor R;
	const MicrofacetDistribution* distribution;
	const Fresnel* fresnel;
};

class BSDF
{
public:
	BSDF(const SurfaceInteraction& si, float eta = 1) : eta(eta)
	{}

	void Add(BxDF* b)
	{
		bxdfs[nBxDFs++] = b;
	}
	const float eta;
private:
	~BSDF() {}

	int nBxDFs = 0;
	static constexpr int MaxBxDFs = 8;
	BxDF* bxdfs[MaxBxDFs];
};