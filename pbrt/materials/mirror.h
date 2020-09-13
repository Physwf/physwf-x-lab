#pragma once

#include "pbrt.h"
#include "material.h"

// MirrorMaterial Declarations
class MirrorMaterial : public Material {
public:
	// MirrorMaterial Public Methods
	MirrorMaterial(const std::shared_ptr<Texture<Spectrum>> &r,
		const std::shared_ptr<Texture<Float>> &bump) {
		Kr = r;
		bumpMap = bump;
	}
	void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
		TransportMode mode,
		bool allowMultipleLobes) const;

private:
	// MirrorMaterial Private Data
	std::shared_ptr<Texture<Spectrum>> Kr;
	std::shared_ptr<Texture<Float>> bumpMap;
};

MirrorMaterial *CreateMirrorMaterial(const TextureParams &mp);