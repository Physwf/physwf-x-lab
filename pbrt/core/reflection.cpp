#include "reflection.h"
#include "spectrum.h"
#include "sampler.h"
#include "sampling.h"
//#include "interpolation.h"
#include "scene.h"
#include "interaction.h"
//#include "stats.h"
#include <stdarg.h>

int BSDF::NumComponents(BxDFType flags /*= BSDF_ALL*/) const
{
	int num = 0;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchFlags(flags)) ++num;
	return num;
}

Spectrum BSDF::f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags) const
{
	Vector3f wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
	bool reflect = Dot(wiW, ng) * Dot(woW, ng) > 0;
	Spectrum f(0.f);
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags) &&
			((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
			(!reflect && (bxdfs[i]->type && BSDF_TRANSMISSION))))
		{
			f += bxdfs[i]->f(wo, wi);
		}
	}
	return f;
}

Spectrum BSDF::rho(int nSamples, const Point2f* smaple1, const Point2f* smaple2, BxDFType flags /*= BSDF_ALL*/) const
{
	Spectrum ret(0.0f);
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchFlags(flags))
			ret += bxdfs[i]->rho(nSamples, smaple1, smaple2);
	return ret;
}

Spectrum BSDF::rho(const Vector3f& woWorld, int nSamples, const Point2f* samples, BxDFType flags /*= BSDF_ALL*/) const
{
	Vector3f wo = WorldToLocal(woWorld);
	Spectrum ret(0.f);
	for (int i = 0; i < nBxDFs; ++i)
		ret += bxdfs[i]->rho(wo, nSamples, samples);
	return ret;
}

Spectrum BSDF::Sample_f(const Vector3f& woW, Vector3f* wiW, const Point2f& u, Float* pdf, BxDFType type, BxDFType *sampledType) const
{
	int matchingComps = NumComponents(type);
	if (matchingComps == 0)
	{
		*pdf = 0;
		return Spectrum(0);
	}
	int comp = std::min((int)std::floor(u[0] * matchingComps), matchingComps - 1);

	BxDF* bxdf = nullptr;
	int count = comp;
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(type) && count-- == 0)
		{
			bxdf = bxdfs[i];
			break;
		}
	}

	//Point2f uRemapped(u[0] * matchingComps - comp, u[1]);
	Point2f uRemapped(std::min(u[0] * matchingComps - comp, OneMinusEpsilon), u[1]);

	Vector3f wi, wo = WorldToLocal(woW);
	if (wo.z == 0) return 0.;
	*pdf = 0;
	if (sampledType) *sampledType = bxdf->type;
	Spectrum f = bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);
	if (*pdf == 0)
		return 0;
	*wiW = LocalToWorld(wi);

	if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
	{
		for (int i = 0; i < nBxDFs; ++i)
		{
			if (bxdfs[i] != bxdf && bxdfs[i]->MatchFlags(type))
			{
				*pdf = bxdfs[i]->Pdf(wo, wi);
			}
		}
	}
	if (matchingComps > 1) *pdf /= matchingComps;

	if (!(bxdf->type & BSDF_SPECULAR))
	{
		bool reflect = Dot(*wiW, ng)*Dot(woW, ng) > 0 ;
		f = 0.f;
		for (int i = 0; i < nBxDFs; ++i)
		{
			if (bxdfs[i]->MatchFlags(type) && 
				((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) || (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
			{
				f += bxdfs[i]->f(wo, wi);
			}
		}
	}
	return f;
}

Float BSDF::Pdf(const Vector3f& woW, const Vector3f& wiW, BxDFType flags /*= BSDF_ALL*/) const
{
	if (nBxDFs == 0.f) return 0.f;
	Vector3f wo = WorldToLocal(woW), wi = WorldToLocal(wiW);
	if (wo.z == 0) return 0.;
	Float pdf = 0.f;
	int matchingComps = 0;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchFlags(flags)) {
			++matchingComps;
			pdf += bxdfs[i]->Pdf(wo, wi);
		}
	Float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
	return v;
}

//默认实现采用了带有cos权重的单位半球采样
Spectrum BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u, Float * pdf, BxDFType *sampleType /*= nullptr*/) const
{
	*wi = CosineSampleHemisphere(u);
	if (wo.z < 0) wi->z *= -1;// If ωo is in the opposite hemisphere, then ωi must be flipped to lie in the same hemisphere as ωo.
	*pdf = Pdf(wo, *wi);
	return f(wo, *wi);
}

Spectrum BxDF::rho(const Vector3f &wo, int nSamples, const Point2f *samples) const
{
	Spectrum r(0.);
	for (int i = 0; i < nSamples; ++i)
	{
		Vector3f wi;
		Float pdf = 0;
		Spectrum f = Sample_f(wo, &wi, samples[i], &pdf);
		if (pdf > 0) r += f * AbsCosTheta(wi) / pdf;
	}
	return r / nSamples;
}

Spectrum BxDF::rho(int nSamples, const Point2f *smaples1, const Point2f *samples2) const
{
	Spectrum r(0.);
	for (int i = 0; i < nSamples; ++i)
	{
		Vector3f wo, wi;
		wo = UniformSamplerHemisphere(smaples1[i]);
		Float pdfo = UniformHemispherePdf(), pdfi = 0;
		Spectrum f = Sample_f(wo, &wi, samples2[i], &pdfi);
		if (pdfi > 0)
			r += f * AbsCosTheta(wi) * AbsCosTheta(wo) / (pdfo * pdfi);
	}
	return r / (Pi * nSamples);
}

Float BxDF::Pdf(const Vector3f &wo, const Vector3f& wi) const
{
	//如果wo,wi不在同一侧则返回0
	return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT)
{
	cosThetaI = Clamp(cosThetaI, -1, 1);
	//如果入射角同发现不在一侧，则反转对应参数
	bool entering = cosThetaI > 0.f;
	if (!entering)
	{
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}
	Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;
	if (sinThetaT >= 1) return 1;//全反射
	Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));
	Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	Float Rperp = ((etaI * cosThetaT) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl*Rparl + Rperp * Rperp) / 2;
}

Spectrum FrConductor(Float cosThetaI, const Spectrum &etaI, const Spectrum& etaT, const Spectrum &k)
{
	cosThetaI = Clamp(cosThetaI, -1, 1);
	Spectrum eta = etaT / etaI;
	Spectrum etak = k / etaI;

	Float cosThetaI2 = cosThetaI * cosThetaI;
	Float sinThetaI2 = 1. - cosThetaI2;
	Spectrum eta2 = eta * eta;
	Spectrum etak2 = etak * etak;
	
	Spectrum t0 = eta2 - etak2 - sinThetaI2;
	Spectrum a2plusb2 = Sqrt(t0*t0 + 4 * eta2*eta2);
	Spectrum t1 = a2plusb2 + cosThetaI2;
	Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
	Spectrum t2 = (Float)2 * cosThetaI * a;
	Spectrum Rs = (t1 - t2) / (t1 + t2);

	Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	Spectrum t4 = t2 * sinThetaI2;
	Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}



Spectrum FresnelConductor::Evaluate(Float cosThetaI) const
{
	return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
}

Spectrum FresnelDielectric::Evaluate(Float cosThetaI) const
{
	return FrDielectric(cosThetaI, etaI, etaT);
}

Spectrum SpecularReflection::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	*wi = Vector3f(-wo.x, -wo.y, wo.z);
	*pdf = 1;
	return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

Spectrum SpecularTransmission::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	bool entering = CosTheta(wo) > 0;
	Float etaI = entering ? etaA : etaB;
	Float etaT = entering ? etaB : etaA;
	if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
	{
		return 0;
	}
	*pdf = 1;
	Spectrum ft = T * Spectrum(1.) - fresnel.Evaluate(CosTheta(*wi));
	if (mode == TransportMode::Radiance)
	{
		ft *= (etaI * etaI) / (etaT*etaT);
	}
	return ft / AbsCosTheta(*wi);
}

Spectrum FresnelSpecular::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	Float F = FrDielectric(CosTheta(wo), etaA, etaB);
	if (sample[0] < F)
	{
		if (sampleType)
			*sampleType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
		*pdf = F;
		return F * R / AbsCosTheta(*wi);
	}
	else
	{
		bool entering = CosTheta(wo) > 0;
		Float etaI = entering ? etaA : etaB;
		Float etaT = entering ? etaB : etaA;

		if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
			return 0;
		Spectrum ft = T * (1 - F);
		if (mode == TransportMode::Radiance)
			ft *= (etaI*etaI) / etaT * etaT;
		if (sampleType)
			*sampleType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
		*pdf = 1 - F;
		return ft / AbsCosTheta(*wi);
	}
}

Spectrum LambertianTransmission::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	*wi = CosineSampleHemisphere(sample);
	if (wo.z > 0) wi->z *= -1;
	*pdf = Pdf(wo, *wi);
	return f(wo,*wi);
}

Float LambertianTransmission::Pdf(const Vector3f &wo, const Vector3f& wi) const
{
	return !SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}

Spectrum OrenNayar::f(const Vector3f& wo, const Vector3f& wi) const
{
	Float sinThetaI = SinTheta(wi);
	Float sinThetaO = SinTheta(wo);
	// Compute cosine term of Oren-Nayar model
	Float maxCos = 0;
	if (sinThetaI > 1e-4 && sinThetaO > 1e-4) {
		Float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
		Float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
		Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
		maxCos = std::max((Float)0, dCos);
	}

	// Compute sine and tangent terms of Oren-Nayar model
	Float sinAlpha, tanBeta;
	if (AbsCosTheta(wi) > AbsCosTheta(wo)) {
		sinAlpha = sinThetaO;
		tanBeta = sinThetaI / AbsCosTheta(wi);
	}
	else {
		sinAlpha = sinThetaI;
		tanBeta = sinThetaO / AbsCosTheta(wo);
	}
	return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
}
