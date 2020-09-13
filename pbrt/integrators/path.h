#pragma once

#include "pbrt.h"
#include "integrator.h"
#include "lightdistrib.h"

class PathIntegrator : public SamplerIntegrator
{
public:
	PathIntegrator(int maxDepth, std::shared_ptr<const Camera> camera, 
		std::shared_ptr<Sampler> sampler, const Bounds2i& pixelBounds, 
		Float rrThreshold = 1, const std::string& lightSampleStrategy = "spatial");

	virtual void Preprocess(const Scene& scene, Sampler &sampler) override;
	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */) const override;
private:
	const int maxDepth;
	const Float rrThreshold;
	const std::string lightSampleStrategy;
	std::unique_ptr<LightDistribution> lightDistribution;
};

PathIntegrator *CreatePathIntegrator(const ParamSet &params,
	std::shared_ptr<Sampler> sampler,
	std::shared_ptr<const Camera> camera);