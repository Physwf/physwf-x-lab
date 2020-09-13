#pragma once

#include "pbrt.h"
#include "material.h"
#include "spectrum.h"

// MetalMaterial Declarations
class MetalMaterial : public Material {
public:
	// MetalMaterial Public Methods
	MetalMaterial(const std::shared_ptr<Texture<Spectrum>> &eta,
		const std::shared_ptr<Texture<Spectrum>> &k,
		const std::shared_ptr<Texture<Float>> &rough,
		const std::shared_ptr<Texture<Float>> &urough,
		const std::shared_ptr<Texture<Float>> &vrough,
		const std::shared_ptr<Texture<Float>> &bump,
		bool remapRoughness);
	void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
		TransportMode mode,
		bool allowMultipleLobes) const;

private:
	// MetalMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> eta, k;
	std::shared_ptr<Texture<Float>> roughness, uRoughness, vRoughness;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};

MetalMaterial *CreateMetalMaterial(const TextureParams &mp);