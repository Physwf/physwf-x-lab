#include "Microfacet.h"

float MicrofacetDistribution::Pdf(const Vector3f& wo, const Vector3f& wh) const
{
	//return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
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

float BeckmannDistribution::Lambda(const Vector3f& w) const
{
	return 0;
}

float TrowbridgeReitzDistribution::D(const Vector3f& wh) const
{
// 	float tan2Theta = Tan2Theta(wh);
// 	if (std::isinf(tan2Theta)) return 0.f;
// 	float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
// 	float alpha2 = alpha * alpha;
// 	float temp = (alpha2 - 1) * Cos2Theta(wh) + 1;
// 	//return alpha2 * CosTheta(wh) / (PI * temp * temp);
// 	return alpha2 / (PI * cos4Theta * (alpha2 + tan2Theta) * (alpha2 + tan2Theta));

	float alpha2 = alpha * alpha;
	float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.;
	const float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	float e = (1 / alpha2) * tan2Theta;
	return 1 / (PI * alpha2 * cos4Theta * (1 + e) * (1 + e));
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
	float cosTheta = std::sqrt((1.f - u[1]) / (u[1] * (alpha * alpha - 1)  + 1.f));
	float sinTheta = std::sqrt(1.f - cosTheta * cosTheta);
	return Vector3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
}

float TrowbridgeReitzDistribution::Lambda(const Vector3f& w) const
{
	float absTanTheta = std::abs(TanTheta(w));
	if (std::isinf(absTanTheta)) return 0.f;

	float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);

	return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}

float BeckmannDistribution2::D(const Vector3f& wh) const
{
	if (CosTheta(wh) <= 0) return 0;
	float TanTheta2 = Tan2Theta(wh);
	float alpha2 = alpha * alpha;
	float CosTheta2 = Cos2Theta(wh);
	float Value = std::exp(-TanTheta2 / alpha2) / (PI * alpha2 * CosTheta2 * CosTheta2);
	assert(!std::isinf(Value)); 
	return Value;
}

float BeckmannDistribution2::G(const Vector3f& wo, const Vector3f& wi) const
{
	Vector3f wh = Normalize(wo + wh);
	float Value = G1(wo, wh) * G1(wi, wh);
	assert(!std::isinf(Value));
	return Value;
}

float BeckmannDistribution2::G1(const Vector3f& w, const Vector3f& wh) const
{
	//if ((Dot(w, wh) / CosTheta(w)) <= 0) return 0;
	float a = 1 / (alpha * TanTheta(w));
	if (a < 1.6f)
	{
		assert(!std::isinf(a));
		float a2 = a * a;
		return (3.535f * a + 2.181f * a2) / (1 + 2.276f * a + 2.557f * a2);
	}
	return 1.0f;
}

Vector3f BeckmannDistribution2::Sample_wh(const Vector3f& wo, const Vector2f& u) const
{
	float phi = 2.f * PI * u[1];
	float theta = std::atan(-alpha * alpha * std::log(1.f - u[0]));
	float sinPhi = std::sin(phi);
	float cosPhi = std::cos(phi);
	float sinTheta = std::sin(theta);
	float cosTheta = std::cos(theta);
	return Vector3f(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
}

float BeckmannDistribution2::Lambda(const Vector3f& w) const
{
	return 0;
}
