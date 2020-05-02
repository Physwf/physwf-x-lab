#pragma once

#include "CoreMinimal.h"
//#include "Stats/Stats.h"
#include "RHI/RHI.h"
#include "RenderCore/TickableObjectRenderThread.h"
#include "RHI/PipelineFileCache.h"
//#include "Delegates/DelegateCombinations.h"

#include "X4.h"

class FShaderPipelineCacheArchive;

class X4_API FShaderPipelineCache : public FTickableObjectRenderThread
{
	struct CompileJob
	{
		FPipelineCacheFileFormatPSO PSO;
		FShaderPipelineCacheArchive* ReadRequests;
	};

public:
	/**
	* Initializes the shader pipeline cache for the desired platform, called by the engine.
	* The shader platform is used only to load the initial pipeline cache and can be changed by closing & reopening the cache if necessary.
	*/
	static void Initialize(EShaderPlatform Platform);
	/** Terminates the shader pipeline cache, called by the engine. */
	static void Shutdown();

	/** Pauses precompilation. */
	static void PauseBatching();

	enum class BatchMode
	{
		Background, // The maximum batch size is defined by r.ShaderPipelineCache.BackgroundBatchSize
		Fast // The maximum batch size is defined by r.ShaderPipelineCache.BatchSize
	};

	/** Sets the precompilation batching mode. */
	static void SetBatchMode(BatchMode Mode);

	/** Resumes precompilation batching. */
	static void ResumeBatching();

	/** Returns the number of pipelines waiting for precompilation. */
	static uint32 NumPrecompilesRemaining();

	/** Returns the number of pipelines actively being precompiled this frame. */
	static uint32 NumPrecompilesActive();

	/** Opens the shader pipeline cache file with the given name and shader platform. */
	static bool OpenPipelineFileCache(FString const& Name, EShaderPlatform Platform);

	/** Saves the current shader pipeline cache to disk using one of the defined save modes, Fast uses an incremental approach whereas Slow will consolidate all data into the file. */
	static bool SavePipelineFileCache(FPipelineFileCache::SaveMode Mode);

	/** Closes the existing pipeline cache, allowing it to be reopened with a different file and/or shader platform. Will implicitly invoke a Fast Save. */
	static void ClosePipelineFileCache();

	static int32 GetGameVersionForPSOFileCache();

	FShaderPipelineCache(EShaderPlatform Platform);
	virtual ~FShaderPipelineCache();

	virtual bool IsTickable() const override final;

	virtual void Tick(float DeltaTime) override final;

	virtual bool NeedsRenderingResumedForRenderingThreadTick() const override final;

	//virtual TStatId GetStatId() const override final;

	enum ELibraryState
	{
		Opened,
		Closed
	};

	/** Called by FShaderCodeLibrary to notify us that the shader code library state changed and shader availability will need to be re-evaluated */
	static void ShaderLibraryStateChanged(ELibraryState State, EShaderPlatform Platform, FString const& Name);

	/**
	* Delegate signature for being notified when a pipeline cache is opened
	*/
	//DECLARE_MULTICAST_DELEGATE_ThreeParams(FShaderCacheOpenedDelegate, FString const& /* Name */, EShaderPlatform /* Platform*/, uint32 /* Count */);

	/**
	* Gets the event delegate to register to to be notified when a pipeline cache is opened.
	*/
	//static FShaderCacheOpenedDelegate& GetCacheOpenedDelegate() { return OnCachedOpened; }

	/**
	* Delegate signature for being notified when a pipeline cache is closed
	*/
	//DECLARE_MULTICAST_DELEGATE_TwoParams(FShaderCacheClosedDelegate, FString const& /* Name */, EShaderPlatform /* Platform*/);

	/**
	* Gets the event delegate to register to to be notified when a pipeline cache is closed.
	*/
	//static FShaderCacheClosedDelegate& GetCacheClosedDelegate() { return OnCachedClosed; }

	/**
	* Delegate signature for being notified that all currently viable PSOs have been precompiled from the cache, how many that was and how long spent precompiling (in sec.)
	*/
	//DECLARE_MULTICAST_DELEGATE_TwoParams(FShaderPrecompilationCompleteDelegate, uint32 /** Count */, double /** Seconds */);

	/**
	* Gets the event delegate to register to to be notified when all currently viable PSOs have been precompiled from the cache.
	*/
	//static FShaderPrecompilationCompleteDelegate& GetPrecompilationCompleteDelegate() { return OnPrecompilationComplete; }

private:
	bool Open(FString const& Name, EShaderPlatform Platform);
	bool Save(FPipelineFileCache::SaveMode Mode);
	void Close();

	bool Precompile(FRHICommandListImmediate& RHICmdList, EShaderPlatform Platform, FPipelineCacheFileFormatPSO const& PSO);
	void PreparePipelineBatch(TArray<FPipelineCacheFileFormatPSORead*>& PipelineBatch);
	bool ReadyForPrecompile();
	void PrecompilePipelineBatch();
	bool ReadyForNextBatch() const;
	bool ReadyForAutoSave() const;
	void PollShutdownItems();
	void Flush();

	void OnShaderLibraryStateChanged(ELibraryState State, EShaderPlatform Platform, FString const& Name);

private:
	static FShaderPipelineCache* ShaderPipelineCache;
	TArray<CompileJob> ReadTasks;
	TArray<CompileJob> CompileTasks;
	TArray<FPipelineCachePSOHeader> OrderedCompileTasks;
	TArray<FPipelineCacheFileFormatPSORead*> FetchTasks;
	TSet<uint32> CompiledHashes;
	FString FileName;
	EShaderPlatform CurrentPlatform;
	uint32 BatchSize;
	bool bPaused;
	bool bOpened;

	volatile int64 TotalActiveTasks;
	volatile int64 TotalWaitingTasks;
	volatile int64 TotalCompleteTasks;
	volatile int64 TotalPrecompileTime;
	FCriticalSection Mutex;
	TArray<FPipelineCachePSOHeader> PreFetchedTasks;

	TArray<CompileJob> ShutdownReadTasks;
	TArray<FPipelineCacheFileFormatPSORead*> ShutdownFetchTasks;

	//static FShaderCacheOpenedDelegate OnCachedOpened;
	//static FShaderCacheClosedDelegate OnCachedClosed;
	//static FShaderPrecompilationCompleteDelegate OnPrecompilationComplete;

	double LastAutoSaveTime;
	double LastAutoSaveTimeLogBoundPSO;
	int32 LastAutoSaveNum;
};