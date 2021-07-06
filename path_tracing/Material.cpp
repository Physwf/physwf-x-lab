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
	LinearColor R;
	float alpha;
	MicrofacetDistribution* distribution = ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(alpha);
	LinearColor etaI;
	LinearColor etaT;
	LinearColor k;
	Fresnel* fresnel = ARENA_ALLOC(arena, FresnelConductor)(etaI, etaT, k);
	si->bsdf->Add(ARENA_ALLOC(arena, MicrofacetReflection)(R, distribution, fresnel));
}
