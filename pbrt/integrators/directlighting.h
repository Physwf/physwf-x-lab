#pragma once

#include "pbrt.h"
#include "integrator.h"
#include "scene.h"

enum class LightStrategy { UniformSampleAll, UniformSampleOne };

class DirectLightingIntegrator : public SamplerIntegrator
{
public:
	DirectLightingIntegrator(LightStrategy strategy, int maxDepth,
		std::shared_ptr<const Camera> camera,
		std::shared_ptr<Sampler> sampler,
		const Bounds2i& pixelBounds)
		:SamplerIntegrator(camera, sampler, pixelBounds), strategy(strategy), maxDepth(maxDepth) {}

	Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */) const override;
	void Preprocess(const Scene& scene, Sampler &sampler) override;
private:
	const LightStrategy strategy;
	const int maxDepth;
	std::vector<int> nLightSamples;
};

DirectLightingIntegrator *CreateDirectLightingIntegrator(
	const ParamSet &params, std::shared_ptr<Sampler> sampler,
	std::shared_ptr<const Camera> camera);

