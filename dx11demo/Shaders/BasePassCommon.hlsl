
#undef NEEDS_LIGHTMAP_COORDINATE
#define NEEDS_LIGHTMAP_COORDINATE		(HQ_TEXTURE_LIGHTMAP || LQ_TEXTURE_LIGHTMAP)

struct SharedBasePassInterpolants
{
};


#define BasePassInterpolantsVSToPS SharedBasePassInterpolants