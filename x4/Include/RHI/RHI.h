#pragma once

#include "CoreMinimal.h"
#include "RHIDefinitions.h"

#include "X4.h"
/**
 * RHI globals.
 */

 /** True if the render hardware has been initialized. */
extern X4_API bool GIsRHIInitialized;

class X4_API FRHICommandList;

/**
 * RHI capabilities.
 */


 /** The maximum number of mip-maps that a texture can contain. 	*/
extern	X4_API int32		GMaxTextureMipCount;

/** true if this platform has quad buffer stereo support. */
extern X4_API bool GSupportsQuadBufferStereo;

/** true if the RHI supports textures that may be bound as both a render target and a shader resource. */
extern X4_API bool GSupportsRenderDepthTargetableShaderResources;

/** true if the RHI supports binding depth as a texture when testing against depth */
extern X4_API bool GSupportsDepthFetchDuringDepthTest;


/** true if the RHI supports SRVs */
extern X4_API bool GSupportsResourceView;

/**
 * only set if RHI has the information (after init of the RHI and only if RHI has that information, never changes after that)
 * e.g. "NVIDIA GeForce GTX 670"
 */
extern X4_API FString GRHIAdapterName;
extern X4_API FString GRHIAdapterInternalDriverVersion;
extern X4_API FString GRHIAdapterUserDriverVersion;
extern X4_API FString GRHIAdapterDriverDate;
extern X4_API uint32 GRHIDeviceId;
extern X4_API uint32 GRHIDeviceRevision;

// 0 means not defined yet, use functions like IsRHIDeviceAMD() to access
extern X4_API uint32 GRHIVendorId;


// to trigger GPU specific optimizations and fallbacks
X4_API bool IsRHIDeviceAMD();

// to trigger GPU specific optimizations and fallbacks
X4_API bool IsRHIDeviceIntel();

// to trigger GPU specific optimizations and fallbacks
X4_API bool IsRHIDeviceNVIDIA();

// helper to convert GRHIVendorId into a printable string, or "Unknown" if unknown.
X4_API const TCHAR* RHIVendorIdToString();

// helper to return the shader language version for the given shader platform.
X4_API uint32 RHIGetShaderLanguageVersion(const EShaderPlatform Platform);

// helper to check that the shader platform supports tessellation.
X4_API bool RHISupportsTessellation(const EShaderPlatform Platform);

// helper to check that the shader platform supports writing to UAVs from pixel shaders.
X4_API bool RHISupportsPixelShaderUAVs(const EShaderPlatform Platform);

// helper to check that the shader platform supports creating a UAV off an index buffer.
X4_API bool RHISupportsIndexBufferUAVs(const EShaderPlatform Platform);

// helper to check if a preview feature level has been requested.
//X4_API bool RHIGetPreviewFeatureLevel(ERHIFeatureLevel::Type& PreviewFeatureLevelOUT);


template <typename TValueType>
class TRHIGlobal
{
public:
	explicit TRHIGlobal(const TValueType& InValue) : Value(InValue) {}

	TRHIGlobal& operator=(const TValueType& InValue)
	{
		Value = InValue;
		return *this;
	}

	inline operator TValueType() const { return Value; }
private:
	TValueType Value;
};

/** true if the GPU is AMD's Pre-GCN architecture */
extern X4_API bool GRHIDeviceIsAMDPreGCNArchitecture;

/** true if PF_G8 render targets are supported */
extern X4_API TRHIGlobal<bool> GSupportsRenderTargetFormat_PF_G8;

/** true if PF_FloatRGBA render targets are supported */
extern X4_API TRHIGlobal<bool> GSupportsRenderTargetFormat_PF_FloatRGBA;

/** true if mobile framebuffer fetch is supported */
extern X4_API bool GSupportsShaderFramebufferFetch;

/** true if mobile depth & stencil fetch is supported */
extern X4_API bool GSupportsShaderDepthStencilFetch;

/** true if RQT_AbsoluteTime is supported by RHICreateRenderQuery */
extern X4_API bool GSupportsTimestampRenderQueries;

/** true if RQT_AbsoluteTime is supported by RHICreateRenderQuery */
extern X4_API bool GRHISupportsGPUTimestampBubblesRemoval;

/** true if RHIGetGPUFrameCycles removes CPu generated bubbles. */
extern X4_API bool GRHISupportsFrameCyclesBubblesRemoval;

/** true if the GPU supports hidden surface removal in hardware. */
extern X4_API bool GHardwareHiddenSurfaceRemoval;

/** true if the RHI supports asynchronous creation of texture resources */
extern X4_API bool GRHISupportsAsyncTextureCreation;

/** true if the RHI supports quad topology (PT_QuadList). */
extern X4_API bool GRHISupportsQuadTopology;

/** true if the RHI supports rectangular topology (PT_RectList). */
extern X4_API bool GRHISupportsRectTopology;

/** Temporary. When OpenGL is running in a separate thread, it cannot yet do things like initialize shaders that are first discovered in a rendering task. It is doable, it just isn't done. */
extern X4_API bool GSupportsParallelRenderingTasksWithSeparateRHIThread;

/** If an RHI is so slow, that it is the limiting factor for the entire frame, we can kick early to try to give it as much as possible. */
extern X4_API bool GRHIThreadNeedsKicking;

/** If an RHI cannot do an unlimited number of occlusion queries without stalling and waiting for the GPU, this can be used to tune hte occlusion culler to try not to do that. */
extern X4_API int32 GRHIMaximumReccommendedOustandingOcclusionQueries;

/** Some RHIs can only do visible or not occlusion queries. */
extern X4_API bool GRHISupportsExactOcclusionQueries;

/** True if and only if the GPU support rendering to volume textures (2D Array, 3D). Some OpenGL 3.3 cards support SM4, but can't render to volume textures. */
extern X4_API bool GSupportsVolumeTextureRendering;

/** True if the RHI supports separate blend states per render target. */
extern X4_API bool GSupportsSeparateRenderTargetBlendState;

/** True if the RHI can render to a depth-only render target with no additional color render target. */
extern X4_API bool GSupportsDepthRenderTargetWithoutColorRenderTarget;

/** True if the RHI has artifacts with atlased CSM depths. */
extern X4_API bool GRHINeedsUnatlasedCSMDepthsWorkaround;

/** true if the RHI supports 3D textures */
extern X4_API bool GSupportsTexture3D;

/** true if the RHI supports mobile multi-view */
extern X4_API bool GSupportsMobileMultiView;

/** true if the RHI supports image external */
extern X4_API bool GSupportsImageExternal;

/** true if the RHI supports MRT */
extern X4_API TRHIGlobal<bool> GSupportsMultipleRenderTargets;

/** true if the RHI supports 256bit MRT */
extern X4_API bool GSupportsWideMRT;

/** True if the RHI and current hardware supports supports depth bounds testing */
extern X4_API bool GSupportsDepthBoundsTest;

/** True if the RHI and current hardware support a render target write mask */
extern X4_API bool GSupportsRenderTargetWriteMask;

/** True if the RHI and current hardware supports efficient AsyncCompute (by default we assume false and later we can enable this for more hardware) */
extern X4_API bool GSupportsEfficientAsyncCompute;

/** True if the RHI supports 'GetHDR32bppEncodeModeES2' shader intrinsic. */
extern X4_API bool GSupportsHDR32bppEncodeModeIntrinsic;

/** True if the RHI supports getting the result of occlusion queries when on a thread other than the renderthread */
extern X4_API bool GSupportsParallelOcclusionQueries;

/** true if the RHI supports aliasing of transient resources */
extern X4_API bool GSupportsTransientResourceAliasing;

/** true if the RHI requires a valid RT bound during UAV scatter operation inside the pixel shader */
extern X4_API bool GRHIRequiresRenderTargetForPixelShaderUAVs;

/** The minimum Z value in clip space for the RHI. */
extern X4_API float GMinClipZ;

/** The sign to apply to the Y axis of projection matrices. */
extern X4_API float GProjectionSignY;

/** Does this RHI need to wait for deletion of resources due to ref counting. */
extern X4_API bool GRHINeedsExtraDeletionLatency;

/** The maximum size to allow for the shadow depth buffer in the X dimension.  This must be larger or equal to GMaxShadowDepthBufferSizeY. */
extern X4_API TRHIGlobal<int32> GMaxShadowDepthBufferSizeX;
/** The maximum size to allow for the shadow depth buffer in the Y dimension. */
extern X4_API TRHIGlobal<int32> GMaxShadowDepthBufferSizeY;

/** The maximum size allowed for 2D textures in both dimensions. */
extern X4_API TRHIGlobal<int32> GMaxTextureDimensions;

FORCEINLINE uint32 GetMax2DTextureDimension()
{
	return GMaxTextureDimensions;
}

/** The maximum size allowed for cube textures. */
extern X4_API TRHIGlobal<int32> GMaxCubeTextureDimensions;
FORCEINLINE uint32 GetMaxCubeTextureDimension()
{
	return GMaxCubeTextureDimensions;
}

/** The Maximum number of layers in a 1D or 2D texture array. */
extern X4_API int32 GMaxTextureArrayLayers;
FORCEINLINE uint32 GetMaxTextureArrayLayers()
{
	return GMaxTextureArrayLayers;
}

extern X4_API int32 GMaxTextureSamplers;
FORCEINLINE uint32 GetMaxTextureSamplers()
{
	return GMaxTextureSamplers;
}


/**
 *	The size to check against for Draw*UP call vertex counts.
 *	If greater than this value, the draw call will not occur.
 */
extern X4_API int32 GDrawUPVertexCheckCount;
/**
 *	The size to check against for Draw*UP call index counts.
 *	If greater than this value, the draw call will not occur.
 */
extern X4_API int32 GDrawUPIndexCheckCount;

/** true for each VET that is supported. One-to-one mapping with EVertexElementType */
extern X4_API class FVertexElementTypeSupportInfo GVertexElementTypeSupport;

/** True if the RHI supports texture streaming */
extern X4_API bool GRHISupportsTextureStreaming;
/** Amount of memory allocated by textures. In kilobytes. */
extern X4_API volatile int32 GCurrentTextureMemorySize;
/** Amount of memory allocated by rendertargets. In kilobytes. */
extern X4_API volatile int32 GCurrentRendertargetMemorySize;
/** Current texture streaming pool size, in bytes. 0 means unlimited. */
extern X4_API int64 GTexturePoolSize;
/** In percent. If non-zero, the texture pool size is a percentage of GTotalGraphicsMemory. */
extern X4_API int32 GPoolSizeVRAMPercentage;

/** Some simple runtime stats, reset on every call to RHIBeginFrame */
/** Num draw calls & primitives on previous frame (accurate on any thread)*/
extern X4_API int32 GNumDrawCallsRHI;
extern X4_API int32 GNumPrimitivesDrawnRHI;

/** Num draw calls and primitives this frame (only accurate on RenderThread) */
extern X4_API int32 GCurrentNumDrawCallsRHI;
extern X4_API int32 GCurrentNumPrimitivesDrawnRHI;


/** Whether or not the RHI can handle a non-zero BaseVertexIndex - extra SetStreamSource calls will be needed if this is false */
extern X4_API bool GRHISupportsBaseVertexIndex;

/** True if the RHI supports hardware instancing */
extern X4_API TRHIGlobal<bool> GRHISupportsInstancing;

/** True if the RHI supports copying cubemap faces using CopyToResolveTarget */
extern X4_API bool GRHISupportsResolveCubemapFaces;

/** Whether or not the RHI can handle a non-zero FirstInstance - extra SetStreamSource calls will be needed if this is false */
extern X4_API bool GRHISupportsFirstInstance;

/** Whether or not the RHI can handle dynamic resolution or not. */
extern X4_API bool GRHISupportsDynamicResolution;

/** Whether or not the RHI can perform MSAA sample load. */
extern X4_API bool GRHISupportsMSAADepthSampleAccess;

/** Whether the present adapter/display offers HDR output capabilities. */
extern X4_API bool GRHISupportsHDROutput;

/** Format used for the backbuffer when outputting to a HDR display. */
extern X4_API EPixelFormat GRHIHDRDisplayOutputFormat;

/** Counter incremented once on each frame present. Used to support game thread synchronization with swap chain frame flips. */
extern X4_API uint64 GRHIPresentCounter;

/** Called once per frame only from within an RHI. */
extern X4_API void RHIPrivateBeginFrame();

class FReadSurfaceDataFlags
{
public:
	FReadSurfaceDataFlags(ERangeCompressionMode InCompressionMode = RCM_UNorm, ECubeFace InCubeFace = CubeFace_MAX)
		:CubeFace(InCubeFace), CompressionMode(InCompressionMode), bLinearToGamma(true), MaxDepthRange(16000.0f), bOutputStencil(false), MipLevel(0)
	{
	}
	ECubeFace GetCubeFace() const
	{
		checkSlow(CubeFace <= CubeFace_NegZ);
		return CubeFace;
	}

	ERangeCompressionMode GetCompressionMode() const
	{
		return CompressionMode;
	}

	void SetLinearToGamma(bool Value)
	{
		bLinearToGamma = Value;
	}

	bool GetLinearToGamma() const
	{
		return bLinearToGamma;
	}

	void SetOutputStencil(bool Value)
	{
		bOutputStencil = Value;
	}

	bool GetOutputStencil() const
	{
		return bOutputStencil;
	}

	void SetMip(uint8 InMipLevel)
	{
		MipLevel = InMipLevel;
	}

	uint8 GetMip() const
	{
		return MipLevel;
	}

	void SetMaxDepthRange(float Value)
	{
		MaxDepthRange = Value;
	}

	float ComputeNormalizedDepth(float DeviceZ) const
	{
		return FMath::Abs(ConvertFromDeviceZ(DeviceZ) / MaxDepthRange);
	}
private:
	// @return SceneDepth
	float ConvertFromDeviceZ(float DeviceZ) const
	{
		DeviceZ = FMath::Min(DeviceZ, 1 - Z_PRECISION);

		// for depth to linear conversion
		const FVector2D InvDeviceZToWorldZ(0.1f, 0.1f);

		return 1.0f / (DeviceZ * InvDeviceZToWorldZ.X - InvDeviceZToWorldZ.Y);
	}
	ECubeFace CubeFace;
	ERangeCompressionMode CompressionMode;
	bool bLinearToGamma;
	float MaxDepthRange;
	bool bOutputStencil;
	uint8 MipLevel;
};

/** Info for supporting the vertex element types */
class FVertexElementTypeSupportInfo
{
public:
	FVertexElementTypeSupportInfo() { for (int32 i = 0; i < VET_MAX; i++) ElementCaps[i] = true; }
	FORCEINLINE bool IsSupported(EVertexElementType ElementType) { return ElementCaps[ElementType]; }
	FORCEINLINE void SetSupported(EVertexElementType ElementType, bool bIsSupported) { ElementCaps[ElementType] = bIsSupported; }
private:
	/** cap bit set for each VET. One-to-one mapping based on EVertexElementType */
	bool ElementCaps[VET_MAX];
};

struct FVertexElement
{
	uint8 StreamIndex;
	uint8 Offset;
	TEnumAsByte<EVertexElementType> Type;
	uint8 AttributeIndex;
	uint16 Stride;
	/**
	 * Whether to use instance index or vertex index to consume the element.
	 * eg if bUseInstanceIndex is 0, the element will be repeated for every instance.
	 */
	uint16 bUseInstanceIndex;

	FVertexElement() {}
	FVertexElement(uint8 InStreamIndex, uint8 InOffset, EVertexElementType InType, uint8 InAttributeIndex, uint16 InStride, bool bInUseInstanceIndex = false) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride),
		bUseInstanceIndex(bInUseInstanceIndex)
	{}
	/**
	* Suppress the compiler generated assignment operator so that padding won't be copied.
	* This is necessary to get expected results for code that zeros, assigns and then CRC's the whole struct.
	*/
	void operator=(const FVertexElement& Other)
	{
		StreamIndex = Other.StreamIndex;
		Offset = Other.Offset;
		Type = Other.Type;
		AttributeIndex = Other.AttributeIndex;
		bUseInstanceIndex = Other.bUseInstanceIndex;
	}
	X4_API FString ToString() const;
	X4_API void FromString(const FString& Src);
};

/** RHI representation of a single stream out element. */
struct FStreamOutElement
{
	/** Index of the output stream from the geometry shader. */
	uint32 Stream;

	/** Semantic name of the output element as defined in the geometry shader.  This should not contain the semantic number. */
	const ANSICHAR* SemanticName;

	/** Semantic index of the output element as defined in the geometry shader.  For example "TEXCOORD5" in the shader would give a SemanticIndex of 5. */
	uint32 SemanticIndex;

	/** Start component index of the shader output element to stream out. */
	uint8 StartComponent;

	/** Number of components of the shader output element to stream out. */
	uint8 ComponentCount;

	/** Stream output target slot, corresponding to the streams set by RHISetStreamOutTargets. */
	uint8 OutputSlot;

	FStreamOutElement() {}
	FStreamOutElement(uint32 InStream, const ANSICHAR* InSemanticName, uint32 InSemanticIndex, uint8 InComponentCount, uint8 InOutputSlot) :
		Stream(InStream),
		SemanticName(InSemanticName),
		SemanticIndex(InSemanticIndex),
		StartComponent(0),
		ComponentCount(InComponentCount),
		OutputSlot(InOutputSlot)
	{}
};

typedef TArray<FStreamOutElement, TFixedAllocator<MaxVertexElementCount> > FStreamOutElementList;