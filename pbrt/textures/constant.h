#pragma once

#include "pbrt.h"
#include "texture.h"
#include "paramset.h"

// ConstantTexture Declarations
template <typename T>
class ConstantTexture : public Texture<T> {
public:
	// ConstantTexture Public Methods
	ConstantTexture(const T &value) : value(value) {}
	T Evaluate(const SurfaceInteraction &) const { return value; }

private:
	T value;
};

ConstantTexture<Float> *CreateConstantFloatTexture(const Transform &tex2world,
	const TextureParams &tp);
ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
	const Transform &tex2world, const TextureParams &tp);
