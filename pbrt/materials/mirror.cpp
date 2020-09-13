#include "mirror.h"
#include "spectrum.h"
#include "reflection.h"
#include "paramset.h"
#include "interaction.h"

void MirrorMaterial::ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena, TransportMode mode, bool allowMultipleLobes) const
{

}

MirrorMaterial *CreateMirrorMaterial(const TextureParams &mp) {
	std::shared_ptr<Texture<Spectrum>> Kr =
		mp.GetSpectrumTexture("Kr", Spectrum(0.9f));
	std::shared_ptr<Texture<Float>> bumpMap =
		mp.GetFloatTextureOrNull("bumpmap");
	return new MirrorMaterial(Kr, bumpMap);
}