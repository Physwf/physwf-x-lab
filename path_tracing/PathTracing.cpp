#include "PathTracing.h"
#include "Light.h"
#include "RayTracing.h"
#include "SurfaceScattering.h"

PathIntergrator::PathIntergrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler, float rrThreshold)
	: SamplerIntegrator(camera,sampler)
	, maxDepth(maxDepth)
	, rrThreshhold(rrThreshold)
{
}

void PathIntergrator::Preprocess(const Scene& scene, Sampler& sampler)
{
	lightDistribution = CreateLightDistribution("uniform", scene);
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
			else
			{
				for (const auto& light : scene.infiniteLights)
				{
					L += beta * light->Le(ray);
				}
			}
		}


		if (!foundIntersection || bounces >= maxDepth) break;

		isect.ComputeScatteringFunctions(ray, arena);
		if (!isect.bsdf)
		{
			ray = isect.SpawnRay(ray.d);
			bounces--;
			continue;
		}
		const Distribution1D* distrib = lightDistribution->Lookup(isect.p);

		if (isect.bsdf->NumComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0)
		{
			LinearColor Ld = beta * UniformSampleOneLight(isect, scene, arena, sampler, distrib);
			if (std::isnan(Ld[0]) || std::isnan(Ld[1]) || std::isnan(Ld[2]))
			{
				L += 0;
			}
			else
			{
				L += Ld;
			}
		}
		///*if(bounces == 1)*/ break;
		Vector3f wo = -ray.d, wi;
		float pdf;
		BxDFType flags;
		LinearColor f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, BSDF_ALL, &flags);
		if (f.IsBlack() || pdf == 0) break;
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
	//assert(!std::isnan(L[0]));
	return L;
}

VolPathIntegrator::VolPathIntegrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler, float rrThreshold)
	: SamplerIntegrator(camera, sampler)
	, maxDepth(maxDepth)
	, rrThreshhold(rrThreshold)
{
}

void VolPathIntegrator::Preprocess(const Scene& scene, Sampler& sampler)
{

}

LinearColor VolPathIntegrator::Li(const Ray & r, const Scene & scene, Sampler & sampler, MemoryArena & arena, int depth)
{
	LinearColor L(0.f), beta(1.0f);
	Ray ray(r);
	bool specularBounce = false;
	for (int bounce = 0;; ++bounce)
	{
		SurfaceInteraction isect;
		bool foundIntersection = scene.Intersect(ray, &isect);
		MediumInteraction mi;
		if (ray.medium)
			beta *= ray.medium->Sample(ray, sampler, arena, &mi);
		if(beta.IsBlack())
			break;
		if (mi.IsValid())
		{
			L += beta * UniformSampleOneLight(mi, scene, arena, sampler,NULL, true);
			Vector3f wo = -r.d, wi;
			mi.phase->Sample_p(wo, &wi, sampler.Get2D());
			ray = mi.SpawnRay(wi);
		}
		else
		{
			
		}
	}
}
