#include "Microfacet.h"

float MicrofacetDistribution::Pdf(const Vector3f& wo, const Vector3f& wh) const
{
	return D(wh) * AbsCosTheta(wh);
}

float BeckmannDistribution::D(const Vector3f& wh) const
{
	float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.f;
	float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	float alpha2 = alpha * alpha;
	return std::exp(-tan2Theta * alpha2) / (PI * alpha2 * cos4Theta);
}

float BeckmannDistribution::G1(const Vector3f& w, const Vector3f& wh) const
{
	if (Dot(w, wh) / CosTheta(w) < 0.f) return 0.f;
	float tanTheta = std::abs(TanTheta(w));
	float a = 1.f / (tanTheta * alpha);
	if (a < 1.0f) return (3.535f * a + 2.0181f * a * a) / (1.f + 2.276f * a + 2.577 * a * a);
	return 1.f;
}

Vector3f BeckmannDistribution::Sample_wh(const Vector3f& wo, const Vector2f& u) const
{
	float phi = 2.f * PI * u[0];
	float cosPhi = std::cos(phi);
	float sinPhi = std::sin(phi);
	float cosTheta = 1.f / (1.f - alpha * alpha * std::log(1 - u[1]));
	float sinTheta = std::sqrt(1.f - cosTheta * cosTheta);
	return Vector3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
}

float TrowbridgeReitzDistribution::D(const Vector3f& wh) const
{
	float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.f;
	float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	float alpha2 = alpha * alpha;
	return alpha2 / (PI * cos4Theta * (alpha2 + tan2Theta) * (alpha2 + tan2Theta));
}

float TrowbridgeReitzDistribution::G1(const Vector3f& w, const Vector3f& wh) const
{
	if (Dot(w, wh) / CosTheta(w) < 0.f) return 0.f;
	float tan2Theta = Tan2Theta(w);
	return 2.f / (1.f + std::sqrt(1.f + alpha * alpha * tan2Theta));
}

Vector3f TrowbridgeReitzDistribution::Sample_wh(const Vector3f& wo, const Vector2f& u) const
{
	float phi = 2.f * PI * u[0];
	float cosPhi = std::cos(phi);
	float sinPhi = std::sin(phi);
	float cosTheta = (1.f - u[1]) / ((alpha * alpha - 1) * u[1] + 1.f);
	float sinTheta = std::sqrt(1.f - cosTheta * cosTheta);
	return Vector3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
}
