#include "Material.h"
#include "SurfaceScattering.h"
#include "Fresnel.h"

void MirrorMaterial::ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena)
{
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	LinearColor etaI;
	LinearColor etaT;
	LinearColor k;
	LinearColor R;
	si->bsdf->Add(ARENA_ALLOC(arena, SpecularReflection)(R,ARENA_ALLOC(arena, FresnelConductor)(etaI, etaT,k)));
}

void MetalMaterial::ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena)
{
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	MicrofacetDistribution* distribution = ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(alpha);
	Fresnel* fresnel = ARENA_ALLOC(arena, FresnelConductor)(1.0f, eta, k);
	si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(1.0f, distribution, fresnel));
}

void MatteMaterial::ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena)
{
	si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);
	si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(Kd));
}
