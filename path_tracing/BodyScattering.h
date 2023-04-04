#pragma once

#include "Medium.h"
#include "Material.h"
#include "Fresnel.h"

#include <memory>

class HenyeyGreenstein : public PhaseFunction
{
public:
	HenyeyGreenstein(float g):g(g) {}

	virtual float p(const Vector3f& wo, const Vector3f& wi) const 
	{
		return PhaseHG(Dot(wo, wi), g);
	}
	virtual float Sample_p(const Vector3f& wo, Vector3f* wi, const Vector2f& u) const;
private:
	float g;
};

class HomogeneousMedium : public Medium
{
public:
	HomogeneousMedium(const LinearColor& sigma_a, const LinearColor& sigma_s, float g):sigma_a(sigma_a), sigma_s(sigma_s), sigma_t(sigma_a + sigma_s), g(g) { }
	virtual LinearColor Tr(const Ray& ray, Sampler& sampler) const override
	{
		return Exp(-sigma_t * std::min(ray.tMax *ray.d.Length(),std::numeric_limits<float>::max()));
	}
	virtual LinearColor Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const;
private:
	const LinearColor sigma_a, sigma_s, sigma_t;
	const float g;
};

class BSSRDF
{
public:
	BSSRDF(const SurfaceInteraction& po, float eta) : po(po), eta(eta) {}
	virtual LinearColor S(const SurfaceInteraction& pi, const Vector3f& wi) = 0;
protected:
	const SurfaceInteraction& po;
	float eta;
};

float FresnelMoment1(float invEta);
float FresnelMoment2(float invEta);

class SeparableBSSRDF : public BSSRDF
{
public:
	SeparableBSSRDF(const SurfaceInteraction& po, float eta, const Material* material) 
		: BSSRDF(po,eta),
		ns(po.shading.n),
		ss(Normalize(po.shading.dpdu)),
		ts(Cross(ns,ss)),
		material(material) 
	{}

	virtual LinearColor S(const SurfaceInteraction& pi, const Vector3f& wi) override
	{
		float Ft = 1 - FrDielectirc(Dot(po.wo, po.shading.n), 1, eta);
		return Ft *  Sp(pi) * Sw(wi);
	}

	LinearColor Sw(const Vector3f& w) const
	{
		float c = 1 - FresnelMoment1(1 / eta);
		return (1 - FrDielectirc(CosTheta(w),1,eta)) / (c * PI);
	}
	LinearColor Sp(const SurfaceInteraction& pi) const
	{
		return Sr(Distance(po.p, pi.p));
	}
	virtual LinearColor Sr(float d) const = 0;
private:
	const Vector3f ns;
	const Vector3f ss, ts;
	const Material* material;
};

class BSSRDFTable
{
public:
	BSSRDFTable(int nRhoSamples, int nRadiusSamples) 
		: nRhoSamples(nRhoSamples)
		, nRadiusSamples(nRadiusSamples)
		, rhoSamples(new float[nRhoSamples])
		, radiusSamples(new float[nRadiusSamples])
		, profile(new float[nRhoSamples* nRadiusSamples])
		, rhoEff(new float[nRhoSamples]) {}

	const int nRhoSamples, nRadiusSamples;
	std::unique_ptr<float[]> rhoSamples, radiusSamples;
	std::unique_ptr<float[]> profile;
	std::unique_ptr<float[]> rhoEff;
};

class TabulatedBSSRDF : public SeparableBSSRDF
{
public:
	TabulatedBSSRDF(const SurfaceInteraction& po, const Material* material, float eta, const LinearColor& sigma_a, const LinearColor& sigma_s, const BSSRDFTable& table)
		: SeparableBSSRDF(po,eta,material),table(table)
	{
		sigma_t = sigma_a + sigma_s;
		for (int c = 0; c < LinearColor::nSamples; ++c)
			rho[c] = sigma_t[c] != 0 ? (sigma_s[c] / sigma_t[c]) : 0;
	}

	virtual LinearColor Sr(float d) const override;
private:
	const BSSRDFTable& table;
	LinearColor sigma_t, rho;
};