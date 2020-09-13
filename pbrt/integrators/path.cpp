#include "path.h"
#include "bssrdf.h"
#include "camera.h"
#include "film.h"
#include "interaction.h"
#include "paramset.h"
#include "scene.h"

// PathIntegrator Method Definitions
PathIntegrator::PathIntegrator(int maxDepth,
	std::shared_ptr<const Camera> camera,
	std::shared_ptr<Sampler> sampler,
	const Bounds2i &pixelBounds, Float rrThreshold,
	const std::string &lightSampleStrategy)
	: SamplerIntegrator(camera, sampler, pixelBounds),
	maxDepth(maxDepth),
	rrThreshold(rrThreshold),
	lightSampleStrategy(lightSampleStrategy) {}

void PathIntegrator::Preprocess(const Scene &scene, Sampler &sampler) 
{
	lightDistribution = CreateLightSampleDistribution(lightSampleStrategy, scene);
}


Spectrum PathIntegrator::Li(const RayDifferential& r, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */) const
{
	Spectrum L(0.f), beta(1.0f);
	RayDifferential ray(r);
	bool specularBounce = false;
	for (int bounces = 0;; ++bounces)
	{
		SurfaceInteraction isect;
		bool foundIntersection = scene.Intersect(ray, &isect);
		if (bounces == 0 || specularBounce)
		{
			if (foundIntersection)
			{
				L += beta * isect.Le(-ray.d);
			}
			else
			{
				for (const auto& light : scene.lights)
					L += beta * light->Le(ray);
			}
		}
		if (!foundIntersection || bounces >= maxDepth)
			break;
		isect.ComputeScatteringFunctions(ray, arena, true);
		if (!isect.bsdf)
		{
			ray = isect.SpawnRay(ray.d);
			bounces--;
			continue;
		}
		L += beta * UniformSampleOneLight(isect, scene, arena, sampler);

		Vector3f wo = -ray.d, wi;
		Float pdf;
		BxDFType flags;
		Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, BSDF_ALL, &flags);
		if (f.IsBlack() || pdf == 0.f)
			break;
		beta *= f * AbsDot(wi, isect.shading.n) / pdf;
		specularBounce = (flags & BSDF_SPECULAR) != 0;
		ray = isect.SpawnRay(wi);

		//to-do
		// Account for subsurface scattering, if applicable

		if (bounces > 3)
		{
			Float q = std::max((Float).05, 1 - beta.y());
			if(sampler.Get1D() < q)
				break;
			beta /= 1 - q;
		}
	}
	return L;
}

PathIntegrator *CreatePathIntegrator(const ParamSet &params,
	std::shared_ptr<Sampler> sampler,
	std::shared_ptr<const Camera> camera) {
	int maxDepth = params.FindOneInt("maxdepth", 5);
	int np;
	const int *pb = params.FindInt("pixelbounds", &np);
	Bounds2i pixelBounds = camera->film->GetSampleBounds();
	if (pb) {
		if (np != 4)
			Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
				np);
		else {
			pixelBounds = Intersect(pixelBounds,
				Bounds2i{ { pb[0], pb[2] },{ pb[1], pb[3] } });
			if (pixelBounds.Area() == 0)
				Error("Degenerate \"pixelbounds\" specified.");
		}
	}
	Float rrThreshold = params.FindOneFloat("rrthreshold", 1.);
	std::string lightStrategy =
		params.FindOneString("lightsamplestrategy", "spatial");
	return new PathIntegrator(maxDepth, camera, sampler, pixelBounds,
		rrThreshold, lightStrategy);
}