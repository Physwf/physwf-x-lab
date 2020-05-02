#pragma once

#include "CoreMinimal.h"
//#include "Stats/Stats.h"
#include "Misc/Guid.h"
//#include "HAL/IConsoleManager.h"
#include "Misc/SecureHash.h"
#include "RHI/RHI.h"
#include "RenderCore/TickableObjectRenderThread.h"
#include "ShaderCacheTypes.h"
#include "ShaderCodeLibrary.h"

#include "X4.h"

/** Custom serialization version for FShaderCache */
struct X4_API FShaderCacheCustomVersion
{
	static const FGuid Key;
	static const FGuid GameKey;
	enum Type { Initial, PreDraw, CacheHashes, OptimisedHashes, StreamingKeys, AdditionalResources, SeparateBinaries, IndexedSets, PreDrawEntries, CompressedBinaries, CacheMerging, ShaderPipelines, SimpleVersioning, PlatformLibraries, OptionalResourceTracking, PipelineStateObjects, PipelineStateObjects2, Latest = PipelineStateObjects2 };
};

enum EShaderCacheOptions
{
	SCO_Default,
	SCO_NoShaderPreload = 1 << 0, /* Disable preloading of shaders for RHIs where loading all shaders is too slow (i.e. Metal online compiler). */
};

//
//FShaderCacheState
//
class X4_API FShaderCacheState
{
	friend class FShaderCache;

public:
	FShaderCacheState()
		: bCurrentDepthStencilTarget(false)
		, CurrentNumRenderTargets(0)
		, bIsPreDraw(false)
		, bIsPreBind(false)
		, InvalidResourceCount(0)
	{
		Viewport[0] = Viewport[1] = Viewport[2] = Viewport[3] = 0;
		DepthRange[0] = DepthRange[1] = 0.0f;
	}

private:
	bool bCurrentDepthStencilTarget;
	uint32 CurrentNumRenderTargets;
	FRHIDepthRenderTargetView CurrentDepthStencilTarget;
	FRHIRenderTargetView CurrentRenderTargets[MaxSimultaneousRenderTargets];
	FShaderCacheGraphicsPipelineState CurrentPSO;
	FShaderDrawKey CurrentDrawKey;
	uint32 Viewport[4];
	float DepthRange[2];
	bool bIsPreDraw;
	bool bIsPreBind;

	// When the invalid resource count is greater than 0 no draw keys will be stored to prevent corrupting the shader cache.
	// Warnings are emitted to indicate that the shader cache has encountered a resource lifetime error.
	uint32 InvalidResourceCount;
};

//
//FShaderCache
//
class X4_API FShaderCache : public FTickableObjectRenderThread
{
	friend class FShaderCacheLibrary;

public:
	FShaderCache(uint32 Options, EShaderPlatform Platform);
	virtual ~FShaderCache();

	/** Called by the game to set the game specific shader cache version, only caches of this version will be loaded. Must be called before RHI initialisation, as InitShaderCache will load any existing cache. Defaults to FEngineVersion::Current().GetChangelist() if never called. */
	static void SetGameVersion(int32 InGameVersion);
	static int32 GetGameVersion() { return GameVersion; }

	static FORCEINLINE void SetMaxShaderResources(uint32 InMaxResources)
	{
		check(InMaxResources <= EShaderCacheMaxNumResources);
		MaxResources = InMaxResources;
		FShaderDrawKey::CurrentMaxResources = InMaxResources;
	}

	/** Shader cache initialisation, called only by the RHI. */
	static void InitShaderCache(uint32 Options, EShaderPlatform ShaderPlatform);
	/** Loads any existing cache of shader binaries, called by the RHI after full initialisation. */
	static void LoadBinaryCache();
	/** Save binary cache immediately to the given output dir for the given platform. */
	static void SaveBinaryCache(FString OutputDir, FName PlatformName);
	/** Shader cache shutdown, called only by the RHI. */
	static void ShutdownShaderCache();

	/** Get the global shader cache if it exists or nullptr otherwise. */
	static FORCEINLINE FShaderCache* GetShaderCache()
	{
		return bUseShaderCaching ? Cache : nullptr;
	}

	/**Retuns a Cache State object for the Context if we have a ShaderCache - This function is not intended for regular use - objects should cache this result*/
	static FORCEINLINE FShaderCacheState* CreateOrFindCacheStateForContext(const IRHICommandContext* Context)
	{
		if (Cache && Context)
		{
			return Cache->InternalCreateOrFindCacheStateForContext(Context);
		}

		return nullptr;
	}

	static FORCEINLINE void RemoveCacheStateForContext(const IRHICommandContext* Context)
	{
		if (Cache && Context)
		{
			Cache->InternalRemoveCacheStateForContext(Context);
		}
	}

	static FORCEINLINE FShaderCacheState* GetDefaultCacheState()
	{
		if (Cache)
		{
			return Cache->DefaultCacheState;
		}

		return nullptr;
	}

	/** Instantiate or retrieve a vertex shader from the cache for the provided code & hash. */
	FVertexShaderRHIRef GetVertexShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a pixel shader from the cache for the provided code & hash. */
	FPixelShaderRHIRef GetPixelShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a geometry shader from the cache for the provided code & hash. */
	FGeometryShaderRHIRef GetGeometryShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a hull shader from the cache for the provided code & hash. */
	FHullShaderRHIRef GetHullShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a domain shader from the cache for the provided code & hash. */
	FDomainShaderRHIRef GetDomainShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a compute shader from the cache for the provided code & hash. */
	FComputeShaderRHIRef GetComputeShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code);
	/** Instantiate or retrieve a vertex declaration from the cache for the provided layout. */
	FVertexDeclarationRHIParamRef GetVertexDeclaration(FVertexDeclarationElementList& VertexElements);

	/** Instantiate or retrieve a vertex shader from the cache for the provided code & hash. */
	static FORCEINLINE FVertexShaderRHIRef CreateVertexShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetVertexShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreateVertexShader(Platform, Hash, Code);
		}
	}

	/** Instantiate or retrieve a pixel shader from the cache for the provided code & hash. */
	static FORCEINLINE FPixelShaderRHIRef CreatePixelShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetPixelShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreatePixelShader(Platform, Hash, Code);
		}
	}

	/** Instantiate or retrieve a geometry shader from the cache for the provided code & hash. */
	static FORCEINLINE FGeometryShaderRHIRef CreateGeometryShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetGeometryShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreateGeometryShader(Platform, Hash, Code);
		}
	}

	/** Instantiate or retrieve a hull shader from the cache for the provided code & hash. */
	static FORCEINLINE FHullShaderRHIRef CreateHullShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetHullShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreateHullShader(Platform, Hash, Code);
		}
	}

	/** Instantiate or retrieve a domain shader from the cache for the provided code & hash. */
	static FORCEINLINE FDomainShaderRHIRef CreateDomainShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetDomainShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreateDomainShader(Platform, Hash, Code);
		}
	}

	/** Instantiate or retrieve a compute shader from the cache for the provided code & hash. */
	static FORCEINLINE FComputeShaderRHIRef CreateComputeShader(EShaderPlatform Platform, FSHAHash Hash, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			return Cache->GetComputeShader(Platform, Hash, Code);
		}
		else
		{
			return FShaderCodeLibrary::CreateComputeShader(Platform, Hash, Code);
		}
	}

	/** Called by the game. Logs whether a user-defined streaming key is active or disabled, which is used to batch logged draw-states so that they are only predrawn when the same combination of streaming keys are active. This is important for streaming games, or those with very large numbers of shaders, as it will reduce the number of predraw requests performed at any given time. */
	static FORCEINLINE void LogStreamingKey(uint32 StreamingKey, bool const bActive)
	{
		if (Cache)
		{
			Cache->InternalLogStreamingKey(StreamingKey, bActive);
		}
	}

	/** Called by the Engine/RHI. Logs the use of a given shader & will ensure it is instantiated if not already. */
	static FORCEINLINE void LogShader(EShaderPlatform Platform, EShaderFrequency Frequency, FSHAHash Hash, uint32 UncompressedSize, TArray<uint8> const& Code)
	{
		if (Cache)
		{
			Cache->InternalLogShader(Platform, Frequency, Hash, UncompressedSize, Code, nullptr);
		}
	}

	/** Called by the RHI. Logs the user of a vertex declaration so that the cache can find it for recording shader & draw states. */
	static FORCEINLINE void LogVertexDeclaration(FShaderCacheState* CacheState, const FVertexDeclarationElementList& VertexElements, FVertexDeclarationRHIParamRef VertexDeclaration)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogVertexDeclaration(*CacheState, VertexElements, VertexDeclaration);
		}
	}

	/** Called by the RHI. Logs the construction of a PSO & will record it for prebinding on subsequent runs. */
	static FORCEINLINE void LogGraphicsPipelineState(FShaderCacheState* CacheState,
		EShaderPlatform Platform,
		const FGraphicsPipelineStateInitializer& Initializer,
		FGraphicsPipelineStateRHIParamRef PSO)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogGraphicsPipelineState(*CacheState, Platform, Initializer, PSO);
		}
	}

	/** Called by the RHI. Logs the construction of a bound shader state & will record it for prebinding on subsequent runs. */
	static DEPRECATED(4.16, "Use SetGraphicsPipelineState") FORCEINLINE void LogBoundShaderState(FShaderCacheState* CacheState,
		EShaderPlatform Platform,
		FVertexDeclarationRHIParamRef VertexDeclaration,
		FVertexShaderRHIParamRef VertexShader,
		FPixelShaderRHIParamRef PixelShader,
		FHullShaderRHIParamRef HullShader,
		FDomainShaderRHIParamRef DomainShader,
		FGeometryShaderRHIParamRef GeometryShader,
		FBoundShaderStateRHIParamRef BoundState)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogBoundShaderState(*CacheState, Platform, VertexDeclaration, VertexShader, PixelShader, HullShader, DomainShader, GeometryShader, BoundState);
		}
	}

	/** Called by the RHI. Logs the construction of a blend state & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogBlendState(FShaderCacheState* CacheState, FBlendStateInitializerRHI const& Init, FBlendStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogBlendState(*CacheState, Init, State);
		}
	}

	/** Called by the RHI. Logs the construction of a rasterize state & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogRasterizerState(FShaderCacheState* CacheState, FRasterizerStateInitializerRHI const& Init, FRasterizerStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogRasterizerState(*CacheState, Init, State);
		}
	}

	/** Called by the RHI. Logs the construction of a stencil state & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogDepthStencilState(FShaderCacheState* CacheState, FDepthStencilStateInitializerRHI const& Init, FDepthStencilStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogDepthStencilState(*CacheState, Init, State);
		}
	}

	/** Called by the RHI. Logs the construction of a sampelr state & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogSamplerState(FShaderCacheState* CacheState, FSamplerStateInitializerRHI const& Init, FSamplerStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogSamplerState(*CacheState, Init, State);
		}
	}

	/** Called by the RHI. Logs the construction of a texture & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogTexture(FShaderTextureKey const& Init, FTextureRHIParamRef State)
	{
		if (Cache)
		{
			Cache->InternalLogTexture(Init, State);
		}
	}

	/** Called by the RHI. Logs the construction of a SRV & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogSRV(FShaderResourceViewRHIParamRef SRV, FTextureRHIParamRef Texture, uint8 StartMip, uint8 NumMips, uint8 Format)
	{
		if (Cache)
		{
			Cache->InternalLogSRV(SRV, Texture, StartMip, NumMips, Format);
		}
	}

	/** Called by the RHI. Logs the construction of a SRV & caches it so that draw states can be properly recorded. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void LogSRV(FShaderResourceViewRHIParamRef SRV, FVertexBufferRHIParamRef Vb, uint32 Stride, uint8 Format)
	{
		if (Cache)
		{
			Cache->InternalLogSRV(SRV, Vb, Stride, Format);
		}
	}

	/** Called by the RHI. Removes the cached SRV pointer on destruction to prevent further use. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void RemoveSRV(FShaderResourceViewRHIParamRef SRV)
	{
		if (Cache)
		{
			Cache->InternalRemoveSRV(SRV);
		}
	}

	/** Called by the RHI. Removes the cached texture pointer on destruction to prevent further use. No-op when r.UseShaderDrawLog & r.UseShaderPredraw are disabled. */
	static FORCEINLINE void RemoveTexture(FTextureRHIParamRef Texture)
	{
		if (Cache)
		{
			Cache->InternalRemoveTexture(Texture);
		}
	}

	/** Called by the RHI. Records the current blend state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static DEPRECATED(4.16, "Use SetGraphicsPipelineState") FORCEINLINE void SetBlendState(FShaderCacheState* CacheState, FBlendStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetBlendState(*CacheState, State);
		}
	}

	/** Called by the RHI. Records the current rasterizer state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static DEPRECATED(4.16, "Use SetGraphicsPipelineState") FORCEINLINE void SetRasterizerState(FShaderCacheState* CacheState, FRasterizerStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetRasterizerState(*CacheState, State);
		}
	}

	/** Called by the RHI. Records the current depth stencil state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static DEPRECATED(4.16, "Use SetGraphicsPipelineState") FORCEINLINE void SetDepthStencilState(FShaderCacheState* CacheState, FDepthStencilStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetDepthStencilState(*CacheState, State);
		}
	}

	/** Called by the RHI. Records the current render-targets when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetRenderTargets(FShaderCacheState* CacheState, uint32 NumSimultaneousRenderTargets, const FRHIRenderTargetView* NewRenderTargetsRHI, const FRHIDepthRenderTargetView* NewDepthStencilTargetRHI)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetRenderTargets(*CacheState, NumSimultaneousRenderTargets, NewRenderTargetsRHI, NewDepthStencilTargetRHI);
		}
	}

	/** Called by the RHI. Records the current sampler state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetSamplerState(FShaderCacheState* CacheState, EShaderFrequency Frequency, uint32 Index, FSamplerStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetSamplerState(*CacheState, Frequency, Index, State);
		}
	}

	/** Called by the RHI. Records the current texture binding when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetTexture(FShaderCacheState* CacheState, EShaderFrequency Frequency, uint32 Index, FTextureRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetTexture(*CacheState, Frequency, Index, State);
		}
	}

	/** Called by the RHI. Records the current SRV binding when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetSRV(FShaderCacheState* CacheState, EShaderFrequency Frequency, uint32 Index, FShaderResourceViewRHIParamRef SRV)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetSRV(*CacheState, Frequency, Index, SRV);
		}
	}

	/** Called by the RHI. Records the current pipeline state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetGraphicsPipelineStateObject(FShaderCacheState* CacheState, FGraphicsPipelineStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetGraphicsPipelineState(*CacheState, State);
		}
	}

	/** Called by the RHI. Records the current bound shader state when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static DEPRECATED(4.16, "Use SetGraphicsPipelineState") FORCEINLINE void SetBoundShaderState(FShaderCacheState* CacheState, FBoundShaderStateRHIParamRef State)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetBoundShaderState(*CacheState, State);
		}
	}

	/** Called by the RHI. Records the current vieport when r.UseShaderDrawLog or r.UseShaderPredraw are enabled. */
	static FORCEINLINE void SetViewport(FShaderCacheState* CacheState, uint32 MinX, uint32 MinY, float MinZ, uint32 MaxX, uint32 MaxY, float MaxZ)
	{
		if (Cache && CacheState)
		{
			Cache->InternalSetViewport(*CacheState, MinX, MinY, MinZ, MaxX, MaxY, MaxZ);
		}
	}

	/** Called by the RHI. Records the current draw state using the information captured from the other Log/Set* calls if and only if r.UseShaderCaching & r.UseShaderDrawLog are enabled. */
	static FORCEINLINE void LogDraw(FShaderCacheState* CacheState, uint32 PrimitiveType, uint8 IndexType)
	{
		if (Cache && CacheState)
		{
			Cache->InternalLogDraw(*CacheState, PrimitiveType, IndexType);
		}
	}

	/** Called by the RHI. Returns whether the current draw call is a predraw call for shader variant submission in the underlying driver rather than a real UE4 draw call. */
	static FORCEINLINE bool IsPredrawCall(FShaderCacheState const* CacheState)
	{
		return CacheState && CacheState->bIsPreDraw;
	}

	/** Called by the RHI. Returns whether the current CreateBSS is a prebind call for shader  submission to the underlying driver rather than a real UE4 CreateBSS call. */
	static FORCEINLINE bool IsPrebindCall(FShaderCacheState const* CacheState)
	{
		return CacheState && CacheState->bIsPreBind;
	}

	/** Use the accelerated target precompile frame time (in ms) and predraw batch time (in ms) during a loading screen or other scenario where frame time can be much slower than normal. */
	static void BeginAcceleratedBatching();

	/** Stops applying the overrides applied with BeginAcceleratedBatching but doesn't flush outstanding work. */
	static void EndAcceleratedBatching();

	/** Flushes all outstanding precompilation/predraw work & then ends accelerated batching as per EndAcceleratedBatching. */
	static void FlushOutstandingBatches();

	/** Pauses precompilation & predraw batching. */
	static void PauseBatching();

	/** Resumes precompilation & predraw batching. */
	static void ResumeBatching();

	/** Returns the number of shaders waiting for precompilation */
	static uint32 NumShaderPrecompilesRemaining();

public: // From FTickableObjectRenderThread
	virtual void Tick(float DeltaTime) final override;

	virtual bool IsTickable() const final override;

	virtual bool NeedsRenderingResumedForRenderingThreadTick() const final override;

	//virtual TStatId GetStatId() const final override;

private:
	void SaveAll();
	void OnAppDeactivate();

	static bool LoadShaderCache(FString Path, FShaderPlatformCache& Cache);
	static bool SaveShaderCache(FString Path, FShaderPlatformCache& Cache);

	void InternalLogStreamingKey(uint32 StreamKey, bool const bActive);

	void InternalLogVertexDeclaration(const FShaderCacheState& CacheState, const FVertexDeclarationElementList& VertexElements, FVertexDeclarationRHIParamRef VertexDeclaration);
	void InternalLogGraphicsPipelineState(const FShaderCacheState& CacheState,
		EShaderPlatform Platform,
		const FGraphicsPipelineStateInitializer& Initializer,
		FGraphicsPipelineStateRHIParamRef PSO);
	void InternalPrelockedLogGraphicsPipelineState(EShaderPlatform Platform,
		const FGraphicsPipelineStateInitializer& Initializer,
		FGraphicsPipelineStateRHIParamRef PSO);
	void InternalLogBoundShaderState(const FShaderCacheState& CacheState,
		EShaderPlatform Platform, FVertexDeclarationRHIParamRef VertexDeclaration,
		FVertexShaderRHIParamRef VertexShader,
		FPixelShaderRHIParamRef PixelShader,
		FHullShaderRHIParamRef HullShader,
		FDomainShaderRHIParamRef DomainShader,
		FGeometryShaderRHIParamRef GeometryShader,
		FBoundShaderStateRHIParamRef BoundState);

	int32 InternalPrelockedLogBoundShaderState(EShaderPlatform Platform, FVertexDeclarationRHIParamRef VertexDeclaration,
		FVertexShaderRHIParamRef VertexShader,
		FPixelShaderRHIParamRef PixelShader,
		FHullShaderRHIParamRef HullShader,
		FDomainShaderRHIParamRef DomainShader,
		FGeometryShaderRHIParamRef GeometryShader,
		FBoundShaderStateRHIParamRef BoundState);

	void InternalLogBlendState(FShaderCacheState const& CacheState, FBlendStateInitializerRHI const& Init, FBlendStateRHIParamRef State);
	void InternalLogRasterizerState(FShaderCacheState const& CacheState, FRasterizerStateInitializerRHI const& Init, FRasterizerStateRHIParamRef State);
	void InternalLogDepthStencilState(FShaderCacheState const& CacheState, FDepthStencilStateInitializerRHI const& Init, FDepthStencilStateRHIParamRef State);
	void InternalLogSamplerState(FShaderCacheState const& CacheState, FSamplerStateInitializerRHI const& Init, FSamplerStateRHIParamRef State);
	void InternalLogTexture(FShaderTextureKey const& Init, FTextureRHIParamRef State);
	void InternalLogSRV(FShaderResourceViewRHIParamRef SRV, FTextureRHIParamRef Texture, uint8 StartMip, uint8 NumMips, uint8 Format);
	void InternalLogSRV(FShaderResourceViewRHIParamRef SRV, FVertexBufferRHIParamRef Vb, uint32 Stride, uint8 Format);

	void InternalRemoveSRV(FShaderResourceViewRHIParamRef SRV);
	void InternalRemoveTexture(FTextureRHIParamRef Texture);

	void InternalSetBlendState(FShaderCacheState& CacheState, FBlendStateRHIParamRef State);
	void InternalSetRasterizerState(FShaderCacheState& CacheState, FRasterizerStateRHIParamRef State);
	void InternalSetDepthStencilState(FShaderCacheState& CacheState, FDepthStencilStateRHIParamRef State);
	void InternalSetRenderTargets(FShaderCacheState& CacheState, uint32 NumSimultaneousRenderTargets, const FRHIRenderTargetView* NewRenderTargetsRHI, const FRHIDepthRenderTargetView* NewDepthStencilTargetRHI);
	void InternalSetSamplerState(FShaderCacheState& CacheState, EShaderFrequency Frequency, uint32 Index, FSamplerStateRHIParamRef State);
	void InternalSetTexture(FShaderCacheState& CacheState, EShaderFrequency Frequency, uint32 Index, FTextureRHIParamRef State);
	void InternalSetSRV(FShaderCacheState& CacheState, EShaderFrequency Frequency, uint32 Index, FShaderResourceViewRHIParamRef SRV);
	void InternalSetBoundShaderState(FShaderCacheState& CacheState, FBoundShaderStateRHIParamRef State);
	void InternalSetViewport(FShaderCacheState& CacheState, uint32 MinX, uint32 MinY, float MinZ, uint32 MaxX, uint32 MaxY, float MaxZ);
	void InternalSetGraphicsPipelineState(FShaderCacheState& CacheState, FGraphicsPipelineStateRHIParamRef State);

	void InternalLogDraw(FShaderCacheState& CacheState, uint32 PrimitiveType, uint8 IndexType);
	void InternalPreDrawShaders(FRHICommandList& RHICmdList, float DeltaTime);

	void InternalLogShader(EShaderPlatform Platform, EShaderFrequency Frequency, FSHAHash Hash, uint32 UncompressedSize, TArray<uint8> const& Code, FShaderCacheState* CacheState);
	void InternalPrebindShader(FShaderCacheKey const& Key, FShaderCacheState* CacheState);
	void InternalSubmitShader(FShaderCacheKey const& Key, TArray<uint8> const& Code, FRHIShaderLibrary* Library, FShaderCacheState * CacheState);
	void InternalPreDrawShader(FRHICommandList& RHICmdList, FShaderCacheBoundState const& Shader, TSet<int32> const& DrawStates);
	template <typename TShaderRHIRef>
	void InternalSetShaderSamplerTextures(FRHICommandList& RHICmdList, FShaderDrawKey const& DrawKey, EShaderFrequency Frequency, TShaderRHIRef Shader, bool bClear = false);
	FTextureRHIRef InternalCreateTexture(FShaderTextureKey const& TextureKey, bool const bCached);
	FShaderTextureBinding InternalCreateSRV(FShaderResourceKey const& ResourceKey);
	FTextureRHIRef InternalCreateRenderTarget(FShaderRenderTargetKey const& TargetKey);

	int32 GetPredrawBatchTime() const;
	int32 GetTargetPrecompileFrameTime() const;

	FShaderCacheState* InternalCreateOrFindCacheStateForContext(const IRHICommandContext* Context);
	void InternalRemoveCacheStateForContext(const IRHICommandContext* Context);

	bool ShouldPreDrawShaders(int64 CurrentPreDrawTime) const;

private:
	//struct FShaderCacheOpenGLDrawState
	//{
	TMap<FSamplerStateRHIParamRef, int32> SamplerStates;
	TMap<FTextureRHIParamRef, int32> Textures;
	TMap<FShaderResourceViewRHIParamRef, FShaderResourceKey> SRVs;

	// Caches to track application & predraw created textures/SRVs so that we minimise temporary resource creation
	TMap<FShaderTextureKey, FTextureRHIParamRef> CachedTextures;
	TMap<FShaderResourceKey, FShaderResourceViewBinding> CachedSRVs;

	// Temporary shader resources for pre-draw
	// Cleared after each round of pre-drawing is complete
	TSet<FShaderTextureBinding> PredrawBindings;
	TMap<FShaderRenderTargetKey, FTextureRHIParamRef> PredrawRTs;
	TSet<FVertexBufferRHIRef> PredrawVBs;

	// Permanent shader pre-draw resources
	FIndexBufferRHIRef IndexBufferUInt16;
	FIndexBufferRHIRef IndexBufferUInt32;

	// Growable pre-draw resources
	FVertexBufferRHIRef PredrawVB; // Standard VB
	FVertexBufferRHIRef PredrawZVB; // Zero-stride VB
									//};

private:
	EShaderPlatform CurrentPlatform;

	// Serialized
	FShaderPlatformCache CurrentShaderPlatformCache;

	// Optional, separate runtime code cache
	class FShaderCacheLibrary* CodeCache;

	// All the pipeline states loaded from the libraries - only valid for OpenGL.
	TMap<FShaderCacheKey, TSet<FShaderPipelineKey>> Pipelines;

	// Transient non-invasive tracking of RHI resources for shader logging
	TMap<FShaderCacheKey, FVertexShaderRHIRef> CachedVertexShaders;
	TMap<FShaderCacheKey, FPixelShaderRHIRef> CachedPixelShaders;
	TMap<FShaderCacheKey, FGeometryShaderRHIRef> CachedGeometryShaders;
	TMap<FShaderCacheKey, FHullShaderRHIRef> CachedHullShaders;
	TMap<FShaderCacheKey, FDomainShaderRHIRef> CachedDomainShaders;
	TMap<FShaderCacheKey, FComputeShaderRHIRef> CachedComputeShaders;
	TMap<FVertexDeclarationRHIParamRef, FVertexDeclarationElementList> VertexDeclarations;
	TMap<FShaderCacheBoundState, FBoundShaderStateRHIRef> BoundShaderStates;
	TMap<FShaderCacheGraphicsPipelineState, FGraphicsPipelineStateRHIRef> GraphicsPSOs;

	// Transient non-invasive tracking of RHI resources for shader predrawing
	TMap<FBlendStateRHIParamRef, FBlendStateInitializerRHI> BlendStates;
	TMap<FRasterizerStateRHIParamRef, FRasterizerStateInitializerRHI> RasterizerStates;
	TMap<FDepthStencilStateRHIParamRef, FDepthStencilStateInitializerRHI> DepthStencilStates;
	TMap<FBoundShaderStateRHIParamRef, FShaderCacheBoundState> ShaderStates;
	TMap<FGraphicsPipelineStateRHIParamRef, FShaderCacheGraphicsPipelineState> GraphicsPSOStates;

	// Active streaming keys
	TSet<uint32> ActiveStreamingKeys;

	// Current combination of streaming keys that define the current streaming environment.
	// Logged draws will only be predrawn when this key becomes active.
	uint32 StreamingKey;

	// Shaders to precompile - List of Shader Library Iterators currently PreCompiling
	uint32 ShadersToPrecompile;
	TArray<TRefCountPtr<FRHIShaderLibrary::FShaderLibraryIterator>> ShaderLibraryPreCompileProgress;

	// Shaders we need to predraw
	TMap<uint32, FShaderStreamingCache> ShadersToDraw;

	// OpenGL specific functionality
	// FShaderCacheOpenGLDrawState OpenGLDrawState;

	//Cache Options
	uint32 Options;

	FGraphicsPipelineStateInitializer GraphicsPSOInit;

	// Overrides for shader warmup times to use when loading or to force a flush.
	int32 OverridePrecompileTime;
	int32 OverridePredrawBatchTime;
	bool bBatchingPaused;

	//Thread access mutual exclusion
	mutable FRWLock ShaderCacheGlobalStateMutex;
	mutable FRWLock PipelineStateMutex;
	mutable FRWLock DrawLogMutex;
	mutable FRWLock ContextCacheStatesMutex;

	//List of states per RHI context with a default state
	FShaderCacheState* DefaultCacheState;
	mutable TMap<const IRHICommandContext*, FShaderCacheState*> ContextCacheStates;

	IConsoleObject* SaveShaderCacheCmd;

	static FShaderCache* Cache;
	static int32 GameVersion;
	static int32 bUseShaderCaching;
	static int32 bUseUserShaderCache;
	static int32 bUseShaderPredraw;
	static int32 bUseShaderDrawLog;
	static int32 PredrawBatchTime;
	static int32 bUseShaderBinaryCache;
	static int32 bUseAsyncShaderPrecompilation;
	static int32 TargetPrecompileFrameTime;
	static int32 AccelPredrawBatchTime;
	static int32 AccelTargetPrecompileFrameTime;
	static float InitialShaderLoadTime;
	static uint32 MaxTextureSamplers;
	static uint8 MaxResources;
// 	static FAutoConsoleVariableRef CVarUseShaderCaching;
// 	static FAutoConsoleVariableRef CVarUseUserShaderCache;
// 	static FAutoConsoleVariableRef CVarUseShaderPredraw;
// 	static FAutoConsoleVariableRef CVarUseShaderDrawLog;
// 	static FAutoConsoleVariableRef CVarPredrawBatchTime;
// 	static FAutoConsoleVariableRef CVarUseShaderBinaryCache;
// 	static FAutoConsoleVariableRef CVarUseAsyncShaderPrecompilation;
// 	static FAutoConsoleVariableRef CVarTargetPrecompileFrameTime;
// 	static FAutoConsoleVariableRef CVarAccelPredrawBatchTime;
// 	static FAutoConsoleVariableRef CVarAccelTargetPrecompileFrameTime;
// 	static FAutoConsoleVariableRef CVarInitialShaderLoadTime;
};
