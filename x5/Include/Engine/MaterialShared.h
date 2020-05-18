#pragma once

#include "CoreMinimal.h"
#include "Containers/IndirectArray.h"
#include "Misc/Guid.h"
#include "Engine/EngineTypes.h"
#include "Templates/RefCounting.h"
//#include "Templates/ScopedPointer.h"
#include "Misc/SecureHash.h"
#include "RHI/RHI.h"
#include "RenderCore/RenderResource.h"
#include "RenderCore/RenderingThread.h"
#include "RenderCore/UniformBuffer.h"
#include "ShaderCore/Shader.h"
#include "ShaderCore/VertexFactory.h"
#include "SceneTypes.h"
#include "StaticParameterSet.h"
#include "Misc/Optional.h"

#include "X5.h"

class FMaterial;
class FMaterialCompiler;
class FMaterialRenderProxy;
class FMaterialShaderType;
class FMaterialUniformExpression;
class FMeshMaterialShaderType;
class FSceneView;
class FShaderCommonCompileJob;
class UMaterial;
class UMaterialExpression;
class UMaterialExpressionMaterialFunctionCall;
class UMaterialInstance;
class UMaterialInterface;
class USubsurfaceProfile;
class UTexture;
struct FExpressionInput;

extern void GetMaterialQualityLevelName(EMaterialQualityLevel::Type InMaterialQualityLevel, FString& OutName);

inline bool IsSubsurfaceShadingModel(EMaterialShadingModel ShadingModel)
{
	return ShadingModel == MSM_Subsurface || ShadingModel == MSM_PreintegratedSkin ||
		ShadingModel == MSM_SubsurfaceProfile || ShadingModel == MSM_TwoSidedFoliage ||
		ShadingModel == MSM_Cloth || ShadingModel == MSM_Eye;
}

inline bool UseSubsurfaceProfile(EMaterialShadingModel ShadingModel)
{
	return ShadingModel == MSM_SubsurfaceProfile || ShadingModel == MSM_Eye;
}

inline uint32 GetUseSubsurfaceProfileShadingModelMask()
{
	return (1 << MSM_SubsurfaceProfile) | (1 << MSM_Eye);
}

/**
 * The types which can be used by materials.
 */
enum EMaterialValueType
{
	/**
	 * A scalar float type.
	 * Note that MCT_Float1 will not auto promote to any other float types,
	 * So use MCT_Float instead for scalar expression return types.
	 */
	MCT_Float1 = 1,
	MCT_Float2 = 2,
	MCT_Float3 = 4,
	MCT_Float4 = 8,

	/**
	 * Any size float type by definition, but this is treated as a scalar which can auto convert (by replication) to any other size float vector.
	 * Use this as the type for any scalar expressions.
	 */
	MCT_Float = 8 | 4 | 2 | 1,
	MCT_Texture2D = 16,
	MCT_TextureCube = 32,
	MCT_VolumeTexture = 64,
	MCT_StaticBool = 128,
	MCT_Unknown = 256,
	MCT_MaterialAttributes = 512,
	MCT_TextureExternal = 1024,
	MCT_Texture = 16 | 32 | 64 | 1024,
};

/**
 * The common bases of material
 */
enum EMaterialCommonBasis
{
	MCB_Tangent,
	MCB_Local,
	MCB_TranslatedWorld,
	MCB_World,
	MCB_View,
	MCB_Camera,
	MCB_MeshParticle,
	MCB_MAX,
};

//when setting deferred scene resources whether to throw warnings when we fall back to defaults.
enum struct EDeferredParamStrictness
{
	ELoose, // no warnings
	EStrict, // throw warnings
};

/** Defines the domain of a material. */
//UENUM()
enum EMaterialDomain
{
	/** The material's attributes describe a 3d surface. */
	MD_Surface /*UMETA(DisplayName = "Surface")*/,
	/** The material's attributes describe a deferred decal, and will be mapped onto the decal's frustum. */
	MD_DeferredDecal /*UMETA(DisplayName = "Deferred Decal")*/,
	/** The material's attributes describe a light's distribution. */
	MD_LightFunction /*UMETA(DisplayName = "Light Function")*/,
	/** The material's attributes describe a 3d volume. */
	MD_Volume /*UMETA(DisplayName = "Volume")*/,
	/** The material will be used in a custom post process pass. */
	MD_PostProcess /*UMETA(DisplayName = "Post Process")*/,
	/** The material will be used for UMG or Slate UI */
	MD_UI /*UMETA(DisplayName = "User Interface")*/,

	MD_MAX
};

X5_API FString MaterialDomainString(EMaterialDomain MaterialDomain);


/**
 * The context of a material being rendered.
 */
struct X5_API FMaterialRenderContext
{
	/** material instance used for the material shader */
	const FMaterialRenderProxy* MaterialRenderProxy;
	/** Material resource to use. */
	const FMaterial& Material;

	/** Whether or not selected objects should use their selection color. */
	bool bShowSelection;

	/**
	* Constructor
	*/
	FMaterialRenderContext(
		const FMaterialRenderProxy* InMaterialRenderProxy,
		const FMaterial& InMaterial,
		const FSceneView* InView);
};

/**
 * Represents a subclass of FMaterialUniformExpression.
 */
class FMaterialUniformExpressionType
{
public:

	typedef class FMaterialUniformExpression* (*SerializationConstructorType)();

	/**
	 * @return The global uniform expression type list.  The list is used to temporarily store the types until
	 *			the name subsystem has been initialized.
	 */
	static TLinkedList<FMaterialUniformExpressionType*>*& GetTypeList();

	/**
	 * Should not be called until the name subsystem has been initialized.
	 * @return The global uniform expression type map.
	 */
	static TMap<FName, FMaterialUniformExpressionType*>& GetTypeMap();

	/**
	 * Minimal initialization constructor.
	 */
	FMaterialUniformExpressionType(const TCHAR* InName, SerializationConstructorType InSerializationConstructor);

	/**
	 * Serializer for references to uniform expressions.
	 */
	friend FArchive& operator<<(FArchive& Ar, class FMaterialUniformExpression*& Ref);
	friend FArchive& operator<<(FArchive& Ar, class FMaterialUniformExpressionTexture*& Ref);
	friend FArchive& operator<<(FArchive& Ar, class FMaterialUniformExpressionExternalTexture*& Ref);

	const TCHAR* GetName() const { return Name; }

private:

	const TCHAR* Name;
	SerializationConstructorType SerializationConstructor;
};

#define DECLARE_MATERIALUNIFORMEXPRESSION_TYPE(Name) \
	public: \
	static FMaterialUniformExpressionType StaticType; \
	static FMaterialUniformExpression* SerializationConstructor() { return new Name(); } \
	virtual FMaterialUniformExpressionType* GetType() const { return &StaticType; }

#define IMPLEMENT_MATERIALUNIFORMEXPRESSION_TYPE(Name) \
	FMaterialUniformExpressionType Name::StaticType(TEXT(#Name),&Name::SerializationConstructor);


/**
 * Represents an expression which only varies with uniform inputs.
 */
class FMaterialUniformExpression : public FRefCountedObject
{
public:

	virtual ~FMaterialUniformExpression() {}

	virtual FMaterialUniformExpressionType* GetType() const = 0;
	virtual void Serialize(FArchive& Ar) = 0;
	virtual void GetNumberValue(const struct FMaterialRenderContext& Context, FLinearColor& OutValue) const {}
	virtual class FMaterialUniformExpressionTexture* GetTextureUniformExpression() { return nullptr; }
	virtual class FMaterialUniformExpressionExternalTexture* GetExternalTextureUniformExpression() { return nullptr; }
	virtual bool IsConstant() const { return false; }
	virtual bool IsIdentical(const FMaterialUniformExpression* OtherExpression) const { return false; }

	friend FArchive& operator<<(FArchive& Ar, class FMaterialUniformExpression*& Ref);
};

/**
 * A texture expression.
 */
class FMaterialUniformExpressionTexture : public FMaterialUniformExpression
{
	DECLARE_MATERIALUNIFORMEXPRESSION_TYPE(FMaterialUniformExpressionTexture);
public:

	FMaterialUniformExpressionTexture();

	FMaterialUniformExpressionTexture(int32 InTextureIndex, ESamplerSourceMode InSamplerSource);

	// FMaterialUniformExpression interface.
	virtual void Serialize(FArchive& Ar);
	virtual void GetTextureValue(const FMaterialRenderContext& Context, const FMaterial& Material, const UTexture*& OutValue, ESamplerSourceMode& OutSamplerSource) const;
	/** Accesses the texture used for rendering this uniform expression. */
	virtual void GetGameThreadTextureValue(const class UMaterialInterface* MaterialInterface, const FMaterial& Material, UTexture*& OutValue, bool bAllowOverride = true) const;
	virtual class FMaterialUniformExpressionTexture* GetTextureUniformExpression() { return this; }
	void SetTransientOverrideTextureValue(UTexture* InOverrideTexture);

	virtual bool IsConstant() const
	{
		return false;
	}
	virtual bool IsIdentical(const FMaterialUniformExpression* OtherExpression) const;

	friend FArchive& operator<<(FArchive& Ar, class FMaterialUniformExpressionTexture*& Ref);

	int32 GetTextureIndex() const { return TextureIndex; }

protected:
	/** Index into FMaterial::GetReferencedTextures */
	int32 TextureIndex;
	ESamplerSourceMode SamplerSource;
	/** Texture that may be used in the editor for overriding the texture but never saved to disk, accessible only by the game thread! */
	UTexture* TransientOverrideValue_GameThread;
	/** Texture that may be used in the editor for overriding the texture but never saved to disk, accessible only by the rendering thread! */
	UTexture* TransientOverrideValue_RenderThread;
};

class FMaterialUniformExpressionExternalTextureBase : public FMaterialUniformExpression
{
	DECLARE_MATERIALUNIFORMEXPRESSION_TYPE(FMaterialUniformExpressionExternalTextureBase);
public:

	FMaterialUniformExpressionExternalTextureBase(int32 InSourceTextureIndex = INDEX_NONE);
	FMaterialUniformExpressionExternalTextureBase(const FGuid& InExternalTextureGuid);

	virtual void Serialize(FArchive& Ar) override;
	virtual bool IsConstant() const override { return false; }
	virtual bool IsIdentical(const FMaterialUniformExpression* OtherExpression) const override;

protected:

	/** Resolve the guid that relates to texture information inside FExternalTexture */
	FGuid ResolveExternalTextureGUID(const FMaterialRenderContext& Context, TOptional<FName> ParameterName = TOptional<FName>()) const;

	/** Index of the texture in the material that should be used to retrieve the external texture GUID at runtime (or INDEX_NONE) */
	int32 SourceTextureIndex;
	/** Optional external texture GUID defined at compile time */
	FGuid ExternalTextureGuid;
};

/**
* An external texture expression.
*/
class FMaterialUniformExpressionExternalTexture : public FMaterialUniformExpressionExternalTextureBase
{
	DECLARE_MATERIALUNIFORMEXPRESSION_TYPE(FMaterialUniformExpressionExternalTexture);
public:

	FMaterialUniformExpressionExternalTexture(int32 InSourceTextureIndex = INDEX_NONE) : FMaterialUniformExpressionExternalTextureBase(InSourceTextureIndex) {}
	FMaterialUniformExpressionExternalTexture(const FGuid& InGuid) : FMaterialUniformExpressionExternalTextureBase(InGuid) {}

	// FMaterialUniformExpression interface.
	virtual FMaterialUniformExpressionExternalTexture* GetExternalTextureUniformExpression() override { return this; }

	// Lookup the external texture if it is set
	virtual bool GetExternalTexture(const FMaterialRenderContext& Context, FTextureRHIRef& OutTextureRHI, FSamplerStateRHIRef& OutSamplerStateRHI) const;

	friend FArchive& operator<<(FArchive& Ar, FMaterialUniformExpressionExternalTexture*& Ref)
	{
		Ar << (FMaterialUniformExpression*&)Ref;
		return Ar;
	}
};

/** Stores all uniform expressions for a material generated from a material translation. */
class FUniformExpressionSet : public FRefCountedObject
{
public:
	FUniformExpressionSet() {}

	X5_API void Serialize(FArchive& Ar);
	bool IsEmpty() const;
	bool operator==(const FUniformExpressionSet& ReferenceSet) const;
	FString GetSummaryString() const;

	void SetParameterCollections(const TArray<class UMaterialParameterCollection*>& Collections);
	void CreateBufferStruct();
	X5_API const FUniformBufferStruct& GetUniformBufferStruct() const;

	X5_API FUniformBufferRHIRef CreateUniformBuffer(const FMaterialRenderContext& MaterialRenderContext, FRHICommandList* CommandListIfLocalMode, struct FLocalUniformBuffer* OutLocalUniformBuffer) const;

	uint32 GetAllocatedSize() const
	{
		return UniformVectorExpressions.GetAllocatedSize()
			+ UniformScalarExpressions.GetAllocatedSize()
			+ Uniform2DTextureExpressions.GetAllocatedSize()
			+ UniformCubeTextureExpressions.GetAllocatedSize()
			+ UniformVolumeTextureExpressions.GetAllocatedSize()
			+ UniformExternalTextureExpressions.GetAllocatedSize()
			+ ParameterCollections.GetAllocatedSize()
			+ (UniformBufferStruct ? (sizeof(FUniformBufferStruct) + UniformBufferStruct->GetMembers().GetAllocatedSize()) : 0);
	}

protected:

	TArray<TRefCountPtr<FMaterialUniformExpression> > UniformVectorExpressions;
	TArray<TRefCountPtr<FMaterialUniformExpression> > UniformScalarExpressions;
	TArray<TRefCountPtr<FMaterialUniformExpressionTexture> > Uniform2DTextureExpressions;
	TArray<TRefCountPtr<FMaterialUniformExpressionTexture> > UniformCubeTextureExpressions;
	TArray<TRefCountPtr<FMaterialUniformExpressionTexture> > UniformVolumeTextureExpressions;
	TArray<TRefCountPtr<FMaterialUniformExpressionExternalTexture> > UniformExternalTextureExpressions;

	/** Ids of parameter collections referenced by the material that was translated. */
	TArray<FGuid> ParameterCollections;

	/** The structure of a uniform buffer containing values for these uniform expressions. */
	TOptional<FUniformBufferStruct> UniformBufferStruct;

	friend class FMaterial;
	friend class FHLSLMaterialTranslator;
	friend class FMaterialShaderMap;
	friend class FMaterialShader;
	friend class FMaterialRenderProxy;
	friend class FDebugUniformExpressionSet;
};

/** Stores outputs from the material compile that need to be saved. */
class FMaterialCompilationOutput
{
public:
	FMaterialCompilationOutput() :
		NumUsedUVScalars(0),
		NumUsedCustomInterpolatorScalars(0),
		EstimatedNumTextureSamplesVS(0),
		EstimatedNumTextureSamplesPS(0),
		bRequiresSceneColorCopy(false),
		bNeedsSceneTextures(false),
		bUsesEyeAdaptation(false),
		bModifiesMeshPosition(false),
		bUsesWorldPositionOffset(false),
		bNeedsGBuffer(false),
		bUsesGlobalDistanceField(false),
		bUsesPixelDepthOffset(false),
		bUsesSceneDepthLookup(false)
	{}

	X5_API void Serialize(FArchive& Ar);

	FUniformExpressionSet UniformExpressionSet;

	/** Number of used custom UV scalars. */
	uint8 NumUsedUVScalars;

	/** Number of used custom vertex interpolation scalars. */
	uint8 NumUsedCustomInterpolatorScalars;

	/** Number of times SampleTexture is called, excludes custom nodes. */
	uint16 EstimatedNumTextureSamplesVS;
	uint16 EstimatedNumTextureSamplesPS;

	/** Indicates whether the material uses scene color. */
	bool bRequiresSceneColorCopy;

	/** true if the material needs the scenetexture lookups. */
	bool bNeedsSceneTextures;

	/** true if the material uses the EyeAdaptationLookup */
	bool bUsesEyeAdaptation;

	/** true if the material modifies the the mesh position. */
	bool bModifiesMeshPosition;

	/** Whether the material uses world position offset. */
	bool bUsesWorldPositionOffset;

	/** true if the material uses any GBuffer textures */
	bool bNeedsGBuffer;

	/** true if material uses the global distance field */
	bool bUsesGlobalDistanceField;

	/** true if the material writes a pixel depth offset */
	bool bUsesPixelDepthOffset;

	/** true if the material uses the SceneDepth lookup */
	bool bUsesSceneDepthLookup;
};

/**
 * Usage options for a shader map.
 * The purpose of EMaterialShaderMapUsage is to allow creating a unique yet deterministic (no appCreateGuid) Id,
 * For a shader map corresponding to any UMaterial or UMaterialInstance, for different use cases.
 * As an example, when exporting a material to Lightmass we want to compile a shader map with FLightmassMaterialProxy,
 * And generate a FMaterialShaderMapId for it that allows reuse later, so it must be deterministic.
 */
namespace EMaterialShaderMapUsage
{
	enum Type
	{
		Default,
		LightmassExportEmissive,
		LightmassExportDiffuse,
		LightmassExportOpacity,
		LightmassExportNormal,
		MaterialExportBaseColor,
		MaterialExportSpecular,
		MaterialExportNormal,
		MaterialExportMetallic,
		MaterialExportRoughness,
		MaterialExportAO,
		MaterialExportEmissive,
		MaterialExportOpacity,
		MaterialExportOpacityMask,
		MaterialExportSubSurfaceColor,
		DebugViewModeTexCoordScale,
		DebugViewModeRequiredTextureResolution
	};
}

/** Contains all the information needed to uniquely identify a FMaterialShaderMap. */
class FMaterialShaderMapId
{
public:

	/**
	 * The base material's StateId.
	 * This guid represents all the state of a UMaterial that is not covered by the other members of FMaterialShaderMapId.
	 * Any change to the UMaterial that modifes that state (for example, adding an expression) must modify this guid.
	 */
	FGuid BaseMaterialId;

	/**
	 * Quality level that this shader map is going to be compiled at.
	 * Can be a value of EMaterialQualityLevel::Num if quality level doesn't matter to the compiled result.
	 */
	EMaterialQualityLevel::Type QualityLevel;

	/** Feature level that the shader map is going to be compiled for. */
	ERHIFeatureLevel::Type FeatureLevel;

	/**
	 * Indicates what use case this shader map will be for.
	 * This allows the same UMaterial / UMaterialInstance to be compiled with multiple FMaterial derived classes,
	 * While still creating an Id that is deterministic between runs (no appCreateGuid used).
	 */
	EMaterialShaderMapUsage::Type Usage;

private:
	/** Static parameters and base Id. */
	//FStaticParameterSet ParameterSet;
	FString ParameterSetLayerParametersKey;

public:
	/** Guids of any functions the material was dependent on. */
	TArray<FGuid> ReferencedFunctions;

	/** Guids of any Parameter Collections the material was dependent on. */
	TArray<FGuid> ReferencedParameterCollections;

	/** Guids of any Shared Input Collections the material was dependent on. */
	TArray<FGuid> ReferencedSharedInputCollections;

	/** Shader types of shaders that are inlined in this shader map in the DDC. */
	TArray<FShaderTypeDependency> ShaderTypeDependencies;

	/** Shader pipeline types of shader pipelines that are inlined in this shader map in the DDC. */
	TArray<FShaderPipelineTypeDependency> ShaderPipelineTypeDependencies;

	/** Vertex factory types of shaders that are inlined in this shader map in the DDC. */
	TArray<FVertexFactoryTypeDependency> VertexFactoryTypeDependencies;

	/**
	 * Hash of the textures referenced by the uniform expressions in the shader map.
	 * This is stored in the shader map Id to gracefully handle situations where code changes
	 * that generates the array of textures that the uniform expressions use to link up after being loaded from the DDC.
	 */
	FSHAHash TextureReferencesHash;

	/** A hash of the base property overrides for this material instance. */
	FSHAHash BasePropertyOverridesHash;


	FMaterialShaderMapId()
		: BaseMaterialId(0, 0, 0, 0)
		, QualityLevel(EMaterialQualityLevel::High)
		, FeatureLevel(ERHIFeatureLevel::SM4)
		, Usage(EMaterialShaderMapUsage::Default)
	{ }

	~FMaterialShaderMapId()
	{ }

	X5_API void SetShaderDependencies(const TArray<FShaderType*>& ShaderTypes, const TArray<const FShaderPipelineType*>& ShaderPipelineTypes, const TArray<FVertexFactoryType*>& VFTypes);

	void Serialize(FArchive& Ar);

	friend uint32 GetTypeHash(const FMaterialShaderMapId& Ref)
	{
		return Ref.BaseMaterialId.A;
	}

	SIZE_T GetSizeBytes() const
	{
		return sizeof(*this)
			+ ReferencedFunctions.GetAllocatedSize()
			+ ReferencedParameterCollections.GetAllocatedSize()
			+ ReferencedSharedInputCollections.GetAllocatedSize()
			+ ShaderTypeDependencies.GetAllocatedSize()
			+ ShaderPipelineTypeDependencies.GetAllocatedSize()
			+ VertexFactoryTypeDependencies.GetAllocatedSize();
	}

	/** Hashes the material-specific part of this shader map Id. */
	void GetMaterialHash(FSHAHash& OutHash) const;

	/**
	* Tests this set against another for equality, disregarding override settings.
	*
	* @param ReferenceSet	The set to compare against
	* @return				true if the sets are equal
	*/
	bool operator==(const FMaterialShaderMapId& ReferenceSet) const;

	bool operator!=(const FMaterialShaderMapId& ReferenceSet) const
	{
		return !(*this == ReferenceSet);
	}

	/** Updates the Id's static parameter set */
	//void UpdateParameterSet(const FStaticParameterSet& StaticParameters);

// 	const FStaticParameterSet& GetParameterSet() const
// 	{
// 		return ParameterSet;
// 	}

	const FString& GetParameterSetLayerParametersKey() const
	{
		return ParameterSetLayerParametersKey;
	}

	/** Appends string representations of this Id to a key string. */
	void AppendKeyString(FString& KeyString) const;

	/** Returns true if the requested shader type is a dependency of this shader map Id. */
	bool ContainsShaderType(const FShaderType* ShaderType) const;

	/** Returns true if the requested shader type is a dependency of this shader map Id. */
	bool ContainsShaderPipelineType(const FShaderPipelineType* ShaderPipelineType) const;

	/** Returns true if the requested vertex factory type is a dependency of this shader map Id. */
	bool ContainsVertexFactoryType(const FVertexFactoryType* VFType) const;
};

/**
 * The shaders which the render the material on a mesh generated by a particular vertex factory type.
 */
class FMeshMaterialShaderMap : public TShaderMap<FMeshMaterialShaderType>
{
public:

	FMeshMaterialShaderMap(EShaderPlatform InPlatform, FVertexFactoryType* InVFType)
		: TShaderMap<FMeshMaterialShaderType>(InPlatform)
		, VertexFactoryType(InVFType)
	{}

	/**
	 * Enqueues compilation for all shaders for a material and vertex factory type.
	 * @param Material - The material to compile shaders for.
	 * @param VertexFactoryType - The vertex factory type to compile shaders for.
	 * @param Platform - The platform to compile for.
	 */
	uint32 BeginCompile(
		uint32 ShaderMapId,
		const FMaterialShaderMapId& InShaderMapId,
		const FMaterial* Material,
		FShaderCompilerEnvironment* MaterialEnvironment,
		EShaderPlatform Platform,
		TArray<FShaderCommonCompileJob*>& NewJobs
	);

	/**
	 * Checks whether a material shader map is missing any shader types necessary for the given material.
	 * May be called with a NULL FMeshMaterialShaderMap, which is equivalent to a FMeshMaterialShaderMap with no shaders cached.
	 * @param MeshShaderMap - The FMeshMaterialShaderMap to check for the necessary shaders.
	 * @param Material - The material which is checked.
	 * @return True if the shader map has all of the shader types necessary.
	 */
	static bool IsComplete(
		const FMeshMaterialShaderMap* MeshShaderMap,
		EShaderPlatform Platform,
		const FMaterial* Material,
		FVertexFactoryType* InVertexFactoryType,
		bool bSilent
	);

	void LoadMissingShadersFromMemory(
		const FSHAHash& MaterialShaderMapHash,
		const FMaterial* Material,
		EShaderPlatform Platform);

	/**
	 * Removes all entries in the cache with exceptions based on a shader type
	 * @param ShaderType - The shader type to flush
	 */
	void FlushShadersByShaderType(FShaderType* ShaderType);

	/**
 * Removes all entries in the cache with exceptions based on a shader type
 * @param ShaderType - The shader type to flush
 */
	void FlushShadersByShaderPipelineType(const FShaderPipelineType* ShaderPipelineType);

	// Accessors.
	inline FVertexFactoryType* GetVertexFactoryType() const { return VertexFactoryType; }

private:
	/** The vertex factory type these shaders are for. */
	FVertexFactoryType* VertexFactoryType;

	static bool IsMeshShaderComplete(const FMeshMaterialShaderMap* MeshShaderMap, EShaderPlatform Platform, const FMaterial* Material, const FMeshMaterialShaderType* ShaderType, const FShaderPipelineType* Pipeline, FVertexFactoryType* InVertexFactoryType, bool bSilent);
};

/**
 * The set of material shaders for a single material.
 */
class FMaterialShaderMap : public TShaderMap<FMaterialShaderType>, public FDeferredCleanupInterface
{
public:

	/**
	 * Finds the shader map for a material.
	 * @param StaticParameterSet - The static parameter set identifying the shader map
	 * @param Platform - The platform to lookup for
	 * @return NULL if no cached shader map was found.
	 */
	static FMaterialShaderMap* FindId(const FMaterialShaderMapId& ShaderMapId, EShaderPlatform Platform);

	/** Flushes the given shader types from any loaded FMaterialShaderMap's. */
	static void FlushShaderTypes(TArray<FShaderType*>& ShaderTypesToFlush, TArray<const FShaderPipelineType*>& ShaderPipelineTypesToFlush, TArray<const FVertexFactoryType*>& VFTypesToFlush);

	static void FixupShaderTypes(EShaderPlatform Platform,
		const TMap<FShaderType*, FString>& ShaderTypeNames,
		const TMap<const FShaderPipelineType*, FString>& ShaderPipelineTypeNames,
		const TMap<FVertexFactoryType*, FString>& VertexFactoryTypeNames);

	/**
	 * Attempts to load the shader map for the given material from the Derived Data Cache.
	 * If InOutShaderMap is valid, attempts to load the individual missing shaders instead.
	 */
	static void LoadFromDerivedDataCache(const FMaterial* Material, const FMaterialShaderMapId& ShaderMapId, EShaderPlatform Platform, TRefCountPtr<FMaterialShaderMap>& InOutShaderMap);

	inline FMaterialShaderMap() : FMaterialShaderMap(EShaderPlatform::SP_NumPlatforms) {}
	FMaterialShaderMap(EShaderPlatform InPlatform);

	// Destructor.
	~FMaterialShaderMap();

	/**
	 * Compiles the shaders for a material and caches them in this shader map.
	 * @param Material - The material to compile shaders for.
	 * @param ShaderMapId - the set of static parameters to compile for
	 * @param Platform - The platform to compile to
	 */
	void Compile(
		FMaterial* Material,
		const FMaterialShaderMapId& ShaderMapId,
		TRefCountPtr<FShaderCompilerEnvironment> MaterialEnvironment,
		const FMaterialCompilationOutput& InMaterialCompilationOutput,
		EShaderPlatform Platform,
		bool bSynchronousCompile,
		bool bApplyCompletedShaderMapForRendering
	);

	/** Sorts the incoming compiled jobs into the appropriate mesh shader maps, and finalizes this shader map so that it can be used for rendering. */
	bool ProcessCompilationResults(const TArray<FShaderCommonCompileJob*>& InCompilationResults, int32& ResultIndex, float& TimeBudget, TMap<const FVertexFactoryType*, TArray<const FShaderPipelineType*> >& SharedPipelines);

	/**
	 * Checks whether the material shader map is missing any shader types necessary for the given material.
	 * @param Material - The material which is checked.
	 * @return True if the shader map has all of the shader types necessary.
	 */
	bool IsComplete(const FMaterial* Material, bool bSilent);

	/** Attempts to load missing shaders from memory. */
	void LoadMissingShadersFromMemory(const FMaterial* Material);

	/**
	 * Checks to see if the shader map is already being compiled for another material, and if so
	 * adds the specified material to the list to be applied to once the compile finishes.
	 * @param Material - The material we also wish to apply the compiled shader map to.
	 * @return True if the shader map was being compiled and we added Material to the list to be applied.
	 */
	bool TryToAddToExistingCompilationTask(FMaterial* Material);

#if WITH_EDITOR
	ENGINE_API const FString *GetShaderSource(const FName ShaderTypeName) const;
#endif

	/** Builds a list of the shaders in a shader map. */
	X5_API void GetShaderList(TMap<FShaderId, FShader*>& OutShaders) const;

	/** Builds a list of the shaders in a shader map. Key is FShaderType::TypeName */
	X5_API void GetShaderList(TMap<FName, FShader*>& OutShaders) const;

	/** Builds a list of the shader pipelines in a shader map. */
	X5_API void GetShaderPipelineList(TArray<FShaderPipeline*>& OutShaderPipelines) const;

	/** Registers a material shader map in the global map so it can be used by materials. */
	void Register(EShaderPlatform InShaderPlatform);

	// Reference counting.
	X5_API void AddRef();
	X5_API void Release();

	/**
	 * Removes all entries in the cache with exceptions based on a shader type
	 * @param ShaderType - The shader type to flush
	 */
	void FlushShadersByShaderType(FShaderType* ShaderType);

	/**
	 * Removes all entries in the cache with exceptions based on a shader pipeline type
	 * @param ShaderPipelineType - The shader pipeline type to flush
	 */
	void FlushShadersByShaderPipelineType(const FShaderPipelineType* ShaderPipelineType);

	/**
	 * Removes all entries in the cache with exceptions based on a vertex factory type
	 * @param ShaderType - The shader type to flush
	 */
	void FlushShadersByVertexFactoryType(const FVertexFactoryType* VertexFactoryType);

	/** Removes a material from ShaderMapsBeingCompiled. */
	static void RemovePendingMaterial(FMaterial* Material);

	/** Finds a shader map currently being compiled that was enqueued for the given material. */
	static const FMaterialShaderMap* GetShaderMapBeingCompiled(const FMaterial* Material);

	/** Serializes the shader map. */
	void Serialize(FArchive& Ar, bool bInlineShaderResources = true);

	/** Saves this shader map to the derived data cache. */
	void SaveToDerivedDataCache();

	/** Registers all shaders that have been loaded in Serialize */
	virtual void RegisterSerializedShaders() override;
	virtual void DiscardSerializedShaders() override;

	/** Backs up any FShaders in this shader map to memory through serialization and clears FShader references. */
	TArray<uint8>* BackupShadersToMemory();
	/** Recreates FShaders from the passed in memory, handling shader key changes. */
	void RestoreShadersFromMemory(const TArray<uint8>& ShaderData);

	/** Serializes a shader map to an archive (used with recompiling shaders for a remote console) */
	X5_API static void SaveForRemoteRecompile(FArchive& Ar, const TMap<FString, TArray<TRefCountPtr<FMaterialShaderMap> > >& CompiledShaderMaps, const TArray<FShaderResourceId>& ClientResourceIds);
	X5_API static void LoadForRemoteRecompile(FArchive& Ar, EShaderPlatform ShaderPlatform, const TArray<FString>& MaterialsForShaderMaps);

	/** Computes the memory used by this shader map without counting the shaders themselves. */
	uint32 GetSizeBytes() const
	{
		return sizeof(*this)
			+ MeshShaderMaps.GetAllocatedSize()
			+ OrderedMeshShaderMaps.GetAllocatedSize()
			+ VertexFactoryMap.GetAllocatedSize()
			+ MaterialCompilationOutput.UniformExpressionSet.GetAllocatedSize()
#if ALLOW_SHADERMAP_DEBUG_DATA
			+ FriendlyName.GetAllocatedSize()
			+ DebugDescription.GetAllocatedSize()
#endif
			;
	}

	/** Returns the maximum number of texture samplers used by any shader in this shader map. */
	uint32 GetMaxTextureSamplers() const;

	// Accessors.
	X5_API const FMeshMaterialShaderMap* GetMeshShaderMap(FVertexFactoryType* VertexFactoryType) const;
	const FMaterialShaderMapId& GetShaderMapId() const { return ShaderMapId; }
	uint32 GetCompilingId() const { return CompilingId; }
	bool IsCompilationFinalized() const { return bCompilationFinalized; }
	bool CompiledSuccessfully() const { return bCompiledSuccessfully; }
#if ALLOW_SHADERMAP_DEBUG_DATA
	const FString& GetFriendlyName() const { return FriendlyName; }
	const FString& GetDebugDescription() const { return DebugDescription; }
#else
	const FString& GetFriendlyName() const { static FString T; return T; }
	const FString& GetDebugDescription() const { static FString T; return T; }
#endif
	bool RequiresSceneColorCopy() const { return MaterialCompilationOutput.bRequiresSceneColorCopy; }
	bool NeedsSceneTextures() const { return MaterialCompilationOutput.bNeedsSceneTextures; }
	bool UsesGlobalDistanceField() const { return MaterialCompilationOutput.bUsesGlobalDistanceField; }
	bool UsesWorldPositionOffset() const { return MaterialCompilationOutput.bUsesWorldPositionOffset; }
	bool NeedsGBuffer() const { return MaterialCompilationOutput.bNeedsGBuffer; }
	bool UsesEyeAdaptation() const { return MaterialCompilationOutput.bUsesEyeAdaptation; }
	bool ModifiesMeshPosition() const { return MaterialCompilationOutput.bModifiesMeshPosition; }
	bool UsesPixelDepthOffset() const { return MaterialCompilationOutput.bUsesPixelDepthOffset; }
	bool UsesSceneDepthLookup() const { return MaterialCompilationOutput.bUsesSceneDepthLookup; }
	uint32 GetNumUsedUVScalars() const { return MaterialCompilationOutput.NumUsedUVScalars; }
	uint32 GetNumUsedCustomInterpolatorScalars() const { return MaterialCompilationOutput.NumUsedCustomInterpolatorScalars; }
	void GetEstimatedNumTextureSamples(uint32& VSSamples, uint32& PSSamples) const { VSSamples = MaterialCompilationOutput.EstimatedNumTextureSamplesVS; PSSamples = MaterialCompilationOutput.EstimatedNumTextureSamplesPS; }

	bool IsValidForRendering(bool bFailOnInvalid = false) const
	{
		const bool bValid = bCompilationFinalized && bCompiledSuccessfully && !bDeletedThroughDeferredCleanup;
		checkf(bValid || !bFailOnInvalid, TEXT("FMaterialShaderMap %s invalid for rendering: bCompilationFinalized: %i, bCompiledSuccessfully: %i, bDeletedThroughDeferredCleanup: %i"), *GetFriendlyName(), bCompilationFinalized, bCompiledSuccessfully, bDeletedThroughDeferredCleanup ? 1 : 0);
		return bValid;
	}

	const FUniformExpressionSet& GetUniformExpressionSet() const { return MaterialCompilationOutput.UniformExpressionSet; }

	int32 GetNumRefs() const { return NumRefs; }

	void CountNumShaders(int32& NumShaders, int32& NumPipelines) const
	{
		NumShaders = GetNumShaders();
		NumPipelines = GetNumShaderPipelines();

		for (FMeshMaterialShaderMap* MeshShaderMap : OrderedMeshShaderMaps)
		{
			if (MeshShaderMap)
			{
				NumShaders += MeshShaderMap->GetNumShaders();
				NumPipelines += MeshShaderMap->GetNumShaderPipelines();
			}
		}
	}
	void DumpDebugInfo();
	void SaveShaderStableKeys(EShaderPlatform TargetShaderPlatform, const struct FStableShaderKeyAndValue& SaveKeyVal);

private:

	/**
	 * A global map from a material's static parameter set to any shader map cached for that material.
	 * Note: this does not necessarily contain all material shader maps in memory.  Shader maps with the same key can evict each other.
	 * No ref counting needed as these are removed on destruction of the shader map.
	 */
	static TMap<FMaterialShaderMapId, FMaterialShaderMap*> GIdToMaterialShaderMap[SP_NumPlatforms];

	/**
	 * All material shader maps in memory.
	 * No ref counting needed as these are removed on destruction of the shader map.
	 */
	static TArray<FMaterialShaderMap*> AllMaterialShaderMaps;

	/** The material's cached shaders for vertex factory type dependent shaders. */
	TIndirectArray<FMeshMaterialShaderMap> MeshShaderMaps;

	/** The material's mesh shader maps, indexed by VFType->GetId(), for fast lookup at runtime. */
	TArray<FMeshMaterialShaderMap*> OrderedMeshShaderMaps;

#if ALLOW_SHADERMAP_DEBUG_DATA
	/** The material's user friendly name, typically the object name. */
	FString FriendlyName;
#endif

	/** The static parameter set that this shader map was compiled with */
	FMaterialShaderMapId ShaderMapId;

	/** A map from vertex factory type to the material's cached shaders for that vertex factory type. */
	TMap<FVertexFactoryType*, FMeshMaterialShaderMap*> VertexFactoryMap;

	/** Uniform expressions generated from the material compile. */
	FMaterialCompilationOutput MaterialCompilationOutput;

	/** Next value for CompilingId. */
	static uint32 NextCompilingId;

	/** Tracks material resources and their shader maps that need to be compiled but whose compilation is being deferred. */
	static TMap<TRefCountPtr<FMaterialShaderMap>, TArray<FMaterial*> > ShaderMapsBeingCompiled;

	/** Uniquely identifies this shader map during compilation, needed for deferred compilation where shaders from multiple shader maps are compiled together. */
	uint32 CompilingId;

	mutable int32 NumRefs;

	/** Used to catch errors where the shader map is deleted directly. */
	bool bDeletedThroughDeferredCleanup;

	/** Indicates whether this shader map has been registered in GIdToMaterialShaderMap */
	uint32 bRegistered : 1;

	/**
	 * Indicates whether this shader map has had ProcessCompilationResults called after Compile.
	 * The shader map must not be used on the rendering thread unless bCompilationFinalized is true.
	 */
	uint32 bCompilationFinalized : 1;

	uint32 bCompiledSuccessfully : 1;

	/** Indicates whether the shader map should be stored in the shader cache. */
	uint32 bIsPersistent : 1;

#if ALLOW_SHADERMAP_DEBUG_DATA
	/** Debug information about how the material shader map was compiled. */
	FString DebugDescription;

#if WITH_EDITOR
	/** Map that stores source code used to compile a certain shader present in this FMaterialShaderMap instance. Key is ShaderType::TypeName. */
	TMap<FName, FString> ShaderProcessedSource;
#endif
#endif

	FShader* ProcessCompilationResultsForSingleJob(class FShaderCompileJob* SingleJob, const FShaderPipelineType* ShaderPipeline, const FSHAHash& MaterialShaderMapHash);

	bool IsMaterialShaderComplete(const FMaterial* Material, const FMaterialShaderType* ShaderType, const FShaderPipelineType* Pipeline, bool bSilent);

	/** Initializes OrderedMeshShaderMaps from the contents of MeshShaderMaps. */
	void InitOrderedMeshShaderMaps();

	friend X5_API void DumpMaterialStats(EShaderPlatform Platform);
	friend class FShaderCompilingManager;
};

/**
 * Enum that contains entries for the ways that material properties need to be compiled.
 * This 'inherits' from EMaterialProperty in the sense that all of its values start after the values in EMaterialProperty.
 * Each material property is compiled once for its usual shader frequency, determined by GetShaderFrequency(),
 * And then this enum contains entries for extra compiles of a material property with a different shader frequency.
 * This is necessary for material properties which need to be evaluated in multiple shader frequencies.
 */
enum ECompiledMaterialProperty
{
	CompiledMP_EmissiveColorCS = MP_MAX,
	CompiledMP_PrevWorldPositionOffset,
	CompiledMP_MAX
};

/**
 * Uniquely identifies a material expression output.
 * Used by the material compiler to keep track of which output it is compiling.
 */
class FMaterialExpressionKey
{
public:
	UMaterialExpression* Expression;
	int32 OutputIndex;
	/** Attribute currently being compiled through a MatterialAttributes connection. */
	FGuid MaterialAttributeID;
	// Expressions are different (e.g. View.PrevWorldViewOrigin) when using previous frame's values, value if from FHLSLMaterialTranslator::bCompilingPreviousFrame
	bool bCompilingPreviousFrameKey;

	FMaterialExpressionKey(UMaterialExpression* InExpression, int32 InOutputIndex) :
		Expression(InExpression),
		OutputIndex(InOutputIndex),
		MaterialAttributeID(FGuid(0, 0, 0, 0)),
		bCompilingPreviousFrameKey(false)
	{}

	FMaterialExpressionKey(UMaterialExpression* InExpression, int32 InOutputIndex, const FGuid& InMaterialAttributeID, bool bInCompilingPreviousFrameKey) :
		Expression(InExpression),
		OutputIndex(InOutputIndex),
		MaterialAttributeID(InMaterialAttributeID),
		bCompilingPreviousFrameKey(bInCompilingPreviousFrameKey)
	{}


	friend bool operator==(const FMaterialExpressionKey& X, const FMaterialExpressionKey& Y)
	{
		return X.Expression == Y.Expression && X.OutputIndex == Y.OutputIndex && X.MaterialAttributeID == Y.MaterialAttributeID && X.bCompilingPreviousFrameKey == Y.bCompilingPreviousFrameKey;
	}

	friend uint32 GetTypeHash(const FMaterialExpressionKey& ExpressionKey)
	{
		return PointerHash(ExpressionKey.Expression);
	}
};

/** Function specific compiler state. */
class FMaterialFunctionCompileState
{
public:
	explicit FMaterialFunctionCompileState(UMaterialExpressionMaterialFunctionCall* InFunctionCall)
		: FunctionCall(InFunctionCall)
	{}

	~FMaterialFunctionCompileState()
	{
		ClearSharedFunctionStates();
	}

	FMaterialFunctionCompileState* FindOrAddSharedFunctionState(FMaterialExpressionKey& ExpressionKey, class UMaterialExpressionMaterialFunctionCall* SharedFunctionCall)
	{
		if (FMaterialFunctionCompileState** ExistingState = SharedFunctionStates.Find(ExpressionKey))
		{
			return *ExistingState;
		}
		return SharedFunctionStates.Add(ExpressionKey, new FMaterialFunctionCompileState(SharedFunctionCall));
	}

	void ClearSharedFunctionStates()
	{
		for (auto SavedStateIt = SharedFunctionStates.CreateIterator(); SavedStateIt; ++SavedStateIt)
		{
			FMaterialFunctionCompileState* SavedState = SavedStateIt.Value();
			SavedState->ClearSharedFunctionStates();
			delete SavedState;
		}
		SharedFunctionStates.Empty();
	}

	void Reset()
	{
		ExpressionStack.Empty();
		ExpressionCodeMap.Empty();
		ClearSharedFunctionStates();
	}

	class UMaterialExpressionMaterialFunctionCall* FunctionCall;

	// Stack used to avoid re-entry within this function
	TArray<FMaterialExpressionKey> ExpressionStack;

	/** A map from material expression to the index into CodeChunks of the code for the material expression. */
	TMap<FMaterialExpressionKey, int32> ExpressionCodeMap;

private:
	/** Cache of MaterialFunctionOutput CodeChunks.  Allows for further reuse than just the ExpressionCodeMap */
	TMap<FMaterialExpressionKey, FMaterialFunctionCompileState*> SharedFunctionStates;
};

/** Returns whether the given expression class is allowed. */
//extern X5_API bool IsAllowedExpressionType(UClass* Class, bool bMaterialFunction);

/** Parses a string into multiple lines, for use with tooltips. */
extern X5_API void ConvertToMultilineToolTip(const FString& InToolTip, int32 TargetLineLength, TArray<FString>& OutToolTip);

/** Given a combination of EMaterialValueType flags, get text descriptions of all types */
extern X5_API void GetMaterialValueTypeDescriptions(uint32 MaterialValueType, TArray<FText>& OutDescriptions);

/** Check whether a combination of EMaterialValueType flags can be connected */
extern X5_API bool CanConnectMaterialValueTypes(uint32 InputType, uint32 OutputType);


/**
 * FMaterial serves 3 intertwined purposes:
 *   Represents a material to the material compilation process, and provides hooks for extensibility (CompileProperty, etc)
 *   Represents a material to the renderer, with functions to access material properties
 *   Stores a cached shader map, and other transient output from a compile, which is necessary with async shader compiling
 *      (when a material finishes async compilation, the shader map and compile errors need to be stored somewhere)
 */
class FMaterial
{
public:

	/**
	 * Minimal initialization constructor.
	 */
	FMaterial() :
		RenderingThreadShaderMap(NULL),
		Id_DEPRECATED(0, 0, 0, 0),
		QualityLevel(EMaterialQualityLevel::High),
		bHasQualityLevelUsage(false),
		FeatureLevel(ERHIFeatureLevel::SM4),
		bContainsInlineShaders(false),
		bLoadedCookedShaderMapId(false)
	{}

	/**
	 * Destructor
	 */
	X5_API virtual ~FMaterial();

	/**
	 * Caches the material shaders for this material with no static parameters on the given platform.
	 * This is used by material resources of UMaterials.
	 */
	X5_API bool CacheShaders(EShaderPlatform Platform, bool bApplyCompletedShaderMapForRendering);

	/**
	 * Caches the material shaders for the given static parameter set and platform.
	 * This is used by material resources of UMaterialInstances.
	 */
	X5_API bool CacheShaders(const FMaterialShaderMapId& ShaderMapId, EShaderPlatform Platform, bool bApplyCompletedShaderMapForRendering);

	/**
	 * Should the shader for this material with the given platform, shader type and vertex
	 * factory type combination be compiled
	 *
	 * @param Platform		The platform currently being compiled for
	 * @param ShaderType	Which shader is being compiled
	 * @param VertexFactory	Which vertex factory is being compiled (can be NULL)
	 *
	 * @return true if the shader should be compiled
	 */
	X5_API virtual bool ShouldCache(EShaderPlatform Platform, const FShaderType* ShaderType, const FVertexFactoryType* VertexFactoryType) const;

	/** Serializes the material. */
	X5_API virtual void LegacySerialize(FArchive& Ar);

	/** Serializes the shader map inline in this material, including any shader dependencies. */
	void SerializeInlineShaderMap(FArchive& Ar);

	/** Serializes the shader map inline in this material, including any shader dependencies. */
	void RegisterInlineShaderMap();

	/** Releases this material's shader map.  Must only be called on materials not exposed to the rendering thread! */
	void ReleaseShaderMap();

	/** Discards loaded shader maps if the application can't render */
	void DiscardShaderMap();

	// Material properties.
	X5_API virtual void GetShaderMapId(EShaderPlatform Platform, FMaterialShaderMapId& OutId) const;
	virtual EMaterialDomain GetMaterialDomain() const = 0; // See EMaterialDomain.
	virtual bool IsTwoSided() const = 0;
	virtual bool IsDitheredLODTransition() const = 0;
	virtual bool IsTranslucencyWritingCustomDepth() const { return false; }
	virtual bool IsTangentSpaceNormal() const { return false; }
	virtual bool ShouldInjectEmissiveIntoLPV() const { return false; }
	virtual bool ShouldBlockGI() const { return false; }
	virtual bool ShouldGenerateSphericalParticleNormals() const { return false; }
	virtual	bool ShouldDisableDepthTest() const { return false; }
	virtual	bool ShouldWriteOnlyAlpha() const { return false; }
	virtual	bool ShouldEnableResponsiveAA() const { return false; }
	virtual bool ShouldDoSSR() const { return false; }
	virtual bool ShouldDoContactShadows() const { return false; }
	virtual bool IsLightFunction() const = 0;
	virtual bool IsUsedWithEditorCompositing() const { return false; }
	virtual bool IsDeferredDecal() const = 0;
	virtual bool IsVolumetricPrimitive() const = 0;
	virtual bool IsWireframe() const = 0;
	virtual bool IsUIMaterial() const { return false; }
	virtual bool IsSpecialEngineMaterial() const = 0;
	virtual bool IsUsedWithSkeletalMesh() const { return false; }
	virtual bool IsUsedWithLandscape() const { return false; }
	virtual bool IsUsedWithParticleSystem() const { return false; }
	virtual bool IsUsedWithParticleSprites() const { return false; }
	virtual bool IsUsedWithBeamTrails() const { return false; }
	virtual bool IsUsedWithMeshParticles() const { return false; }
	virtual bool IsUsedWithNiagaraSprites() const { return false; }
	virtual bool IsUsedWithNiagaraRibbons() const { return false; }
	virtual bool IsUsedWithNiagaraMeshParticles() const { return false; }
	virtual bool IsUsedWithStaticLighting() const { return false; }
	virtual	bool IsUsedWithMorphTargets() const { return false; }
	virtual bool IsUsedWithSplineMeshes() const { return false; }
	virtual bool IsUsedWithInstancedStaticMeshes() const { return false; }
	virtual bool IsUsedWithAPEXCloth() const { return false; }
	virtual bool IsUsedWithUI() const { return false; }
	virtual bool IsUsedWithGeometryCache() const { return false; }
	X5_API virtual enum EMaterialTessellationMode GetTessellationMode() const;
	virtual bool IsCrackFreeDisplacementEnabled() const { return false; }
	virtual bool IsAdaptiveTessellationEnabled() const { return false; }
	virtual bool IsFullyRough() const { return false; }
	virtual bool UseNormalCurvatureToRoughness() const { return false; }
	virtual bool IsUsingFullPrecision() const { return false; }
	virtual bool IsUsingHQForwardReflections() const { return false; }
	virtual bool IsUsingPlanarForwardReflections() const { return false; }
	virtual bool OutputsVelocityOnBasePass() const { return true; }
	virtual bool IsNonmetal() const { return false; }
	virtual bool UseLmDirectionality() const { return true; }
	virtual bool IsMasked() const = 0;
	virtual bool IsDitherMasked() const { return false; }
	virtual bool AllowNegativeEmissiveColor() const { return false; }
	virtual enum EBlendMode GetBlendMode() const = 0;
	X5_API virtual enum ERefractionMode GetRefractionMode() const;
	virtual enum EMaterialShadingModel GetShadingModel() const = 0;
	virtual enum ETranslucencyLightingMode GetTranslucencyLightingMode() const { return TLM_VolumetricNonDirectional; };
	virtual float GetOpacityMaskClipValue() const = 0;
	virtual bool GetCastDynamicShadowAsMasked() const = 0;
	virtual bool IsDistorted() const { return false; };
	virtual float GetTranslucencyDirectionalLightingIntensity() const { return 1.0f; }
	virtual float GetTranslucentShadowDensityScale() const { return 1.0f; }
	virtual float GetTranslucentSelfShadowDensityScale() const { return 1.0f; }
	virtual float GetTranslucentSelfShadowSecondDensityScale() const { return 1.0f; }
	virtual float GetTranslucentSelfShadowSecondOpacity() const { return 1.0f; }
	virtual float GetTranslucentBackscatteringExponent() const { return 1.0f; }
	virtual bool IsTranslucencyAfterDOFEnabled() const { return false; }
	virtual bool IsMobileSeparateTranslucencyEnabled() const { return false; }
	virtual FLinearColor GetTranslucentMultipleScatteringExtinction() const { return FLinearColor::White; }
	virtual float GetTranslucentShadowStartOffset() const { return 0.0f; }
	virtual float GetRefractionDepthBiasValue() const { return 0.0f; }
	virtual float GetMaxDisplacement() const { return 0.0f; }
	virtual bool ShouldApplyFogging() const { return false; }
	virtual bool ComputeFogPerPixel() const { return false; }
	virtual FString GetFriendlyName() const = 0;
	virtual bool HasVertexPositionOffsetConnected() const { return false; }
	virtual bool HasPixelDepthOffsetConnected() const { return false; }
	virtual bool HasMaterialAttributesConnected() const { return false; }
	virtual uint32 GetDecalBlendMode() const { return 0; }
	virtual uint32 GetMaterialDecalResponse() const { return 0; }
	virtual bool HasNormalConnected() const { return false; }
	virtual bool RequiresSynchronousCompilation() const { return false; };
	virtual bool IsDefaultMaterial() const { return false; };
	virtual int32 GetNumCustomizedUVs() const { return 0; }
	virtual int32 GetBlendableLocation() const { return 0; }
	virtual bool GetBlendableOutputAlpha() const { return false; }
	/**
	 * Should shaders compiled for this material be saved to disk?
	 */
	virtual bool IsPersistent() const = 0;
	virtual UMaterialInterface* GetMaterialInterface() const { return NULL; }

	/**
	* Called when compilation of an FMaterial finishes, after the GameThreadShaderMap is set and the render command to set the RenderThreadShaderMap is queued
	*/
	virtual void NotifyCompilationFinished() { }

	/**
	* Cancels all outstanding compilation jobs for this material.
	*/
	X5_API void CancelCompilation();

	/**
	 * Blocks until compilation has completed. Returns immediately if a compilation is not outstanding.
	 */
	X5_API void FinishCompilation();

	/**
	 * Checks if the compilation for this shader is finished
	 *
	 * @return returns true if compilation is complete false otherwise
	 */
	X5_API bool IsCompilationFinished() const;

	/**
	* Checks if there is a valid GameThreadShaderMap, that is, the material can be rendered as intended.
	*
	* @return returns true if there is a GameThreadShaderMap.
	*/
	X5_API bool HasValidGameThreadShaderMap() const;

	/** Returns whether this material should be considered for casting dynamic shadows. */
	inline bool ShouldCastDynamicShadows() const
	{
		return GetShadingModel() != MSM_Unlit &&
			(GetBlendMode() == BLEND_Opaque ||
				GetBlendMode() == BLEND_Masked ||
				(GetBlendMode() == BLEND_Translucent && GetCastDynamicShadowAsMasked()));
	}


	EMaterialQualityLevel::Type GetQualityLevel() const
	{
		return QualityLevel;
	}

	// Accessors.
	const TArray<FString>& GetCompileErrors() const { return CompileErrors; }
	void SetCompileErrors(const TArray<FString>& InCompileErrors) { CompileErrors = InCompileErrors; }
	const TArray<UMaterialExpression*>& GetErrorExpressions() const { return ErrorExpressions; }
	X5_API const TArray<TRefCountPtr<FMaterialUniformExpressionTexture> >& GetUniform2DTextureExpressions() const;
	X5_API const TArray<TRefCountPtr<FMaterialUniformExpressionTexture> >& GetUniformCubeTextureExpressions() const;
	X5_API const TArray<TRefCountPtr<FMaterialUniformExpressionTexture> >& GetUniformVolumeTextureExpressions() const;
	X5_API const TArray<TRefCountPtr<FMaterialUniformExpression> >& GetUniformVectorParameterExpressions() const;
	X5_API const TArray<TRefCountPtr<FMaterialUniformExpression> >& GetUniformScalarParameterExpressions() const;
	const FGuid& GetLegacyId() const { return Id_DEPRECATED; }

	ERHIFeatureLevel::Type GetFeatureLevel() const { return FeatureLevel; }
	bool GetUsesDynamicParameter() const
	{
		//@todo - remove non-dynamic parameter particle VF and always support dynamic parameter
		return true;
	}
	X5_API bool RequiresSceneColorCopy_GameThread() const;
	X5_API bool RequiresSceneColorCopy_RenderThread() const;
	X5_API bool NeedsSceneTextures() const;
	X5_API bool NeedsGBuffer() const;
	X5_API bool UsesEyeAdaptation() const;
	X5_API bool UsesGlobalDistanceField_GameThread() const;
	X5_API bool UsesWorldPositionOffset_GameThread() const;

	/** Does the material modify the mesh position. */
	X5_API bool MaterialModifiesMeshPosition_RenderThread() const;
	X5_API bool MaterialModifiesMeshPosition_GameThread() const;

	/** Does the material use a pixel depth offset. */
	X5_API bool MaterialUsesPixelDepthOffset() const;

	/** Does the material use a SceneDepth lookup. */
	X5_API bool MaterialUsesSceneDepthLookup_RenderThread() const;
	X5_API bool MaterialUsesSceneDepthLookup_GameThread() const;

	/** Note: This function is only intended for use in deciding whether or not shader permutations are required before material translation occurs. */
	X5_API bool MaterialMayModifyMeshPosition() const;

	class FMaterialShaderMap* GetGameThreadShaderMap() const
	{
		checkSlow(IsInGameThread() /*|| IsInAsyncLoadingThread()*/);
		return GameThreadShaderMap;
	}

	/** Note: SetRenderingThreadShaderMap must also be called with the same value, but from the rendering thread. */
	void SetGameThreadShaderMap(FMaterialShaderMap* InMaterialShaderMap)
	{
		checkSlow(IsInGameThread() /*|| IsInAsyncLoadingThread()*/);
		GameThreadShaderMap = InMaterialShaderMap;
	}

	void SetInlineShaderMap(FMaterialShaderMap* InMaterialShaderMap)
	{
		checkSlow(IsInGameThread() /*|| IsInAsyncLoadingThread()*/);
		GameThreadShaderMap = InMaterialShaderMap;
		bContainsInlineShaders = true;
		bLoadedCookedShaderMapId = true;
		CookedShaderMapId = InMaterialShaderMap->GetShaderMapId();

	}

	X5_API class FMaterialShaderMap* GetRenderingThreadShaderMap() const;

	/** Note: SetGameThreadShaderMap must also be called with the same value, but from the game thread. */
	X5_API void SetRenderingThreadShaderMap(FMaterialShaderMap* InMaterialShaderMap);

	void RemoveOutstandingCompileId(const int32 OldOutstandingCompileShaderMapId)
	{
		OutstandingCompileShaderMapIds.Remove(OldOutstandingCompileShaderMapId);
	}

	//X5_API virtual void AddReferencedObjects(FReferenceCollector& Collector);

	virtual const TArray<UTexture*>& GetReferencedTextures() const = 0;

	/**
	 * Finds the shader matching the template type and the passed in vertex factory, asserts if not found.
	 * Note - Only implemented for FMeshMaterialShaderTypes
	 */
	template<typename ShaderType>
	ShaderType* GetShader(FVertexFactoryType* VertexFactoryType, bool bFatalIfMissing = true) const
	{
		return (ShaderType*)GetShader(&ShaderType::StaticType, VertexFactoryType, bFatalIfMissing);
	}

	X5_API FShaderPipeline* GetShaderPipeline(class FShaderPipelineType* ShaderPipelineType, FVertexFactoryType* VertexFactoryType, bool bFatalIfNotFound = true) const;

	/** Returns a string that describes the material's usage for debugging purposes. */
	virtual FString GetMaterialUsageDescription() const = 0;

	/** Returns true if this material is allowed to make development shaders via the global CVar CompileShadersForDevelopment. */
	virtual bool GetAllowDevelopmentShaderCompile()const { return true; }

	/** Returns which shadermap this material is bound to. */
	virtual EMaterialShaderMapUsage::Type GetMaterialShaderMapUsage() const { return EMaterialShaderMapUsage::Default; }

	/**
	* Get user source code for the material, with a list of code snippets to highlight representing the code for each MaterialExpression
	* @param OutSource - generated source code
	* @param OutHighlightMap - source code highlight list
	* @return - true on Success
	*/
	X5_API bool GetMaterialExpressionSource(FString& OutSource);

	/* Helper function to look at both IsMasked and IsDitheredLODTransition to determine if it writes every pixel */
	X5_API bool WritesEveryPixel(bool bShadowPass = false) const
	{
		static TConsoleVariableData<int32>* CVarStencilDitheredLOD =
			IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.StencilForLODDither"));

		return !IsMasked()
			// Render dithered material as masked if a stencil prepass is not used (UE-50064, UE-49537)
			&& !((bShadowPass || !CVarStencilDitheredLOD->GetValueOnAnyThread()) && IsDitheredLODTransition())
			&& !IsWireframe();
	}

	/** call during shader compilation jobs setup to fill additional settings that may be required by classes who inherit from this */
	virtual void SetupExtaCompilationSettings(const EShaderPlatform Platform, FExtraShaderCompilerSettings& Settings) const
	{}

	void DumpDebugInfo();
	void SaveShaderStableKeys(EShaderPlatform TargetShaderPlatform, struct FStableShaderKeyAndValue& SaveKeyVal); // arg is non-const, we modify it as we go

	/**
	 * Adds an FMaterial to the global list.
	 * Any FMaterials that don't belong to a UMaterialInterface need to be registered in this way to work correctly with runtime recompiling of outdated shaders.
	 */
	static void AddEditorLoadedMaterialResource(FMaterial* Material)
	{
		EditorLoadedMaterialResources.Add(Material);
	}

	/** Recompiles any materials in the EditorLoadedMaterialResources list if they are not complete. */
	static void UpdateEditorLoadedMaterialResources(EShaderPlatform InShaderPlatform);

	/** Backs up any FShaders in editor loaded materials to memory through serialization and clears FShader references. */
	static void BackupEditorLoadedMaterialShadersToMemory(TMap<FMaterialShaderMap*, TUniquePtr<TArray<uint8> > >& ShaderMapToSerializedShaderData);
	/** Recreates FShaders in editor loaded materials from the passed in memory, handling shader key changes. */
	static void RestoreEditorLoadedMaterialShadersFromMemory(const TMap<FMaterialShaderMap*, TUniquePtr<TArray<uint8> > >& ShaderMapToSerializedShaderData);

protected:

	// shared code needed for GetUniformScalarParameterExpressions, GetUniformVectorParameterExpressions, GetUniformCubeTextureExpressions..
	// @return can be 0
	const FMaterialShaderMap* GetShaderMapToUse() const;

	/**
	* Fills the passed array with IDs of shader maps unfinished compilation jobs.
	*/
	void GetShaderMapIDsWithUnfinishedCompilation(TArray<int32>& ShaderMapIds);

	/**
	 * Entry point for compiling a specific material property.  This must call SetMaterialProperty.
	 * @param OverrideShaderFrequency SF_NumFrequencies to not override
	 * @return cases to the proper type e.g. Compiler->ForceCast(Ret, GetValueType(Property));
	 */
	virtual int32 CompilePropertyAndSetMaterialProperty(EMaterialProperty Property, class FMaterialCompiler* Compiler, EShaderFrequency OverrideShaderFrequency = SF_NumFrequencies, bool bUsePreviousFrameTime = false) const = 0;

#if HANDLE_CUSTOM_OUTPUTS_AS_MATERIAL_ATTRIBUTES
	/** Used to translate code for custom output attributes such as ClearCoatBottomNormal  */
	virtual int32 CompileCustomAttribute(const FGuid& AttributeID, class FMaterialCompiler* Compiler) const { return INDEX_NONE; }
#endif

	/* Gather any UMaterialExpressionCustomOutput expressions they can be compiled in turn */
	virtual void GatherCustomOutputExpressions(TArray<class UMaterialExpressionCustomOutput*>& OutCustomOutputs) const {}

	/* Gather any UMaterialExpressionCustomOutput expressions in the material and referenced function calls */
	virtual void GatherExpressionsForCustomInterpolators(TArray<class UMaterialExpression*>& OutExpressions) const {}

	/** Returns the index to the Expression in the Expressions array, or -1 if not found. */
	int32 FindExpression(const TArray<TRefCountPtr<FMaterialUniformExpressionTexture> >&Expressions, const FMaterialUniformExpressionTexture &Expression);

	/** Useful for debugging. */
	virtual FString GetBaseMaterialPathName() const { return TEXT(""); }
	virtual FString GetDebugName() const { return GetBaseMaterialPathName(); }

	void SetQualityLevelProperties(EMaterialQualityLevel::Type InQualityLevel, bool bInHasQualityLevelUsage, ERHIFeatureLevel::Type InFeatureLevel)
	{
		QualityLevel = InQualityLevel;
		bHasQualityLevelUsage = bInHasQualityLevelUsage;
		FeatureLevel = InFeatureLevel;
	}

	/**
	 * Gets the shader map usage of the material, which will be included in the DDC key.
	 * This mechanism allows derived material classes to create different DDC keys with the same base material.
	 * For example lightmass exports diffuse and emissive, each of which requires a material resource with the same base material.
	 */
	virtual EMaterialShaderMapUsage::Type GetShaderMapUsage() const { return EMaterialShaderMapUsage::Default; }

	/** Gets the Guid that represents this material. */
	virtual FGuid GetMaterialId() const = 0;

	/** Produces arrays of any shader and vertex factory type that this material is dependent on. */
	X5_API void GetDependentShaderAndVFTypes(EShaderPlatform Platform, TArray<FShaderType*>& OutShaderTypes, TArray<const FShaderPipelineType*>& OutShaderPipelineTypes, TArray<FVertexFactoryType*>& OutVFTypes) const;

	bool GetLoadedCookedShaderMapId() const { return bLoadedCookedShaderMapId; }

private:

	/**
	 * Tracks FMaterials without a corresponding UMaterialInterface in the editor, for example FExpressionPreviews.
	 * Used to handle the 'recompileshaders changed' command in the editor.
	 * This doesn't have to use a reference counted pointer because materials are removed on destruction.
	 */
	X5_API static TSet<FMaterial*> EditorLoadedMaterialResources;

	TArray<FString> CompileErrors;

	/** List of material expressions which generated a compiler error during the last compile. */
	TArray<UMaterialExpression*> ErrorExpressions;

	/**
	 * Game thread tracked shader map, which is ref counted and manages shader map lifetime.
	 * The shader map uses deferred deletion so that the rendering thread has a chance to process a release command when the shader map is no longer referenced.
	 * Code that sets this is responsible for updating RenderingThreadShaderMap in a thread safe way.
	 * During an async compile, this will be NULL and will not contain the actual shader map until compilation is complete.
	 */
	TRefCountPtr<FMaterialShaderMap> GameThreadShaderMap;

	/**
	 * Shader map for this material resource which is accessible by the rendering thread.
	 * This must be updated along with GameThreadShaderMap, but on the rendering thread.
	 */
	FMaterialShaderMap* RenderingThreadShaderMap;

	/**
	 * Legacy unique identifier of this material resource.
	 * This functionality is now provided by UMaterial::StateId.
	 */
	FGuid Id_DEPRECATED;

	/**
	 * Contains the compiling id of this shader map when it is being compiled asynchronously.
	 * This can be used to access the shader map during async compiling, since GameThreadShaderMap will not have been set yet.
	 */
	TArray<int32, TInlineAllocator<1> > OutstandingCompileShaderMapIds;

	/** Quality level that this material is representing. */
	EMaterialQualityLevel::Type QualityLevel;

	/** Whether this material has quality level specific nodes. */
	bool bHasQualityLevelUsage;

	/** Feature level that this material is representing. */
	ERHIFeatureLevel::Type FeatureLevel;

	/**
	 * Whether this material was loaded with shaders inlined.
	 * If true, GameThreadShaderMap will contain a reference to the inlined shader map between Serialize and PostLoad.
	 */
	uint32 bContainsInlineShaders : 1;
	uint32 bLoadedCookedShaderMapId : 1;

	FMaterialShaderMapId CookedShaderMapId;

	/**
	* Compiles this material for Platform, storing the result in OutShaderMap if the compile was synchronous
	*/
	bool BeginCompileShaderMap(
		const FMaterialShaderMapId& ShaderMapId,
		EShaderPlatform Platform,
		TRefCountPtr<class FMaterialShaderMap>& OutShaderMap,
		bool bApplyCompletedShaderMapForRendering);

	/** Populates OutEnvironment with defines needed to compile shaders for this material. */
	void SetupMaterialEnvironment(
		EShaderPlatform Platform,
		const FUniformExpressionSet& InUniformExpressionSet,
		FShaderCompilerEnvironment& OutEnvironment
	) const;

	/**
	 * Finds the shader matching the template type and the passed in vertex factory, asserts if not found.
	 */
	X5_API FShader* GetShader(class FMeshMaterialShaderType* ShaderType, FVertexFactoryType* VertexFactoryType, bool bFatalIfMissing = true) const;

	void GetReferencedTexturesHash(EShaderPlatform Platform, FSHAHash& OutHash) const;

	EMaterialQualityLevel::Type GetQualityLevelForShaderMapId() const
	{
		return bHasQualityLevelUsage ? QualityLevel : EMaterialQualityLevel::Num;
	}

	friend class FMaterialShaderMap;
	friend class FShaderCompilingManager;
	friend class FHLSLMaterialTranslator;
};

/**
 * Cached uniform expression values.
 */
struct FUniformExpressionCache
{
	/** Material uniform buffer. */
	FUniformBufferRHIRef UniformBuffer;
	/** Material uniform buffer. */
	FLocalUniformBuffer LocalUniformBuffer;
	/** Ids of parameter collections needed for rendering. */
	TArray<FGuid> ParameterCollections;
	/** True if the cache is up to date. */
	bool bUpToDate;

	/** Shader map that was used to cache uniform expressions on this material.  This is used for debugging and verifying correct behavior. */
	const FMaterialShaderMap* CachedUniformExpressionShaderMap;

	FUniformExpressionCache() :
		bUpToDate(false),
		CachedUniformExpressionShaderMap(NULL)
	{}

	/** Destructor. */
	~FUniformExpressionCache()
	{
		UniformBuffer.SafeRelease();
	}
};

class USubsurfaceProfile;