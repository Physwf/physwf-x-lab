
#undef NEEDS_LIGHTMAP_COORDINATE
#define NEEDS_LIGHTMAP_COORDINATE		(HQ_TEXTURE_LIGHTMAP || LQ_TEXTURE_LIGHTMAP)

struct SharedBasePassInterpolants
{
    float3 PixelPositionExcludingWPO : TEXCOORD9;
};


#define BasePassInterpolantsVSToPS SharedBasePassInterpolants