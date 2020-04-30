#pragma once

#include "CoreMinimal.h"
#include "Templates/RefCounting.h"
#include "Misc/MemStack.h"
#include "Modules/ModuleInterface.h"
#include "RHI/RHI.h"
#include "RenderResource.h"
#include "RenderUtils.h"
#include "Misc/EnumClassFlags.h"
#include "UniformBuffer.h"

class FCanvas;
class FMaterial;
class FSceneInterface;
class FSceneRenderTargets;
class FSceneView;
class FSceneViewFamily;
class FSceneTexturesUniformParameters;
struct FMeshBatch;
struct FSynthBenchmarkResults;

enum class EShowMaterialDrawEventTypes
{
	None						= 0 << 0,  //0
	CompositionLighting			= 1 << 0,  //1
	BasePass					= 1 << 1,  //2
	DepthPositionOnly			= 1 << 2,  //4
	Depth						= 1 << 3,  //8
	DistortionDynamic			= 1 << 4,  //16
	DistortionStatic			= 1 << 5,  //32
	MobileBasePass				= 1 << 6,  //64
	MobileTranslucent			= 1 << 7,  //128
	MobileTranslucentOpacity	= 1 << 8,  //256
	ShadowDepth					= 1 << 9,  //512
	ShadowDepthRsm				= 1 << 10, //1024
	ShadowDepthStatic			= 1 << 11, //2048
	StaticDraw					= 1 << 12, //4096
	StaticDrawStereo			= 1 << 13, //8192
	TranslucentLighting			= 1 << 14, //16384
	Translucent					= 1 << 15, //32768
	Velocity					= 1 << 16, //65536
	FogVoxelization				= 1 << 17, //131072
};
ENUM_CLASS_FLAGS(EShowMaterialDrawEventTypes)

// Shortcut for the allocator used by scene rendering.
typedef TMemStackAllocator<> SceneRenderingAllocator;

/** All necessary data to create a render target from the pooled render targets. */
struct FPooledRenderTargetDesc
{
public:

	/** Default constructor, use one of the factory functions below to make a valid description */
	FPooledRenderTargetDesc()
		: ClearValue(FClearValueBinding())
		, Extent(0, 0)
		, Depth(0)
		, ArraySize(1)
		, bIsArray(false)
		, bIsCubemap(false)
		, NumMips(0)
		, NumSamples(1)
		, Format(PF_Unknown)
		, Flags(TexCreate_None)
		, TargetableFlags(TexCreate_None)
		, bForceSeparateTargetAndShaderResource(false)
		, DebugName(TEXT("UnknownTexture"))
		, AutoWritable(true)
		, bCreateRenderTargetWriteMask(false)
	{
		check(!IsValid());
	}

	/**
	 * Factory function to create 2D texture description
	 * @param InFlags bit mask combined from elements of ETextureCreateFlags e.g. TexCreate_UAV
	 */
	static FPooledRenderTargetDesc Create2DDesc(
		FIntPoint InExtent,
		EPixelFormat InFormat,
		const FClearValueBinding& InClearValue,
		uint32 InFlags,
		uint32 InTargetableFlags,
		bool bInForceSeparateTargetAndShaderResource,
		uint16 InNumMips = 1,
		bool InAutowritable = true,
		bool InCreateRTWriteMask = false)
	{
		check(InExtent.X);
		check(InExtent.Y);

		FPooledRenderTargetDesc NewDesc;
		NewDesc.ClearValue = InClearValue;
		NewDesc.Extent = InExtent;
		NewDesc.Depth = 0;
		NewDesc.ArraySize = 1;
		NewDesc.bIsArray = false;
		NewDesc.bIsCubemap = false;
		NewDesc.NumMips = InNumMips;
		NewDesc.NumSamples = 1;
		NewDesc.Format = InFormat;
		NewDesc.Flags = InFlags;
		NewDesc.TargetableFlags = InTargetableFlags;
		NewDesc.bForceSeparateTargetAndShaderResource = bInForceSeparateTargetAndShaderResource;
		NewDesc.DebugName = TEXT("UnknownTexture2D");
		NewDesc.AutoWritable = InAutowritable;
		NewDesc.bCreateRenderTargetWriteMask = InCreateRTWriteMask;
		check(NewDesc.Is2DTexture());
		return NewDesc;
	}

	/**
	 * Factory function to create 3D texture description
	 * @param InFlags bit mask combined from elements of ETextureCreateFlags e.g. TexCreate_UAV
	 */
	static FPooledRenderTargetDesc CreateVolumeDesc(
		uint32 InSizeX,
		uint32 InSizeY,
		uint32 InSizeZ,
		EPixelFormat InFormat,
		const FClearValueBinding& InClearValue,
		uint32 InFlags,
		uint32 InTargetableFlags,
		bool bInForceSeparateTargetAndShaderResource,
		uint16 InNumMips = 1,
		bool InAutowritable = true)
	{
		check(InSizeX);
		check(InSizeY);

		FPooledRenderTargetDesc NewDesc;
		NewDesc.ClearValue = InClearValue;
		NewDesc.Extent = FIntPoint(InSizeX, InSizeY);
		NewDesc.Depth = InSizeZ;
		NewDesc.ArraySize = 1;
		NewDesc.bIsArray = false;
		NewDesc.bIsCubemap = false;
		NewDesc.NumMips = InNumMips;
		NewDesc.NumSamples = 1;
		NewDesc.Format = InFormat;
		NewDesc.Flags = InFlags;
		NewDesc.TargetableFlags = InTargetableFlags;
		NewDesc.bForceSeparateTargetAndShaderResource = bInForceSeparateTargetAndShaderResource;
		NewDesc.DebugName = TEXT("UnknownTextureVolume");
		NewDesc.AutoWritable = InAutowritable;
		check(NewDesc.Is3DTexture());
		return NewDesc;
	}

	/**
	 * Factory function to create cube map texture description
	 * @param InFlags bit mask combined from elements of ETextureCreateFlags e.g. TexCreate_UAV
	 */
	static FPooledRenderTargetDesc CreateCubemapDesc(
		uint32 InExtent,
		EPixelFormat InFormat,
		const FClearValueBinding& InClearValue,
		uint32 InFlags,
		uint32 InTargetableFlags,
		bool bInForceSeparateTargetAndShaderResource,
		uint32 InArraySize = 1,
		uint16 InNumMips = 1,
		bool InAutowritable = true)
	{
		check(InExtent);

		FPooledRenderTargetDesc NewDesc;
		NewDesc.ClearValue = InClearValue;
		NewDesc.Extent = FIntPoint(InExtent, InExtent);
		NewDesc.Depth = 0;
		NewDesc.ArraySize = InArraySize;
		// Note: this doesn't allow an array of size 1
		NewDesc.bIsArray = InArraySize > 1;
		NewDesc.bIsCubemap = true;
		NewDesc.NumMips = InNumMips;
		NewDesc.NumSamples = 1;
		NewDesc.Format = InFormat;
		NewDesc.Flags = InFlags;
		NewDesc.TargetableFlags = InTargetableFlags;
		NewDesc.bForceSeparateTargetAndShaderResource = bInForceSeparateTargetAndShaderResource;
		NewDesc.DebugName = TEXT("UnknownTextureCube");
		NewDesc.AutoWritable = InAutowritable;
		check(NewDesc.IsCubemap());

		return NewDesc;
	}

	/** Comparison operator to test if a render target can be reused */
	bool Compare(const FPooledRenderTargetDesc& rhs, bool bExact) const
	{
		auto LhsFlags = Flags;
		auto RhsFlags = rhs.Flags;

		if (!bExact || !FPlatformMemory::SupportsFastVRAMMemory())
		{
			LhsFlags &= (~TexCreate_FastVRAM);
			RhsFlags &= (~TexCreate_FastVRAM);
		}

		return Extent == rhs.Extent
			&& Depth == rhs.Depth
			&& bIsArray == rhs.bIsArray
			&& bIsCubemap == rhs.bIsCubemap
			&& ArraySize == rhs.ArraySize
			&& NumMips == rhs.NumMips
			&& NumSamples == rhs.NumSamples
			&& Format == rhs.Format
			&& LhsFlags == RhsFlags
			&& TargetableFlags == rhs.TargetableFlags
			&& bForceSeparateTargetAndShaderResource == rhs.bForceSeparateTargetAndShaderResource
			&& ClearValue == rhs.ClearValue
			&& AutoWritable == rhs.AutoWritable;
	}

	bool IsCubemap() const
	{
		return bIsCubemap;
	}

	bool Is2DTexture() const
	{
		return Extent.X != 0 && Extent.Y != 0 && Depth == 0 && !bIsCubemap;
	}

	bool Is3DTexture() const
	{
		return Extent.X != 0 && Extent.Y != 0 && Depth != 0 && !bIsCubemap;
	}

	// @return true if this texture is a texture array
	bool IsArray() const
	{
		return bIsArray;
	}

	bool IsValid() const
	{
		if (NumSamples != 1)
		{
			if (NumSamples < 1 || NumSamples > 8)
			{
				// D3D11 limitations
				return false;
			}

			if (!Is2DTexture())
			{
				return false;
			}
		}

		return Extent.X != 0 && NumMips != 0 && NumSamples >= 1 && NumSamples <= 16 && Format != PF_Unknown
			&& ((TargetableFlags & TexCreate_UAV) == 0 || GMaxRHIFeatureLevel == ERHIFeatureLevel::SM5);
	}

	FIntVector GetSize() const
	{
		return FIntVector(Extent.X, Extent.Y, Depth);
	}

	/**
	 * for debugging purpose
	 * @return e.g. (2D 128x64 PF_R8)
	 */
	FString GenerateInfoString() const
	{
		const TCHAR* FormatString = GetPixelFormatString(Format);

		FString FlagsString = TEXT("");

		uint32 LocalFlags = Flags | TargetableFlags;

		if (LocalFlags & TexCreate_RenderTargetable)
		{
			FlagsString += TEXT(" RT");
		}
		if (LocalFlags & TexCreate_SRGB)
		{
			FlagsString += TEXT(" sRGB");
		}
		if (NumSamples > 1)
		{
			FlagsString += FString::Printf(TEXT(" %dxMSAA"), NumSamples);
		}
		if (LocalFlags & TexCreate_UAV)
		{
			FlagsString += TEXT(" UAV");
		}

		if (LocalFlags & TexCreate_FastVRAM)
		{
			FlagsString += TEXT(" VRam");
		}

		if (LocalFlags & TexCreate_Transient)
		{
			FlagsString += TEXT(" Transient");
		}

		FString ArrayString;

		if (IsArray())
		{
			ArrayString = FString::Printf(TEXT("[%d]"), ArraySize);
		}

		if (Is2DTexture())
		{
			return FString::Printf(TEXT("(2D%s %dx%d %s%s)"), *ArrayString, Extent.X, Extent.Y, FormatString, *FlagsString);
		}
		else if (Is3DTexture())
		{
			return FString::Printf(TEXT("(3D%s %dx%dx%d %s%s)"), *ArrayString, Extent.X, Extent.Y, Depth, FormatString, *FlagsString);
		}
		else if (IsCubemap())
		{
			return FString::Printf(TEXT("(Cube%s %d %s%s)"), *ArrayString, Extent.X, FormatString, *FlagsString);
		}
		else
		{
			return TEXT("(INVALID)");
		}
	}

	// useful when compositing graph takes an input as output format
	void Reset()
	{
		// Usually we don't want to propagate MSAA samples.
		NumSamples = 1;

		bForceSeparateTargetAndShaderResource = false;
		AutoWritable = true;

		// Remove UAV flag for rendertargets that don't need it (some formats are incompatible)
		TargetableFlags |= TexCreate_RenderTargetable;
		TargetableFlags &= (~TexCreate_UAV);
	}

	/** Value allowed for fast clears for this target. */
	FClearValueBinding ClearValue;

	/** In pixels, (0,0) if not set, (x,0) for cube maps, todo: make 3d int vector for volume textures */
	FIntPoint Extent;
	/** 0, unless it's texture array or volume texture */
	uint32 Depth;

	/** >1 if a texture array should be used (not supported on DX9) */
	uint32 ArraySize;
	/** true if an array texture. Note that ArraySize still can be 1 */
	bool bIsArray;
	/** true if a cubemap texture */
	bool bIsCubemap;
	/** Number of mips */
	uint16 NumMips;
	/** Number of MSAA samples, default: 1  */
	uint16 NumSamples;
	/** Texture format e.g. PF_B8G8R8A8 */
	EPixelFormat Format;
	/** The flags that must be set on both the shader-resource and the targetable texture. bit mask combined from elements of ETextureCreateFlags e.g. TexCreate_UAV */
	uint32 Flags;
	/** The flags that must be set on the targetable texture. bit mask combined from elements of ETextureCreateFlags e.g. TexCreate_UAV */
	uint32 TargetableFlags;
	/** Whether the shader-resource and targetable texture must be separate textures. */
	bool bForceSeparateTargetAndShaderResource;
	/** only set a pointer to memory that never gets released */
	const TCHAR* DebugName;
	/** automatically set to writable via barrier during */
	bool AutoWritable;
	/** create render target write mask (supported only on specific platforms) */
	bool bCreateRenderTargetWriteMask;
};

/**
 * Single render target item consists of a render surface and its resolve texture, Render thread side
 */
struct FSceneRenderTargetItem
{
	/** default constructor */
	FSceneRenderTargetItem() {}

	/** constructor */
	FSceneRenderTargetItem(FTextureRHIParamRef InTargetableTexture, FTextureRHIParamRef InShaderResourceTexture, FUnorderedAccessViewRHIRef InUAV)
		: TargetableTexture(InTargetableTexture)
		, ShaderResourceTexture(InShaderResourceTexture)
		, UAV(InUAV)
	{}

	/** */
	void SafeRelease()
	{
		TargetableTexture.SafeRelease();
		ShaderResourceTexture.SafeRelease();
		UAV.SafeRelease();
		for (int32 i = 0; i < MipUAVs.Num(); i++)
		{
			MipUAVs[i].SafeRelease();
		}
		for (int32 i = 0; i < MipSRVs.Num(); i++)
		{
			MipSRVs[i].SafeRelease();
		}
	}

	bool IsValid() const
	{
		return TargetableTexture != 0
			|| ShaderResourceTexture != 0
			|| UAV != 0;
	}

	/** The 2D or cubemap texture that may be used as a render or depth-stencil target. */
	FTextureRHIRef TargetableTexture;
	/** The 2D or cubemap shader-resource 2D texture that the targetable textures may be resolved to. */
	FTextureRHIRef ShaderResourceTexture;
	/** only created if requested through the flag, same as MipUAVs[0] */
	// TODO: refactor all the code to only use MipUAVs?
	FUnorderedAccessViewRHIRef UAV;
	/** only created if requested through the flag  */
	TArray< FUnorderedAccessViewRHIRef, TInlineAllocator<1> > MipUAVs;
	/** only created if requested through the flag  */
	TArray< FShaderResourceViewRHIRef > MipSRVs;

	FShaderResourceViewRHIRef RTWriteMaskBufferRHI_SRV;
	FStructuredBufferRHIRef RTWriteMaskDataBufferRHI;
};

/**
 * Render thread side, use TRefCountPtr<IPooledRenderTarget>, allows sharing and VisualizeTexture
 */
struct IPooledRenderTarget
{
	virtual ~IPooledRenderTarget() {}

	/** Checks if the reference count indicated that the rendertarget is unused and can be reused. */
	virtual bool IsFree() const = 0;
	/** Get all the data that is needed to create the render target. */
	virtual const FPooledRenderTargetDesc& GetDesc() const = 0;
	/** @param InName must not be 0 */
	virtual void SetDebugName(const TCHAR *InName) = 0;
	/**
	 * Only for debugging purpose
	 * @return in bytes
	 **/
	virtual uint32 ComputeMemorySize() const = 0;
	/** Get the low level internals (texture/surface) */
	inline FSceneRenderTargetItem& GetRenderTargetItem() { return RenderTargetItem; }
	/** Get the low level internals (texture/surface) */
	inline const FSceneRenderTargetItem& GetRenderTargetItem() const { return RenderTargetItem; }

	// Refcounting
	virtual uint32 AddRef() const = 0;
	virtual uint32 Release() = 0;
	virtual uint32 GetRefCount() const = 0;

protected:

	/** The internal references to the created render target */
	FSceneRenderTargetItem RenderTargetItem;
};

struct FQueryVisualizeTexureInfo
{
	TArray<FString> Entries;
};

/** The vertex data used to filter a texture. */
struct FFilterVertex
{
	FVector4 Position;
	FVector2D UV;
};

/** The filter vertex declaration resource type. */
class FFilterVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	/** Destructor. */
	virtual ~FFilterVertexDeclaration() {}

	virtual void InitRHI()
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FFilterVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FFilterVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FFilterVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI()
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

/** The empty vertex declaration resource type. */
class FEmptyVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	/** Destructor. */
	virtual ~FEmptyVertexDeclaration() {}

	virtual void InitRHI()
	{
		FVertexDeclarationElementList Elements;
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI()
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

// use r.DrawDenormalizedQuadMode to override the function call setting (quick way to see if an artifact is caused by this optimization)
enum EDrawRectangleFlags
{
	// Rectangle is created by 2 triangles (diagonal can cause some slightly less efficient shader execution), this is the default as it has no artifacts
	EDRF_Default,
	//
	EDRF_UseTriangleOptimization,
	//
	EDRF_UseTesselatedIndexBuffer
};

class FPostOpaqueRenderParameters
{
public:
	FIntRect ViewportRect;
	FMatrix ViewMatrix;
	FMatrix ProjMatrix;
	FRHITexture2D* DepthTexture;
	FRHITexture2D* NormalTexture;
	FRHITexture2D* SmallDepthTexture;
	FRHICommandListImmediate* RHICmdList;
	FUniformBufferRHIParamRef ViewUniformBuffer;
	TUniformBufferRef<FSceneTexturesUniformParameters> SceneTexturesUniformParams;
	void* Uid; // A unique identifier for the view.
};
//DECLARE_DELEGATE_OneParam(FPostOpaqueRenderDelegate, class FPostOpaqueRenderParameters&);

class IRendererModule : public IModuleInterface
{
public:
	virtual FSceneInterface* AllocateScene(class UWorld* World) = 0;
	virtual void RemoveScene(FSceneInterface* Scene) = 0;
};