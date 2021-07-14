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
	BxDF(BxDFType type) : type(type) {}
	bool MatchFlags(BxDFType t) const { return (type & t) == type; }
	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf, BxDFType* sampledType = nullptr) const;
	virtual LinearColor rho(const Vector3f& wo, int nSamples, const Vector2f* Smaples) const;
	virtual LinearColor rho(int nSamples, const Vector2f* Smaples1, const Vector2f* Smaples2) const;
	virtual float Pdf(const Vector3f& wo, const Vector3f& wi) const;

	const BxDFType type;
};

class SpecularReflection : public BxDF
{
public:
	SpecularReflection(const LinearColor& R, Fresnel* fresnel) :BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R), fresnel(fresnel) {}
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

class LambertianReflection : public BxDF
{
public:
	LambertianReflection(const LinearColor& InR) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(InR) {}
	LinearColor f(const Vector3f& wo, const Vector3f& wi) const { return R * INV_PI; }
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
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY))
		, R(R)
		, distribution(distribution)
		, fresnel(fresnel) {}
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
	BSDF(const SurfaceInteraction& si, float eta = 1) 
		: eta(eta)
		, ns(si.shading.n)
		, ng(si.n)
		, ss(Normalize(si.shading.dpdu))//取ss为垂直于ns的dpdu
		, ts(Cross(ns,ss))
	{}

	void Add(BxDF* b)
	{
		bxdfs[nBxDFs++] = b;
	}

	int NumComponents(BxDFType flags = BSDF_ALL) const;
	Vector3f WorldToLocal(const Vector3f& v) const
	{
		return Vector3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
	}
	Vector3f LocalToWorld(const Vector3f& v) const
	{
		return Vector3f(ss.X * v.X + ts.X * v.Y + ns.X * v.Z,
						ss.Y * v.X + ts.Y * v.Y + ns.Y * v.Z,
						ss.Z * v.X + ts.Z * v.Y + ns.Z * v.Z);
	}
	LinearColor f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags = BSDF_ALL) const;
	LinearColor rho(int nSamples, const Vector2f* samples1, const Vector2f* samples2, BxDFType flags = BSDF_ALL) const;
	LinearColor rho(const Vector3f& wo, int nSamples, const Vector2f* samples, BxDFType flags = BSDF_ALL) const;
	LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& u, float* pdf, BxDFType type = BSDF_ALL, BxDFType* sampledType = nullptr) const;
	float Pdf(const Vector3f& wo, const Vector3f& wi, BxDFType flags = BSDF_ALL) const;
	const float eta;
private:
	~BSDF() {}

	const Vector3f ns, ng;//shading normal, geometry normal
	const Vector3f ss, ts;//ss,ts,ns 构成本地坐标系

	int nBxDFs = 0;
	static constexpr int MaxBxDFs = 8;
	BxDF* bxdfs[MaxBxDFs];
};