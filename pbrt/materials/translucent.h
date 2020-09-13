#pragma once

#include "pbrt.h"
#include "material.h"

// TranslucentMaterial Declarations
class TranslucentMaterial : public Material {
public:
	// TranslucentMaterial Public Methods
	TranslucentMaterial(const std::shared_ptr<Texture<Spectrum>> &kd,
		const std::shared_ptr<Texture<Spectrum>> &ks,
		const std::shared_ptr<Texture<Float>> &rough,
		const std::shared_ptr<Texture<Spectrum>> &refl,
		const std::shared_ptr<Texture<Spectrum>> &trans,
		const std::shared_ptr<Texture<Float>> &bump,
		bool remap) {
		Kd = kd;
		Ks = ks;
		roughness = rough;
		reflect = refl;
		transmit = trans;
		bumpMap = bump;
		remapRoughness = remap;
	}
	void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
		TransportMode mode,
		bool allowMultipleLobes) const;

private:
	// TranslucentMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> Kd, Ks;
	std::shared_ptr<Texture<Float>> roughness;
	std::shared_ptr<Texture<Spectrum>> reflect, transmit;
	std::shared_ptr<Texture<Float>> bumpMap;
	bool remapRoughness;
};

TranslucentMaterial *CreateTranslucentMaterial(const TextureParams &mp);