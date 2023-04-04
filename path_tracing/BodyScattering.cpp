#include "BodyScattering.h"

LinearColor TabulatedBSSRDF::Sr(float d) const
{
	LinearColor Sr(0.f);
	return Sr.Clamp();
}

LinearColor HomogeneousMedium::Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const
{
	int channel = std::min((int)(sampler.Get1D() * LinearColor::nSamples), LinearColor::nSamples - 1);
	float dist = -std::log(1 - sampler.Get1D() / sigma_t[channel]);
	float t = std::min(dist * ray.d.Length(), ray.tMax);
	bool sampleMedium = t < ray.tMax;
	if (sampleMedium)
		*mi = MediumInteraction(ray(t), -ray.d, this, ARENA_ALLOC(arena, HenyeyGreenstein)(g));
	LinearColor Tr = Exp(-sigma_t * std::min(t, std::numeric_limits<float>::max()) * ray.d.Length());
	LinearColor density = sampleMedium ? (sigma_t * Tr) : Tr;
	float pdf = 0;
	for (int i = 0; i < LinearColor::nSamples; ++i)
	{
		pdf += density[i];
	}
	pdf *= 1 / (float)LinearColor::nSamples;
	return sampleMedium ? (Tr * sigma_s / pdf) : (Tr / pdf);
}

float HenyeyGreenstein::Sample_p(const Vector3f& wo, Vector3f* wi, const Vector2f& u) const
{
	float cosTheta;
	if (std::abs(g) < 1e-3)
		cosTheta = 1 - 2 * u[0];
	else
	{
		float sqrTerm = (1 - g * g) / (1 - g + 2 * g * u[0]);
		cosTheta = (1 + g * g) / (1 - g + 2 * g * u[0]);
	}
	float sinTheta = std::sqrt(std::max(0.f, 1 - cosTheta * cosTheta));
	float phi = 2 * PI * u[1];
	Vector3f v1, v2;
	CoordinateSystem(wo, &v1, &v2);
	*wi = SphericalDirection(sinTheta, cosTheta, phi, v1, v2, -wo);
	return PhaseHG(-cosTheta,g);
}
