#pragma once

#include "pbrt.h"
#include "material.h"

class MatteMaterial : public Material
{
public:
	MatteMaterial(const std::shared_ptr<Texture<Spectrum>> &Kd,
		const std::shared_ptr<Texture<Float>> sigma,
		const std::shared_ptr<Texture<Float>>& bumpMap)
		:Kd(Kd),sigma(sigma),bumpMap(bumpMap) {}

	void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const;
private:
	std::shared_ptr<Texture<Spectrum>> Kd;//spectral diffuse value
	//roughness value
	std::shared_ptr<Texture<Float>> sigma;
	std::shared_ptr<Texture<Float>> bumpMap;
};

MatteMaterial *CreateMatteMaterial(const TextureParams &mp);