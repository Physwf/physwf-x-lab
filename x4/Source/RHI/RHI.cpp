#include "RHI/RHI.h"
#include "Modules/ModuleManager.h"

const FString FResourceTransitionUtility::ResourceTransitionAccessStrings[(int32)EResourceTransitionAccess::EMaxAccess + 1] =
{
	FString(TEXT("EReadable")),
	FString(TEXT("EWritable")),
	FString(TEXT("ERWBarrier")),
	FString(TEXT("ERWNoBarrier")),
	FString(TEXT("ERWSubResBarrier")),
	FString(TEXT("EMetaData")),
	FString(TEXT("EMaxAccess")),
};

//DO NOT USE THE STATIC FLINEARCOLORS TO INITIALIZE THIS STUFF.  
//Static init order is undefined and you will likely end up with bad values on some platforms.
const FClearValueBinding FClearValueBinding::None(EClearBinding::ENoneBound);
const FClearValueBinding FClearValueBinding::Black(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
const FClearValueBinding FClearValueBinding::White(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
const FClearValueBinding FClearValueBinding::Transparent(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
const FClearValueBinding FClearValueBinding::DepthOne(1.0f, 0);
const FClearValueBinding FClearValueBinding::DepthZero(0.0f, 0);
const FClearValueBinding FClearValueBinding::DepthNear((float)ERHIZBuffer::NearPlane, 0);
const FClearValueBinding FClearValueBinding::DepthFar((float)ERHIZBuffer::FarPlane, 0);
const FClearValueBinding FClearValueBinding::Green(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
// Note: this is used as the default normal for DBuffer decals.  It must decode to a value of 0 in DecodeDBufferData.
const FClearValueBinding FClearValueBinding::DefaultNormal8Bit(FLinearColor(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f));


//TLockFreePointerListUnordered<FRHIResource, PLATFORM_CACHE_LINE_SIZE> FRHIResource::PendingDeletes;
//FRHIResource* FRHIResource::CurrentlyDeleting = nullptr;
//TArray<FRHIResource::ResourcesToDelete> FRHIResource::DeferredDeletionQueue;
//uint32 FRHIResource::CurrentFrame = 0;

FString FVertexElement::ToString() const
{
	return FString::Printf(TEXT("<%u %u %u %u %u %u>")
		, uint32(StreamIndex)
		, uint32(Offset)
		, uint32(Type)
		, uint32(AttributeIndex)
		, uint32(Stride)
		, uint32(bUseInstanceIndex)
	);
}

void FVertexElement::FromString(const FString& InSrc)
{
	FString Src = InSrc;
	Src.ReplaceInline(TEXT("\r"), TEXT(" "));
	Src.ReplaceInline(TEXT("\n"), TEXT(" "));
	Src.ReplaceInline(TEXT("\t"), TEXT(" "));
	Src.ReplaceInline(TEXT("<"), TEXT(" "));
	Src.ReplaceInline(TEXT(">"), TEXT(" "));
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(" "));

	check(Parts.Num() == 6 && sizeof(Type) == 1); //not a very robust parser
	LexFromString(StreamIndex, *Parts[0]);
	LexFromString(Offset, *Parts[1]);
	LexFromString((uint8&)Type, *Parts[2]);
	LexFromString(AttributeIndex, *Parts[3]);
	LexFromString(Stride, *Parts[4]);
	LexFromString(bUseInstanceIndex, *Parts[5]);
}

FString FDepthStencilStateInitializerRHI::ToString() const
{
	return
		FString::Printf(TEXT("<%u %u ")
			, uint32(!!bEnableDepthWrite)
			, uint32(DepthTest)
		)
		+ FString::Printf(TEXT("%u %u %u %u %u ")
			, uint32(!!bEnableFrontFaceStencil)
			, uint32(FrontFaceStencilTest)
			, uint32(FrontFaceStencilFailStencilOp)
			, uint32(FrontFaceDepthFailStencilOp)
			, uint32(FrontFacePassStencilOp)
		)
		+ FString::Printf(TEXT("%u %u %u %u %u ")
			, uint32(!!bEnableBackFaceStencil)
			, uint32(BackFaceStencilTest)
			, uint32(BackFaceStencilFailStencilOp)
			, uint32(BackFaceDepthFailStencilOp)
			, uint32(BackFacePassStencilOp)
		)
		+ FString::Printf(TEXT("%u %u>")
			, uint32(StencilReadMask)
			, uint32(StencilWriteMask)
		);
}
void FDepthStencilStateInitializerRHI::FromString(const FString& InSrc)
{
	FString Src = InSrc;
	Src.ReplaceInline(TEXT("\r"), TEXT(" "));
	Src.ReplaceInline(TEXT("\n"), TEXT(" "));
	Src.ReplaceInline(TEXT("\t"), TEXT(" "));
	Src.ReplaceInline(TEXT("<"), TEXT(" "));
	Src.ReplaceInline(TEXT(">"), TEXT(" "));
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(" "));

	check(Parts.Num() == 14 && sizeof(bool) == 1 && sizeof(FrontFaceStencilFailStencilOp) == 1 && sizeof(BackFaceStencilTest) == 1 && sizeof(BackFaceDepthFailStencilOp) == 1); //not a very robust parser

	LexFromString((uint8&)bEnableDepthWrite, *Parts[0]);
	LexFromString((uint8&)DepthTest, *Parts[1]);

	LexFromString((uint8&)bEnableFrontFaceStencil, *Parts[2]);
	LexFromString((uint8&)FrontFaceStencilTest, *Parts[3]);
	LexFromString((uint8&)FrontFaceStencilFailStencilOp, *Parts[4]);
	LexFromString((uint8&)FrontFaceDepthFailStencilOp, *Parts[5]);
	LexFromString((uint8&)FrontFacePassStencilOp, *Parts[6]);

	LexFromString((uint8&)bEnableBackFaceStencil, *Parts[7]);
	LexFromString((uint8&)BackFaceStencilTest, *Parts[8]);
	LexFromString((uint8&)BackFaceStencilFailStencilOp, *Parts[9]);
	LexFromString((uint8&)BackFaceDepthFailStencilOp, *Parts[10]);
	LexFromString((uint8&)BackFacePassStencilOp, *Parts[11]);

	LexFromString(StencilReadMask, *Parts[12]);
	LexFromString(StencilWriteMask, *Parts[13]);
}

FString FBlendStateInitializerRHI::ToString() const
{
	FString Result = TEXT("<");
	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; Index++)
	{
		Result += RenderTargets[Index].ToString();
	}
	Result += FString::Printf(TEXT("%d>"), uint32(!!bUseIndependentRenderTargetBlendStates));
	return Result;
}

void FBlendStateInitializerRHI::FromString(const FString& InSrc)
{
	FString Src = InSrc;
	Src.ReplaceInline(TEXT("\r"), TEXT(" "));
	Src.ReplaceInline(TEXT("\n"), TEXT(" "));
	Src.ReplaceInline(TEXT("\t"), TEXT(" "));
	Src.ReplaceInline(TEXT("<"), TEXT(" "));
	Src.ReplaceInline(TEXT(">"), TEXT(" "));
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(" "));


	check(Parts.Num() == MaxSimultaneousRenderTargets * FRenderTarget::NUM_STRING_FIELDS + 1 && sizeof(bool) == 1); //not a very robust parser
	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; Index++)
	{
		RenderTargets[Index].FromString(Parts, FRenderTarget::NUM_STRING_FIELDS * Index);
	}
	LexFromString((int8&)bUseIndependentRenderTargetBlendStates, *Parts[MaxSimultaneousRenderTargets * FRenderTarget::NUM_STRING_FIELDS]);
}


FString FBlendStateInitializerRHI::FRenderTarget::ToString() const
{
	return FString::Printf(TEXT("%u %u %u %u %u %u %u ")
		, uint32(ColorBlendOp)
		, uint32(ColorSrcBlend)
		, uint32(ColorDestBlend)
		, uint32(AlphaBlendOp)
		, uint32(AlphaSrcBlend)
		, uint32(AlphaDestBlend)
		, uint32(ColorWriteMask)
	);
}

void FBlendStateInitializerRHI::FRenderTarget::FromString(const TArray<FString>& Parts, int32 Index)
{
	check(Index + NUM_STRING_FIELDS <= Parts.Num());
	LexFromString((uint8&)ColorBlendOp, *Parts[Index++]);
	LexFromString((uint8&)ColorSrcBlend, *Parts[Index++]);
	LexFromString((uint8&)ColorDestBlend, *Parts[Index++]);
	LexFromString((uint8&)AlphaBlendOp, *Parts[Index++]);
	LexFromString((uint8&)AlphaSrcBlend, *Parts[Index++]);
	LexFromString((uint8&)AlphaDestBlend, *Parts[Index++]);
	LexFromString((uint8&)ColorWriteMask, *Parts[Index++]);
}

bool FRHIResource::Bypass()
{
	return GRHICommandList.Bypass();
}

void FRHIResource::FlushPendingDeletes(bool bFlushDeferredDeletes /*= false*/)
{
	//SCOPE_CYCLE_COUNTER(STAT_DeleteResources);
#if 0
	check(IsInRenderingThread());
	FRHICommandListExecutor::GetImmediateCommandList().ImmediateFlush(EImmediateFlushType::FlushRHIThread);
	FRHICommandListExecutor::CheckNoOutstandingCmdLists();
	if (GDynamicRHI)
	{
		GDynamicRHI->RHIPerFrameRHIFlushComplete();
	}

	auto Delete = [](TArray<FRHIResource*>& ToDelete)
	{
		for (int32 Index = 0; Index < ToDelete.Num(); Index++)
		{
			FRHIResource* Ref = ToDelete[Index];
			check(Ref->MarkedForDelete == 1);
			if (Ref->GetRefCount() == 0) // caches can bring dead objects back to life
			{
				CurrentlyDeleting = Ref;
				delete Ref;
				CurrentlyDeleting = nullptr;
			}
			else
			{
				Ref->MarkedForDelete = 0;
				FPlatformMisc::MemoryBarrier();
			}
		}
	};

	while (1)
	{
		if (PendingDeletes.IsEmpty())
		{
			break;
		}
		if (PlatformNeedsExtraDeletionLatency())
		{
			const int32 Index = DeferredDeletionQueue.AddDefaulted();
			ResourcesToDelete& ResourceBatch = DeferredDeletionQueue[Index];
			ResourceBatch.FrameDeleted = CurrentFrame;
			PendingDeletes.PopAll(ResourceBatch.Resources);
			check(ResourceBatch.Resources.Num());
		}
		else
		{
			TArray<FRHIResource*> ToDelete;
			PendingDeletes.PopAll(ToDelete);
			check(ToDelete.Num());
			Delete(ToDelete);
		}
	}

	const uint32 NumFramesToExpire = 3;

	if (DeferredDeletionQueue.Num())
	{
		if (bFlushDeferredDeletes)
		{
			FRHICommandListExecutor::GetImmediateCommandList().BlockUntilGPUIdle();

			for (int32 Idx = 0; Idx < DeferredDeletionQueue.Num(); ++Idx)
			{
				ResourcesToDelete& ResourceBatch = DeferredDeletionQueue[Idx];
				Delete(ResourceBatch.Resources);
			}

			DeferredDeletionQueue.Empty();
		}
		else
		{
			int32 DeletedBatchCount = 0;
			while (DeletedBatchCount < DeferredDeletionQueue.Num())
			{
				ResourcesToDelete& ResourceBatch = DeferredDeletionQueue[DeletedBatchCount];
				if (((ResourceBatch.FrameDeleted + NumFramesToExpire) < CurrentFrame) || !GIsRHIInitialized)
				{
					Delete(ResourceBatch.Resources);
					++DeletedBatchCount;
				}
				else
				{
					break;
				}
			}

			if (DeletedBatchCount)
			{
				DeferredDeletionQueue.RemoveAt(0, DeletedBatchCount);
			}
		}

		++CurrentFrame;
	}
#endif
}

static_assert(ERHIZBuffer::FarPlane != ERHIZBuffer::NearPlane, "Near and Far planes must be different!");
static_assert((int32)ERHIZBuffer::NearPlane == 0 || (int32)ERHIZBuffer::NearPlane == 1, "Invalid Values for Near Plane, can only be 0 or 1!");
static_assert((int32)ERHIZBuffer::FarPlane == 0 || (int32)ERHIZBuffer::FarPlane == 1, "Invalid Values for Far Plane, can only be 0 or 1");

/**
 * RHI globals.
 */

bool GIsRHIInitialized = false;
//int32 GMaxTextureMipCount = MAX_TEXTURE_MIP_COUNT;
bool GSupportsQuadBufferStereo = false;
bool GSupportsDepthFetchDuringDepthTest = true;
FString GRHIAdapterName;
FString GRHIAdapterInternalDriverVersion;
FString GRHIAdapterUserDriverVersion;
FString GRHIAdapterDriverDate;
uint32 GRHIVendorId = 0;
uint32 GRHIDeviceId = 0;
uint32 GRHIDeviceRevision = 0;
bool GRHIDeviceIsAMDPreGCNArchitecture = false;
bool GSupportsRenderDepthTargetableShaderResources = true;
TRHIGlobal<bool> GSupportsRenderTargetFormat_PF_G8(true);
TRHIGlobal<bool> GSupportsRenderTargetFormat_PF_FloatRGBA(true);
bool GSupportsShaderFramebufferFetch = false;
bool GSupportsShaderDepthStencilFetch = false;
bool GSupportsTimestampRenderQueries = false;
bool GRHISupportsGPUTimestampBubblesRemoval = false;
bool GRHISupportsFrameCyclesBubblesRemoval = false;
bool GHardwareHiddenSurfaceRemoval = false;
bool GRHISupportsAsyncTextureCreation = false;
bool GRHISupportsQuadTopology = false;
bool GRHISupportsRectTopology = false;
bool GSupportsParallelRenderingTasksWithSeparateRHIThread = true;
bool GRHIThreadNeedsKicking = false;
//int32 GRHIMaximumReccommendedOustandingOcclusionQueries = MAX_int32;
bool GRHISupportsExactOcclusionQueries = true;
bool GSupportsVolumeTextureRendering = true;
bool GSupportsSeparateRenderTargetBlendState = false;
bool GSupportsDepthRenderTargetWithoutColorRenderTarget = true;
bool GRHINeedsUnatlasedCSMDepthsWorkaround = false;
bool GSupportsTexture3D = true;
bool GSupportsMobileMultiView = false;
bool GSupportsImageExternal = false;
bool GSupportsResourceView = true;
TRHIGlobal<bool> GSupportsMultipleRenderTargets(true);
bool GSupportsWideMRT = true;
float GMinClipZ = 0.0f;
float GProjectionSignY = 1.0f;
bool GRHINeedsExtraDeletionLatency = false;
TRHIGlobal<int32> GMaxShadowDepthBufferSizeX(2048);
TRHIGlobal<int32> GMaxShadowDepthBufferSizeY(2048);
TRHIGlobal<int32> GMaxTextureDimensions(2048);
TRHIGlobal<int32> GMaxCubeTextureDimensions(2048);
int32 GMaxTextureArrayLayers = 256;
int32 GMaxTextureSamplers = 16;
bool GUsingNullRHI = false;
int32 GDrawUPVertexCheckCount = MAX_int32;
int32 GDrawUPIndexCheckCount = MAX_int32;
bool GTriggerGPUProfile = false;
FString GGPUTraceFileName;
bool GRHISupportsTextureStreaming = false;
bool GSupportsDepthBoundsTest = false;
bool GSupportsEfficientAsyncCompute = false;
bool GRHISupportsBaseVertexIndex = true;
TRHIGlobal<bool> GRHISupportsInstancing(true);
bool GRHISupportsFirstInstance = false;
bool GRHISupportsDynamicResolution = false;
bool GRHISupportsRHIThread = false;
bool GRHISupportsRHIOnTaskThread = false;
bool GRHISupportsParallelRHIExecute = false;
bool GSupportsHDR32bppEncodeModeIntrinsic = false;
bool GSupportsParallelOcclusionQueries = false;
bool GSupportsRenderTargetWriteMask = false;
bool GSupportsTransientResourceAliasing = false;
bool GRHIRequiresRenderTargetForPixelShaderUAVs = false;

bool GRHISupportsMSAADepthSampleAccess = false;
bool GRHISupportsResolveCubemapFaces = false;

bool GRHISupportsHDROutput = false;
EPixelFormat GRHIHDRDisplayOutputFormat = PF_FloatRGBA;

uint64 GRHIPresentCounter = 1;

/** Whether we are profiling GPU hitches. */
bool GTriggerGPUHitchProfile = false;

FVertexElementTypeSupportInfo GVertexElementTypeSupport;

X4_API int32 volatile GCurrentTextureMemorySize = 0;
X4_API int32 volatile GCurrentRendertargetMemorySize = 0;
X4_API int64 GTexturePoolSize = 0 * 1024 * 1024;
X4_API int32 GPoolSizeVRAMPercentage = 0;

X4_API EShaderPlatform GShaderPlatformForFeatureLevel[ERHIFeatureLevel::Num] = { SP_NumPlatforms,SP_NumPlatforms,SP_NumPlatforms,SP_NumPlatforms };

// simple stats about draw calls. GNum is the previous frame and 
// GCurrent is the current frame.
X4_API int32 GCurrentNumDrawCallsRHI = 0;
X4_API int32 GNumDrawCallsRHI = 0;
X4_API int32 GCurrentNumPrimitivesDrawnRHI = 0;
X4_API int32 GNumPrimitivesDrawnRHI = 0;

/** Called once per frame only from within an RHI. */
void RHIPrivateBeginFrame()
{
	GNumDrawCallsRHI = GCurrentNumDrawCallsRHI;
	GNumPrimitivesDrawnRHI = GCurrentNumPrimitivesDrawnRHI;
	//CSV_CUSTOM_STAT(RHI, DrawCalls, GNumDrawCallsRHI, ECsvCustomStatOp::Set);
	//CSV_CUSTOM_STAT(RHI, PrimitivesDrawn, GNumPrimitivesDrawnRHI, ECsvCustomStatOp::Set);
	GCurrentNumDrawCallsRHI = GCurrentNumPrimitivesDrawnRHI = 0;
}

/** Whether to initialize 3D textures using a bulk data (or through a mip update if false). */
X4_API bool GUseTexture3DBulkDataRHI = false;

//
// The current shader platform.
//

X4_API EShaderPlatform GMaxRHIShaderPlatform = SP_PCD3D_SM5;

/** The maximum feature level supported on this machine */
X4_API ERHIFeatureLevel::Type GMaxRHIFeatureLevel = ERHIFeatureLevel::SM5;

X4_API bool IsRHIDeviceAMD()
{
	check(GRHIVendorId != 0);
	// AMD's drivers tested on July 11 2013 have hitching problems with async resource streaming, setting single threaded for now until fixed.
	return GRHIVendorId == 0x1002;
}

X4_API bool IsRHIDeviceIntel()
{
	check(GRHIVendorId != 0);
	// Intel GPUs are integrated and use both DedicatedVideoMemory and SharedSystemMemory.
	return GRHIVendorId == 0x8086;
}

X4_API bool IsRHIDeviceNVIDIA()
{
	check(GRHIVendorId != 0);
	// NVIDIA GPUs are discrete and use DedicatedVideoMemory only.
	return GRHIVendorId == 0x10DE;
}

X4_API const TCHAR* RHIVendorIdToString()
{
	switch (GRHIVendorId)
	{
	case 0x1002: return TEXT("AMD");
	case 0x1010: return TEXT("ImgTec");
	case 0x10DE: return TEXT("NVIDIA");
	case 0x13B5: return TEXT("ARM");
	case 0x5143: return TEXT("Qualcomm");
	case 0x8086: return TEXT("Intel");
	default: return TEXT("Unknown");
	}
}

X4_API uint32 RHIGetShaderLanguageVersion(const EShaderPlatform Platform)
{
	uint32 Version = 0;
	if (IsMetalPlatform(Platform))
	{
		if (IsPCPlatform(Platform))
		{
			static int32 MaxShaderVersion = -1;
			if (MaxShaderVersion < 0)
			{
				MaxShaderVersion = 2;
				//if (!GConfig->GetInt(TEXT("/Script/MacTargetPlatform.MacTargetSettings"), TEXT("MaxShaderLanguageVersion"), MaxShaderVersion, GEngineIni))
				{
					MaxShaderVersion = 2;
				}
			}
			Version = (uint32)MaxShaderVersion;
		}
		else
		{
			static int32 MaxShaderVersion = -1;
			if (MaxShaderVersion < 0)
			{
				MaxShaderVersion = 0;
				//if (!GConfig->GetInt(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("MaxShaderLanguageVersion"), MaxShaderVersion, GEngineIni))
				{
					MaxShaderVersion = 0;
				}
			}
			Version = (uint32)MaxShaderVersion;
		}
	}
	return Version;
}

X4_API bool RHISupportsTessellation(const EShaderPlatform Platform)
{
	if (IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5) && !IsMetalPlatform(Platform))
	{
		return (Platform == SP_PCD3D_SM5) || (Platform == SP_XBOXONE_D3D12) || (Platform == SP_OPENGL_SM5) || (Platform == SP_OPENGL_ES31_EXT)/* || (IsVulkanSM5Platform(Platform)*/;
	}
	// For Metal we can only support tessellation if we are willing to sacrifice backward compatibility with OS versions.
	// As such it becomes an opt-in project setting.
	else if (Platform == SP_METAL_SM5)
	{
		return (RHIGetShaderLanguageVersion(Platform) >= 2);
	}
	return false;
}

X4_API bool RHISupportsPixelShaderUAVs(const EShaderPlatform Platform)
{
	if (IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5) && !IsMetalPlatform(Platform))
	{
		return true;
	}
	else if (Platform == SP_METAL_SM5 || Platform == SP_METAL_SM5_NOTESS || Platform == SP_METAL_MRT || Platform == SP_METAL_MRT_MAC)
	{
		return (RHIGetShaderLanguageVersion(Platform) >= 2);
	}
	return false;
}

X4_API bool RHISupportsIndexBufferUAVs(const EShaderPlatform Platform)
{
	return Platform == SP_PCD3D_SM5 || IsVulkanPlatform(Platform) || Platform == SP_XBOXONE_D3D12 || Platform == SP_PS4;
}

static ERHIFeatureLevel::Type GRHIMobilePreviewFeatureLevel = ERHIFeatureLevel::Num;
X4_API void RHISetMobilePreviewFeatureLevel(ERHIFeatureLevel::Type MobilePreviewFeatureLevel)
{
	check(MobilePreviewFeatureLevel == ERHIFeatureLevel::ES2 || MobilePreviewFeatureLevel == ERHIFeatureLevel::ES3_1);
	check(GRHIMobilePreviewFeatureLevel == ERHIFeatureLevel::Num);
	//check(!GIsEditor);
	GRHIMobilePreviewFeatureLevel = MobilePreviewFeatureLevel;
}

bool RHIGetPreviewFeatureLevel(ERHIFeatureLevel::Type& PreviewFeatureLevelOUT)
{
#if 0
	static bool bForceFeatureLevelES2 = !GIsEditor && FParse::Param(FCommandLine::Get(), TEXT("FeatureLevelES2"));
	static bool bForceFeatureLevelES3_1 = !GIsEditor && (FParse::Param(FCommandLine::Get(), TEXT("FeatureLevelES31")) || FParse::Param(FCommandLine::Get(), TEXT("FeatureLevelES3_1")));

	if (bForceFeatureLevelES2)
	{
		PreviewFeatureLevelOUT = ERHIFeatureLevel::ES2;
	}
	else if (bForceFeatureLevelES3_1)
	{
		PreviewFeatureLevelOUT = ERHIFeatureLevel::ES3_1;
	}
	else if (!GIsEditor && GRHIMobilePreviewFeatureLevel != ERHIFeatureLevel::Num)
	{
		PreviewFeatureLevelOUT = GRHIMobilePreviewFeatureLevel;
	}
	else
	{
		return false;
	}
	return true;
#endif
	return true;
}



void FRHIRenderPassInfo::ConvertToRenderTargetsInfo(FRHISetRenderTargetsInfo& OutRTInfo) const
{
	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; ++Index)
	{
		if (!ColorRenderTargets[Index].RenderTarget)
		{
			break;
		}

		OutRTInfo.ColorRenderTarget[Index].Texture = ColorRenderTargets[Index].RenderTarget;
		ERenderTargetLoadAction LoadAction = GetLoadAction(ColorRenderTargets[Index].Action);
		OutRTInfo.ColorRenderTarget[Index].LoadAction = LoadAction;
		OutRTInfo.ColorRenderTarget[Index].StoreAction = GetStoreAction(ColorRenderTargets[Index].Action);
		OutRTInfo.ColorRenderTarget[Index].ArraySliceIndex = ColorRenderTargets[Index].ArraySlice;
		OutRTInfo.ColorRenderTarget[Index].MipIndex = ColorRenderTargets[Index].MipIndex;
		++OutRTInfo.NumColorRenderTargets;

		OutRTInfo.bClearColor |= (LoadAction == ERenderTargetLoadAction::EClear);
	}

	ERenderTargetActions DepthActions = GetDepthActions(DepthStencilRenderTarget.Action);
	ERenderTargetActions StencilActions = GetStencilActions(DepthStencilRenderTarget.Action);
	ERenderTargetLoadAction DepthLoadAction = GetLoadAction(DepthActions);
	ERenderTargetStoreAction DepthStoreAction = GetStoreAction(DepthActions);
	ERenderTargetLoadAction StencilLoadAction = GetLoadAction(StencilActions);
	ERenderTargetStoreAction StencilStoreAction = GetStoreAction(StencilActions);

	OutRTInfo.DepthStencilRenderTarget = FRHIDepthRenderTargetView(DepthStencilRenderTarget.DepthStencilTarget,
		DepthLoadAction,
		GetStoreAction(DepthActions),
		StencilLoadAction,
		GetStoreAction(StencilActions),
		DepthStencilRenderTarget.ExclusiveDepthStencil);
	OutRTInfo.bClearDepth = (DepthLoadAction == ERenderTargetLoadAction::EClear);
	OutRTInfo.bClearStencil = (StencilLoadAction == ERenderTargetLoadAction::EClear);

	if (NumUAVs > 0)
	{
		check(UAVIndex != -1);
		check(UAVIndex >= OutRTInfo.NumColorRenderTargets);
		OutRTInfo.NumColorRenderTargets = UAVIndex;
		for (int32 Index = 0; Index < NumUAVs; ++Index)
		{
			OutRTInfo.UnorderedAccessView[Index] = UAVs[Index];
		}
		OutRTInfo.NumUAVs = NumUAVs;
	}
}


void FRHIRenderPassInfo::Validate() const
{
	int32 NumSamples = -1;	// -1 means nothing found yet
	int32 ColorIndex = 0;
	for (; ColorIndex < MaxSimultaneousRenderTargets; ++ColorIndex)
	{
		const FColorEntry& Entry = ColorRenderTargets[ColorIndex];
		if (Entry.RenderTarget)
		{
			// Ensure NumSamples matches amongst all color RTs
			if (NumSamples == -1)
			{
				NumSamples = Entry.RenderTarget->GetNumSamples();
			}
			else
			{
				ensure(Entry.RenderTarget->GetNumSamples() == NumSamples);
			}

			ERenderTargetStoreAction Store = GetStoreAction(Entry.Action);
			// Don't try to resolve a non-msaa
			ensure(Store != ERenderTargetStoreAction::EMultisampleResolve || Entry.RenderTarget->GetNumSamples() > 1);
			// Don't resolve to null
			ensure(Store != ERenderTargetStoreAction::EMultisampleResolve || Entry.ResolveTarget);
		}
		else
		{
			break;
		}
	}

	int32 NumColorRenderTargets = ColorIndex;
	for (; ColorIndex < MaxSimultaneousRenderTargets; ++ColorIndex)
	{
		// Gap in the sequence of valid render targets (ie RT0, null, RT2, ...)
		ensureMsgf(!ColorRenderTargets[ColorIndex].RenderTarget, TEXT("Missing color render target on slot %d"), ColorIndex - 1);
	}

	if (bGeneratingMips)
	{
		if (NumColorRenderTargets == 0)
		{
			ensureMsgf(0, TEXT("Missing color render target for which to generate mips!"));
		}
		else if (NumColorRenderTargets > 1)
		{
			ensureMsgf(0, TEXT("Too many color render targets for which to generate mips!"));
		}
	}

	if (DepthStencilRenderTarget.DepthStencilTarget)
	{
		ensureMsgf(!bGeneratingMips, TEXT("Can't (currently) generate mip maps off a depth/stencil target!"));
		// Ensure NumSamples matches with color RT
		if (NumSamples != -1)
		{
			ensure(DepthStencilRenderTarget.DepthStencilTarget->GetNumSamples() == NumSamples);
		}
		ERenderTargetStoreAction DepthStore = GetStoreAction(GetDepthActions(DepthStencilRenderTarget.Action));
		ERenderTargetStoreAction StencilStore = GetStoreAction(GetStencilActions(DepthStencilRenderTarget.Action));
		bool bIsMSAAResolve = (DepthStore == ERenderTargetStoreAction::EMultisampleResolve) || (StencilStore == ERenderTargetStoreAction::EMultisampleResolve);
		// Don't try to resolve a non-msaa
		ensure(!bIsMSAAResolve || DepthStencilRenderTarget.DepthStencilTarget->GetNumSamples() > 1);
		// Don't resolve to null
		ensure(DepthStencilRenderTarget.ResolveTarget || DepthStore != ERenderTargetStoreAction::EStore);
		// Don't write to depth if read-only
		ensure(DepthStencilRenderTarget.ExclusiveDepthStencil.IsDepthWrite() || DepthStore != ERenderTargetStoreAction::EStore);
		ensure(DepthStencilRenderTarget.ExclusiveDepthStencil.IsStencilWrite() || StencilStore != ERenderTargetStoreAction::EStore);
	}
	else
	{
		ensure(DepthStencilRenderTarget.Action == EDepthStencilTargetActions::DontLoad_DontStore);
		ensure(DepthStencilRenderTarget.ExclusiveDepthStencil == FExclusiveDepthStencil::DepthNop_StencilNop);
	}
}
