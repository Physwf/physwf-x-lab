
#undef NEEDS_LIGHTMAP_COORDINATE
#define NEEDS_LIGHTMAP_COORDINATE		(HQ_TEXTURE_LIGHTMAP || LQ_TEXTURE_LIGHTMAP)

#define USES_GBUFFER 1 //	(MATERIALBLENDING_SOLID || MATERIALBLENDING_MASKED)

struct SharedBasePassInterpolants
{
    float3 PixelPositionExcludingWPO : TEXCOORD9;
};


#define BasePassInterpolantsVSToPS SharedBasePassInterpolants