#pragma once

#include "SamplerIntegrator.h"

class LightDistribution;

class PathIntergrator : public SamplerIntegrator
{
public:
	PathIntergrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler, float rrThreshold);
	virtual LinearColor Li(const Ray& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth /* = 0 */) override;
private:
	const int maxDepth;
	const float rrThreshhold;
	const std::string lightSampleStrategy;
	std::unique_ptr<LightDistribution> lightDistribution;
};