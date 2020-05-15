/**
 * Define view modes to get specific show flag settings (some on, some off and some are not altered)
 * Don't change the order, the ID is serialized with the editor
 */
/*UENUM()*/
enum EViewModeIndex
{
	/** Wireframe w/ brushes. */
	VMI_BrushWireframe = 0,
	/** Wireframe w/ BSP. */
	VMI_Wireframe = 1,
	/** Unlit. */
	VMI_Unlit = 2,
	/** Lit. */
	VMI_Lit = 3,
	VMI_Lit_DetailLighting = 4,
	/** Lit wo/ materials. */
	VMI_LightingOnly = 5,
	/** Colored according to light count. */
	VMI_LightComplexity = 6,
	/** Colored according to shader complexity. */
	VMI_ShaderComplexity = 8,
	/** Colored according to world-space LightMap texture density. */
	VMI_LightmapDensity = 9,
	/** Colored according to light count - showing lightmap texel density on texture mapped objects. */
	VMI_LitLightmapDensity = 10,
	VMI_ReflectionOverride = 11,
	VMI_VisualizeBuffer = 12,
	//	VMI_VoxelLighting = 13,

	/** Colored according to stationary light overlap. */
	VMI_StationaryLightOverlap = 14,

	VMI_CollisionPawn = 15,
	VMI_CollisionVisibility = 16,
	//VMI_UNUSED = 17,
	/** Colored according to the current LOD index. */
	VMI_LODColoration = 18,
	/** Colored according to the quad coverage. */
	VMI_QuadOverdraw = 19,
	/** Visualize the accuracy of the primitive distance computed for texture streaming. */
	VMI_PrimitiveDistanceAccuracy = 20,
	/** Visualize the accuracy of the mesh UV densities computed for texture streaming. */
	VMI_MeshUVDensityAccuracy = 21,
	/** Colored according to shader complexity, including quad overdraw. */
	VMI_ShaderComplexityWithQuadOverdraw = 22,
	/** Colored according to the current HLOD index. */
	VMI_HLODColoration = 23,
	/** Group item for LOD and HLOD coloration*/
	VMI_GroupLODColoration = 24,
	/** Visualize the accuracy of the material texture scales used for texture streaming. */
	VMI_MaterialTextureScaleAccuracy = 25,
	/** Compare the required texture resolution to the actual resolution. */
	VMI_RequiredTextureResolution = 26,

	VMI_Max /*UMETA(Hidden)*/,

	VMI_Unknown = 255 /*UMETA(Hidden)*/,
};

/** Settings to allow designers to override the automatic expose */
/*USTRUCT()*/
struct FExposureSettings
{
	/*GENERATED_USTRUCT_BODY()*/

		FExposureSettings() : FixedEV100(1), bFixed(false)
	{
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d,%d"), FixedEV100, bFixed ? 1 : 0);
	}

	void SetFromString(const TCHAR *In)
	{
		// set to default
		*this = FExposureSettings();

		const TCHAR* Comma = FCString::Strchr(In, TEXT(','));
		check(Comma);

		const int32 BUFFER_SIZE = 128;
		TCHAR Buffer[BUFFER_SIZE];
		check((Comma - In) + 1 < BUFFER_SIZE);

		FCString::Strncpy(Buffer, In, (Comma - In) + 1);
		FixedEV100 = FCString::Atof(Buffer);
		bFixed = !!FCString::Atoi(Comma + 1);
	}

	// EV100 settings for fixed mode
	/*UPROPERTY()*/
		float FixedEV100;
	// true: fixed exposure using the LogOffset value, false: automatic eye adaptation
	/*UPROPERTY()*/
		bool bFixed;
};