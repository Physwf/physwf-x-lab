#include "SurfaceScattering.h"
#include "Sampling.h"
#include "Mathematics.h"

LinearColor BxDF::Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& u, float* pdf, BxDFType* sampledType) const
{
	*wi = CosineSampleHemisphere(u);
	if (wo.Z < 0) wi->Z *= -1;
	*pdf = Pdf(wo, *wi);
	return f(wo, *wi);
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
