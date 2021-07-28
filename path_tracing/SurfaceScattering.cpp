#include "SurfaceScattering.h"
#include "Sampling.h"
#include "Mathematics.h"

LinearColor BxDF::Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& u, float* pdf, BxDFType* sampledType) const
{
	*wi = CosineSampleHemisphere(u);
	if (wo.Z < 0) wi->Z *= -1;
	*pdf = Pdf(wo, *wi);
// 	*pdf = INV_PI;
// 	*wi = Normalize(Vector3f((float)rand()/RAND_MAX - 0.5f, (float)rand() / RAND_MAX - 0.5f, (float)rand() / RAND_MAX));
	return f(wo, *wi);
}

LinearColor BxDF::rho(const Vector3f& wo, int nSamples, const Vector2f* u) const
{
	LinearColor r(0.f);
	for (int i = 0; i < nSamples; ++i)
	{
		Vector3f wi;
		float pdf = 0;
		LinearColor f = Sample_f(wo, &wi, u[i], &pdf);
		if (pdf > 0) r += f * AbsCosTheta(wi) / pdf;
	}
	return r / nSamples;
}

LinearColor BxDF::rho(int nSamples, const Vector2f* u1, const Vector2f* u2) const
{
	LinearColor r(0.f);
	for (int i = 0; i < nSamples; ++i)
	{
		Vector3f wo, wi;
		wo = UniformSampleHemisphere(u1[i]);
		float pdfo = UniformHemispherePdf(), pdfi = 0;
		LinearColor f = Sample_f(wo, &wi, u2[i], &pdfi);
		if (pdfi > 0)
			r += f * AbsCosTheta(wi) * AbsCosTheta(wo) / (pdfo * pdfi);
	}
	return r / (PI * nSamples);
}

float BxDF::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * INV_PI : 0;
}

LinearColor MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi) const
{
	float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	Vector3f wh = wi + wo;
	if (cosThetaI == 0 || cosThetaO == 0) return LinearColor(0.f);
	if (wh.X == 0 && wh.Y == 0 && wh.Z == 0) return LinearColor(0.f);
	wh = Normalize(wh);
	LinearColor F = fresnel->Evaluate(Dot(wi,wh));
	return R * distribution->D(wh) * distribution->G(wo, wi, wh) * F / (4.f * cosThetaI * cosThetaO);
}

LinearColor MicrofacetReflection::Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& u, float* pdf, BxDFType* sampledType) const
{
	if (wo.Z == 0) return 0.f;
	Vector3f wh = distribution->Sample_wh(wo, u);
	if (Dot(wo, wh) < 0) return 0.f;
	*wi = Reflect(wo, wh);
	if (!SameHemisphere(wo, *wi)) return LinearColor(0);
	*pdf = distribution->Pdf(wo, wh) / 4 * Dot(wo, wh);
	return f(wo, *wi);
}

float MicrofacetReflection::Pdf(const Vector3f& wo, const Vector3f& wi) const
{
	if (!SameHemisphere(wo, wi)) return 0;
	Vector3f wh = Normalize(wo + wi);
	return distribution->Pdf(wo, wh) / (4.f * Dot(wo, wh));
}

int BSDF::NumComponents(BxDFType flags /*= BSDF_ALL*/) const
{
	int num = 0;
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags)) ++num;
	}
	return num;
}

LinearColor BSDF::f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags /*= BSDF_ALL*/) const
{
	Vector3f wi = WorldToLocal(wiW);
	Vector3f wo = WorldToLocal(woW);
	if (wo.Z == 0) return LinearColor(0); //??
	bool reflect = Dot(wiW, ng) * Dot(woW, ng);
	LinearColor f(0.f);
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags)
			&& ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
				(!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
		{
			f += bxdfs[i]->f(wo, wi);
		}
	}
	return f;
}

LinearColor BSDF::rho(int nSamples, const Vector2f* samples1, const Vector2f* samples2, BxDFType flags /*= BSDF_ALL*/) const
{
	LinearColor ret(0.f);
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags))
			ret += bxdfs[i]->rho(nSamples, samples1, samples2);
	}
	return ret;
}

LinearColor BSDF::rho(const Vector3f& woW, int nSamples, const Vector2f* samples, BxDFType flags /*= BSDF_ALL*/) const
{
	Vector3f wo = WorldToLocal(woW);
	LinearColor ret(0.f);
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags))
			ret += bxdfs[i]->rho(wo, nSamples, samples);
	}
	return ret;
}

LinearColor BSDF::Sample_f(const Vector3f& woW, Vector3f* wiW, const Vector2f& u, float* pdf, BxDFType type /*= BSDF_ALL*/, BxDFType* sampledType /*= nullptr*/) const
{
	int matchingComps = NumComponents(type);
	if (matchingComps == 0)
	{
		*pdf = 0;
		if (sampledType) *sampledType = BxDFType(0);
		return LinearColor(0.f);
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

	Vector2f uRemapped(std::min((u[0] * matchingComps - comp), FloatOneMinusEpsilon), u[1]);

	Vector3f wi, wo = WorldToLocal(woW);
	if (wo.Z == 0) return 0.f;

	*pdf = 0;
	if (sampledType) *sampledType = bxdf->type;

	LinearColor f = bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);

	if (*pdf == 0)
	{
		if (sampledType) *sampledType = BxDFType(0);
		return 0;
	}

	*wiW = LocalToWorld(wi);

	if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
	{
		for (int i = 0; i < nBxDFs; ++i)
		{
			if (bxdfs[i] != bxdf && bxdfs[i]->MatchFlags(type))
			{
				*pdf += bxdfs[i]->Pdf(wo, wi);
			}
		}
	}
	if (matchingComps > 1) *pdf /= matchingComps;

	if (!(bxdf->type & BSDF_SPECULAR))
	{
		bool reflect = Dot(*wiW, ng) * Dot(woW, ng) > 0;
		f = 0.f;
		for (int i = 0; i < nBxDFs; ++i)
		{
			if (bxdfs[i]->MatchFlags(type) &&
				((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
					(!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
			{
				f += bxdfs[i]->f(wo, wi);
			}
		}
	}
	return f;
}

float BSDF::Pdf(const Vector3f& woW, const Vector3f& wiW, BxDFType flags /*= BSDF_ALL*/) const
{
	if (nBxDFs == 0.f) return 0.f;

	Vector3f wo = WorldToLocal(woW), wi = WorldToLocal(wiW);
	if (wo.Z == 0.f) return 0.f;

	float pdf = .0f;
	int matchingComps = 0;
	for (int i = 0; i < nBxDFs; ++i)
	{
		if (bxdfs[i]->MatchFlags(flags))
		{
			++matchingComps;
			pdf += bxdfs[i]->Pdf(wo, wi);
		}
	}
	float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
	return v;
}

