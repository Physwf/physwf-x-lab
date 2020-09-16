#include "directlighting.h"
#include "interaction.h"
#include "paramset.h"
#include "camera.h"
#include "film.h"


Spectrum DirectLightingIntegrator::Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */) const
{
	struct Clock
	{
		Clock()
		{
			startTime = std::chrono::system_clock::now();
		}
		~Clock()
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			int64_t elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
			if (elapsedMS > 0) printf("elapsedMS:%lld\n", elapsedMS);
		}
		std::chrono::system_clock::time_point startTime;
	};
	//Clock c;
	Spectrum L(0.0f);
	SurfaceInteraction isect;
	{
		//Clock c;
		if (!scene.Intersect(ray, &isect))
		{
			for (const auto& light : scene.lights)
			{
				L += light->Le(ray);
			}
			return L;
		}
	}
	
	isect.ComputeScatteringFunctions(ray, arena);
	if (!isect.bsdf)
		return Li(isect.SpawnRay(ray.d), scene, sampler, arena, depth);

	Vector3f wo = isect.wo;
	L += isect.Le(wo);

	if (scene.lights.size() > 0)
	{
		if (strategy == LightStrategy::UniformSampleAll)
		{
			L += UniformSampleAllLight(isect, scene, arena, sampler, nLightSamples);
		}
		else
		{
			L += UniformSampleOneLight(isect, scene, arena, sampler);
		}
	}
	if (depth + 1 < maxDepth)
	{
		L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
		L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
	}
	
	return L;
}

void DirectLightingIntegrator::Preprocess(const Scene& scene, Sampler &sampler)
{
	if (strategy == LightStrategy::UniformSampleAll)
	{
		for (const auto& light : scene.lights)
		{
			nLightSamples.push_back(sampler.RoundCount(light->nSamples));
		}
		for (int i = 0; i < maxDepth; ++i)
		{
			for (size_t j = 0; j < scene.lights.size(); ++j)
			{
				sampler.Requeset2DArray(nLightSamples[j]);
				sampler.Requeset2DArray(nLightSamples[j]);
			}
		}
	}
}

DirectLightingIntegrator *CreateDirectLightingIntegrator(
	const ParamSet &params, std::shared_ptr<Sampler> sampler,
	std::shared_ptr<const Camera> camera) {
	int maxDepth = params.FindOneInt("maxdepth", 5);
	LightStrategy strategy;
	std::string st = params.FindOneString("strategy", "all");
	if (st == "one")
		strategy = LightStrategy::UniformSampleOne;
	else if (st == "all")
		strategy = LightStrategy::UniformSampleAll;
	else {
		Warning(
			"Strategy \"%s\" for direct lighting unknown. "
			"Using \"all\".",
			st.c_str());
		strategy = LightStrategy::UniformSampleAll;
	}
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
	return new DirectLightingIntegrator(strategy, maxDepth, camera, sampler,
		pixelBounds);
}