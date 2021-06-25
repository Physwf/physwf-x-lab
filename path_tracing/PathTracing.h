#pragma once

#include "colorimetry.h"
#include "Scene.h"
#include "Camera.h"

class Sampler
{
public:
	virtual ~Sampler() {}
	virtual void StartPixel(const Vector2i& p);
	Vector2f GetPixelSample(const Vector2i& pRaster);
	virtual bool StartNextPixel();
	virtual std::unique_ptr<Sampler> Clone(int Seed) = 0;
};

class SamplerIntegrator
{
public:
	SamplerIntegrator(std::shared_ptr<const Camera> Incamera, std::shared_ptr<Sampler> Insampler)
		: camera(Incamera), sampler(Insampler) {}
	virtual void Render(const Scene& scene);
	virtual LinearColor Li(const Ray& ray,const Scene& scene, Sampler& sampler, int depth = 0) = 0;
protected:
	std::shared_ptr<const Camera> camera;
private:
	std::shared_ptr<Sampler> sampler;
};