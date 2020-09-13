#include "translucent.h"
#include "spectrum.h"
#include "reflection.h"
#include "paramset.h"
#include "texture.h"
#include "interaction.h"

void TranslucentMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena, TransportMode mode, bool allowMultipleLobes) const
{

}

TranslucentMaterial *CreateTranslucentMaterial(const TextureParams &mp) {
	std::shared_ptr<Texture<Spectrum>> Kd =
		mp.GetSpectrumTexture("Kd", Spectrum(0.25f));
	std::shared_ptr<Texture<Spectrum>> Ks =
		mp.GetSpectrumTexture("Ks", Spectrum(0.25f));
	std::shared_ptr<Texture<Spectrum>> reflect =
		mp.GetSpectrumTexture("reflect", Spectrum(0.5f));
	std::shared_ptr<Texture<Spectrum>> transmit =
		mp.GetSpectrumTexture("transmit", Spectrum(0.5f));
	std::shared_ptr<Texture<Float>> roughness =
		mp.GetFloatTexture("roughness", .1f);
	std::shared_ptr<Texture<Float>> bumpMap =
		mp.GetFloatTextureOrNull("bumpmap");
	bool remapRoughness = mp.FindBool("remaproughness", true);
	return new TranslucentMaterial(Kd, Ks, roughness, reflect, transmit,
		bumpMap, remapRoughness);
}
