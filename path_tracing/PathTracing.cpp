#include "PathTracing.h"
#include "Light.h"

PathIntergrator::PathIntergrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler, float rrThreshold)
	: SamplerIntegrator(camera,sampler)
	, maxDepth(maxDepth)
	, rrThreshhold(rrThreshold)
{
}

LinearColor PathIntergrator::Li(const Ray& r, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */)
{
	LinearColor L(0.f), beta(1.f);
	Ray ray(r);
	bool specluarBounce = false;
	int bounces;

	float etaScale = 1;

	for (bounces = 0;; ++bounces)
	{
		SurfaceInteraction isect;
		bool foundIntersection = scene.Intersect(ray, &isect);

		if (bounces == 0 || specluarBounce)
		{
			if (foundIntersection)
			{
				L += beta * isect.Le(-ray.d);
			}
		}
		else
		{
			for (const auto& light : scene.infiniteLights)
			{
				L += beta * light->Le(ray);
			}
		}

		if(!foundIntersection || bounces >= maxDepth) break;

		isect.ComputeScatteringFunctions(ray, arena);

		const Distribution1D* distrib = lightDistribution->Lookup(isect.p);

		if (isect.bsdf->NumComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0)
		{
			LinearColor Ld = beta * UniformSampleOneLight(isect, scene, arena, sampler, distrib);
			L += Ld;
		}

		Vector3f wo = -ray.d, wi;
		float pdf;
		BxDFType flags;
		LinearColor f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, BSDF_ALL, &flags);
		beta *= f * AbsDot(wi,isect.shading.n) / pdf;
		specluarBounce = (flags & BSDF_SPECULAR) != 0;
		if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION))
		{
			float eta = isect.bsdf->eta;
			etaScale *= (Dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
		}
		ray = isect.SpawnRay(wi);

		//todo: subsurface scattering

		LinearColor rrBeta = beta * etaScale;
		if (rrBeta.MaxComponentValue() < rrThreshhold && bounces > 3)
		{
			float q = std::max(0.05f, 1.f - rrBeta.MaxComponentValue());
			if(sampler.Get1D() < q) break;
			beta /= 1.f - q;
		}
	}

	return L;
}
