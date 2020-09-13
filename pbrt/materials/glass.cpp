#include "glass.h"
#include "spectrum.h"
#include "reflection.h"
#include "paramset.h"
#include "texture.h"
#include "interaction.h"

void GlassMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena, TransportMode mode, bool allowMultipleLobes) const
{

}


GlassMaterial *CreateGlassMaterial(const TextureParams &mp) {
	std::shared_ptr<Texture<Spectrum>> Kr =
		mp.GetSpectrumTexture("Kr", Spectrum(1.f));
	std::shared_ptr<Texture<Spectrum>> Kt =
		mp.GetSpectrumTexture("Kt", Spectrum(1.f));
	std::shared_ptr<Texture<Float>> eta = mp.GetFloatTextureOrNull("eta");
	if (!eta) eta = mp.GetFloatTexture("index", 1.5f);
	std::shared_ptr<Texture<Float>> roughu =
		mp.GetFloatTexture("uroughness", 0.f);
	std::shared_ptr<Texture<Float>> roughv =
		mp.GetFloatTexture("vroughness", 0.f);
	std::shared_ptr<Texture<Float>> bumpMap =
		mp.GetFloatTextureOrNull("bumpmap");
	bool remapRoughness = mp.FindBool("remaproughness", true);
	return new GlassMaterial(Kr, Kt, roughu, roughv, eta, bumpMap,
		remapRoughness);
}

