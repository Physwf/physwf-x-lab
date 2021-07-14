#pragma once

#include "Colorimetry.h"
#include "Scene.h"
#include "Camera.h"
#include "Sampler.h"

LinearColor UniformSampleAllLights(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const std::vector<int>& nLightSamples);
LinearColor UniformSampleOneLight(const Interaction& it, const Scene& scene, MemoryArena& arena, Sampler& sampler, const Distribution1D* lightDistrib = nullptr);
LinearColor EstimateDirect(const Interaction& it, const Vector2f& uShading, const Light& light, const Vector2f& ulight, const Scene& scene, Sampler& sampler, MemoryArena& arena, bool specular = false);

class SamplerIntegrator
{
public:
	SamplerIntegrator(std::shared_ptr<const Camera> Incamera, std::shared_ptr<Sampler> Insampler)
		: camera(Incamera), sampler(Insampler) {}
	virtual void Preprocess(const Scene& scene, Sampler& sampler) {}
	virtual void Render(const Scene& scene);
	virtual LinearColor Li(const Ray& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth = 0) = 0;
protected:
	std::shared_ptr<const Camera> camera;
private:
	std::shared_ptr<Sampler> sampler;
};