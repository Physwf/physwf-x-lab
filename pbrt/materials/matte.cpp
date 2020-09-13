#include "matte.h"
#include "reflection.h"
#include "interaction.h"
#include "texture.h"
#include "paramset.h"


void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const
{
	if (bumpMap)
		Bump(bumpMap,si);

	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	Spectrum r = Kd->Evaluate(*si).Clamp();
	Float sig = Clamp(sigma->Evaluate(*si), 0, 90);
	if (!r.IsBlack())
	{
		if (sigma == 0)
			si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection(r)));
		else
			si->bsdf->Add(ARENA_ALLOC(arena, OrenNayar)(r, sig));
	}
}

MatteMaterial * CreateMatteMaterial(const TextureParams &mp)
{
	std::shared_ptr<Texture<Spectrum>> Kd =
		mp.GetSpectrumTexture("Kd", Spectrum(0.5f));
	std::shared_ptr<Texture<Float>> sigma = mp.GetFloatTexture("sigma", 0.f);
	std::shared_ptr<Texture<Float>> bumpMap =
		mp.GetFloatTextureOrNull("bumpmap");
	return new MatteMaterial(Kd, sigma, bumpMap);
}
