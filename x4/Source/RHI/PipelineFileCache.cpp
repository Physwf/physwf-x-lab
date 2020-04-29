#include "RHI/PipelineFileCache.h"
#include "RHI/PipelineStateCache.h"
//#include "HAL/FileManager.h"
//#include "Misc/EngineVersion.h"
//#include "Serialization/Archive.h"
//#include "Serialization/MemoryReader.h"
//#include "Serialization/MemoryWriter.h"
#include "RHI/RHI.h"
#include "RHI/RHIResources.h"
#include "Misc/ScopeRWLock.h"
//#include "Misc/Paths.h"
//#include "Async/AsyncFileHandle.h"
//#include "HAL/PlatformFilemanager.h"
//#include "Misc/FileHelper.h"

void FRHIComputeShader::UpdateStats()
{
	FPipelineStateStats::UpdateStats(Stats);
}

void FPipelineStateStats::UpdateStats(FPipelineStateStats* Stats)
{
	if (Stats)
	{
		FPlatformAtomics::InterlockedExchange(&Stats->LastFrameUsed, GFrameCounter);
		FPlatformAtomics::InterlockedIncrement(&Stats->TotalBindCount);
		FPlatformAtomics::InterlockedCompareExchange(&Stats->FirstFrameUsed, GFrameCounter, -1);
	}
}


FString FPipelineCacheFileFormatPSO::ComputeDescriptor::ToString() const
{
	return ComputeShader.ToString();
}

void FPipelineCacheFileFormatPSO::ComputeDescriptor::FromString(const FString& Src)
{
	ComputeShader.FromString(Src.TrimStartAndEnd());
}

FString FPipelineCacheFileFormatPSO::ComputeDescriptor::HeaderLine()
{
	return FString(TEXT("ComputeShader"));
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::ShadersToString() const
{
	FString Result;

	Result += FString::Printf(TEXT("%s,%s,%s,%s,%s")
		, *VertexShader.ToString()
		, *FragmentShader.ToString()
		, *GeometryShader.ToString()
		, *HullShader.ToString()
		, *DomainShader.ToString()
	);

	return Result;
}
void FPipelineCacheFileFormatPSO::GraphicsDescriptor::ShadersFromString(const FString& Src)
{
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(","));
	int32 PartIndex = 0;
	check(Parts.Num() == 5); //not a very robust parser

	VertexShader.FromString(Parts[PartIndex++]);
	FragmentShader.FromString(Parts[PartIndex++]);
	GeometryShader.FromString(Parts[PartIndex++]);
	HullShader.FromString(Parts[PartIndex++]);
	DomainShader.FromString(Parts[PartIndex++]);
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::ShaderHeaderLine()
{
	return FString(TEXT("VertexShader,FragmentShader,GeometryShader,HullShader,DomainShader"));
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::StateToString() const
{
	FString Result;

	Result += FString::Printf(TEXT("%s,%s,%s,")
		, *BlendState.ToString()
		, *RasterizerState.ToString()
		, *DepthStencilState.ToString()
	);
	Result += FString::Printf(TEXT("%d,%d,%d,")
		, MSAASamples
		, uint32(DepthStencilFormat)
		, DepthStencilFlags
	);
	Result += FString::Printf(TEXT("%d,%d,%d,%d,%d,")
		, uint32(DepthLoad)
		, uint32(StencilLoad)
		, uint32(DepthStore)
		, uint32(StencilStore)
		, uint32(PrimitiveType)
	);

	Result += FString::Printf(TEXT("%d,")
		, RenderTargetsActive
	);
	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; Index++)
	{
		Result += FString::Printf(TEXT("%d,%d,%d,%d,")
			, uint32(RenderTargetFormats[Index])
			, RenderTargetFlags[Index]
			, uint32(RenderTargetsLoad[Index])
			, uint32(RenderTargetsStore[Index])
		);
	}

	FVertexElement NullVE;
	FMemory::Memzero(NullVE);
	Result += FString::Printf(TEXT("%d,")
		, VertexDescriptor.Num()
	);
	for (int32 Index = 0; Index < MaxVertexElementCount; Index++)
	{
		if (Index < VertexDescriptor.Num())
		{
			Result += FString::Printf(TEXT("%s,")
				, *VertexDescriptor[Index].ToString()
			);
		}
		else
		{
			Result += FString::Printf(TEXT("%s,")
				, *NullVE.ToString()
			);
		}
	}
	return Result.Left(Result.Len() - 1); // remove trailing comma
}
void FPipelineCacheFileFormatPSO::GraphicsDescriptor::StateFromString(const FString& Src)
{
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(","));
	int32 PartIndex = 0;




	check(Parts.Num() - PartIndex >= 3); //not a very robust parser
	BlendState.FromString(Parts[PartIndex++]);
	RasterizerState.FromString(Parts[PartIndex++]);
	DepthStencilState.FromString(Parts[PartIndex++]);

	check(Parts.Num() - PartIndex >= 3 && sizeof(EPixelFormat) == sizeof(uint32)); //not a very robust parser
	LexFromString(MSAASamples, *Parts[PartIndex++]);
	LexFromString((uint32&)DepthStencilFormat, *Parts[PartIndex++]);
	LexFromString(DepthStencilFlags, *Parts[PartIndex++]);

	check(Parts.Num() - PartIndex >= 5 && sizeof(DepthLoad) == 1 && sizeof(StencilLoad) == 1 && sizeof(DepthStore) == 1 && sizeof(StencilStore) == 1 && sizeof(PrimitiveType) == 4); //not a very robust parser
	LexFromString((uint32&)DepthLoad, *Parts[PartIndex++]);
	LexFromString((uint32&)StencilLoad, *Parts[PartIndex++]);
	LexFromString((uint32&)DepthStore, *Parts[PartIndex++]);
	LexFromString((uint32&)StencilStore, *Parts[PartIndex++]);
	LexFromString((uint32&)PrimitiveType, *Parts[PartIndex++]);

	check(Parts.Num() - PartIndex >= 1); //not a very robust parser
	LexFromString(RenderTargetsActive, *Parts[PartIndex++]);

	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; Index++)
	{
		check(Parts.Num() - PartIndex >= 4 && sizeof(ERenderTargetLoadAction) == 1 && sizeof(ERenderTargetStoreAction) == 1 && sizeof(EPixelFormat) == sizeof(uint32)); //not a very robust parser
		LexFromString((uint32&)(RenderTargetFormats[Index]), *Parts[PartIndex++]);
		LexFromString(RenderTargetFlags[Index], *Parts[PartIndex++]);
		LexFromString((uint8&)RenderTargetsLoad[Index], *Parts[PartIndex++]);
		LexFromString((uint8&)RenderTargetsStore[Index], *Parts[PartIndex++]);
	}

	check(Parts.Num() - PartIndex >= 1); //not a very robust parser
	int32 VertDescNum = 0;
	LexFromString(VertDescNum, *Parts[PartIndex++]);
	check(VertDescNum >= 0 && VertDescNum <= MaxVertexElementCount);

	VertexDescriptor.Empty(VertDescNum);
	VertexDescriptor.AddZeroed(VertDescNum);

	check(Parts.Num() - PartIndex == MaxVertexElementCount); //not a very robust parser
	for (int32 Index = 0; Index < VertDescNum; Index++)
	{
		VertexDescriptor[Index].FromString(Parts[PartIndex++]);
	}

	VertexDescriptor.Sort([](FVertexElement const& A, FVertexElement const& B)
		{
			if (A.StreamIndex < B.StreamIndex)
			{
				return true;
			}
			if (A.StreamIndex > B.StreamIndex)
			{
				return false;
			}
			if (A.Offset < B.Offset)
			{
				return true;
			}
			if (A.Offset > B.Offset)
			{
				return false;
			}
			if (A.AttributeIndex < B.AttributeIndex)
			{
				return true;
			}
			if (A.AttributeIndex > B.AttributeIndex)
			{
				return false;
			}
			return false;
		});
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::StateHeaderLine()
{
	FString Result;

	Result += FString::Printf(TEXT("%s,%s,%s,")
		, TEXT("BlendState")
		, TEXT("RasterizerState")
		, TEXT("DepthStencilState")
	);
	Result += FString::Printf(TEXT("%s,%s,%s,")
		, TEXT("MSAASamples")
		, TEXT("DepthStencilFormat")
		, TEXT("DepthStencilFlags")
	);
	Result += FString::Printf(TEXT("%s,%s,%s,%s,%s,")
		, TEXT("DepthLoad")
		, TEXT("StencilLoad")
		, TEXT("DepthStore")
		, TEXT("StencilStore")
		, TEXT("PrimitiveType")
	);

	Result += FString::Printf(TEXT("%s,")
		, TEXT("RenderTargetsActive")
	);
	for (int32 Index = 0; Index < MaxSimultaneousRenderTargets; Index++)
	{
		Result += FString::Printf(TEXT("%s%d,%s%d,%s%d,%s%d,")
			, TEXT("RenderTargetFormats"), Index
			, TEXT("RenderTargetFlags"), Index
			, TEXT("RenderTargetsLoad"), Index
			, TEXT("RenderTargetsStore"), Index
		);
	}

	Result += FString::Printf(TEXT("%s,")
		, TEXT("VertexDescriptorNum")
	);
	for (int32 Index = 0; Index < MaxVertexElementCount; Index++)
	{
		Result += FString::Printf(TEXT("%s%d,")
			, TEXT("VertexDescriptor"), Index
		);
	}
	return Result.Left(Result.Len() - 1); // remove trailing comma
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::ToString() const
{
	return FString::Printf(TEXT("%s,%s"), *ShadersToString(), *StateToString());
}

void FPipelineCacheFileFormatPSO::GraphicsDescriptor::FromString(const FString& Src)
{
	static const int32 NumShaderParts = 5;
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(","));
	check(Parts.Num() > NumShaderParts);
	TArray<FString> StateParts(Parts);
	StateParts.RemoveAt(0, NumShaderParts);
	Parts.RemoveAt(NumShaderParts, Parts.Num() - 5);
	ShadersFromString(FString::Join(Parts, TEXT(",")));
	StateFromString(FString::Join(StateParts, TEXT(",")));
}

FString FPipelineCacheFileFormatPSO::GraphicsDescriptor::HeaderLine()
{
	return FString::Printf(TEXT("%s,%s"), *ShaderHeaderLine(), *StateHeaderLine());
}



/**
  * FPipelineCacheFileFormatPSO
  **/

/*friend*/ uint32 GetTypeHash(const FPipelineCacheFileFormatPSO &Key)
{
	if (FPlatformAtomics::AtomicRead((volatile int32*)&Key.Hash) == 0)
	{
		uint32 KeyHash = GetTypeHash(Key.Type);
		switch (Key.Type)
		{
		case FPipelineCacheFileFormatPSO::DescriptorType::Compute:
		{
			KeyHash ^= GetTypeHash(Key.ComputeDesc.ComputeShader);
			break;
		}
		case FPipelineCacheFileFormatPSO::DescriptorType::Graphics:
		{
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RenderTargetsActive, sizeof(Key.GraphicsDesc.RenderTargetsActive), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.MSAASamples, sizeof(Key.GraphicsDesc.MSAASamples), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.PrimitiveType, sizeof(Key.GraphicsDesc.PrimitiveType), KeyHash);

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.VertexShader.Hash, sizeof(Key.GraphicsDesc.VertexShader.Hash), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.FragmentShader.Hash, sizeof(Key.GraphicsDesc.FragmentShader.Hash), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.GeometryShader.Hash, sizeof(Key.GraphicsDesc.GeometryShader.Hash), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.HullShader.Hash, sizeof(Key.GraphicsDesc.HullShader.Hash), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DomainShader.Hash, sizeof(Key.GraphicsDesc.DomainShader.Hash), KeyHash);

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilFormat, sizeof(Key.GraphicsDesc.DepthStencilFormat), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilFlags, sizeof(Key.GraphicsDesc.DepthStencilFlags), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthLoad, sizeof(Key.GraphicsDesc.DepthLoad), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.StencilLoad, sizeof(Key.GraphicsDesc.StencilLoad), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStore, sizeof(Key.GraphicsDesc.DepthStore), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.StencilStore, sizeof(Key.GraphicsDesc.StencilStore), KeyHash);

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.bUseIndependentRenderTargetBlendStates, sizeof(Key.GraphicsDesc.BlendState.bUseIndependentRenderTargetBlendStates), KeyHash);
			for (uint32 i = 0; i < MaxSimultaneousRenderTargets; i++)
			{
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].ColorBlendOp, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].ColorBlendOp), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].ColorSrcBlend, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].ColorSrcBlend), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].ColorDestBlend, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].ColorDestBlend), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].ColorWriteMask, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].ColorWriteMask), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaBlendOp, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaBlendOp), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaSrcBlend, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaSrcBlend), KeyHash);
				KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaDestBlend, sizeof(Key.GraphicsDesc.BlendState.RenderTargets[i].AlphaDestBlend), KeyHash);
			}

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RenderTargetFormats, sizeof(Key.GraphicsDesc.RenderTargetFormats), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RenderTargetFlags, sizeof(Key.GraphicsDesc.RenderTargetFlags), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RenderTargetsLoad, sizeof(Key.GraphicsDesc.RenderTargetsLoad), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RenderTargetsStore, sizeof(Key.GraphicsDesc.RenderTargetsStore), KeyHash);

			for (auto const& Element : Key.GraphicsDesc.VertexDescriptor)
			{
				KeyHash = FCrc::MemCrc32(&Element, sizeof(FVertexElement), KeyHash);
			}

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.DepthBias, sizeof(Key.GraphicsDesc.RasterizerState.DepthBias), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.SlopeScaleDepthBias, sizeof(Key.GraphicsDesc.RasterizerState.SlopeScaleDepthBias), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.FillMode, sizeof(Key.GraphicsDesc.RasterizerState.FillMode), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.CullMode, sizeof(Key.GraphicsDesc.RasterizerState.CullMode), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.bAllowMSAA, sizeof(Key.GraphicsDesc.RasterizerState.bAllowMSAA), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.RasterizerState.bEnableLineAA, sizeof(Key.GraphicsDesc.RasterizerState.bEnableLineAA), KeyHash);

			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.bEnableDepthWrite, sizeof(Key.GraphicsDesc.DepthStencilState.bEnableDepthWrite), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.DepthTest, sizeof(Key.GraphicsDesc.DepthStencilState.DepthTest), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.bEnableFrontFaceStencil, sizeof(Key.GraphicsDesc.DepthStencilState.bEnableFrontFaceStencil), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.FrontFaceStencilTest, sizeof(Key.GraphicsDesc.DepthStencilState.FrontFaceStencilTest), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.FrontFaceStencilFailStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.FrontFaceStencilFailStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.FrontFaceDepthFailStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.FrontFaceDepthFailStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.FrontFacePassStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.FrontFacePassStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.bEnableBackFaceStencil, sizeof(Key.GraphicsDesc.DepthStencilState.bEnableBackFaceStencil), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.BackFaceStencilTest, sizeof(Key.GraphicsDesc.DepthStencilState.BackFaceStencilTest), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.BackFaceStencilFailStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.BackFaceStencilFailStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.BackFaceDepthFailStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.BackFaceDepthFailStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.BackFacePassStencilOp, sizeof(Key.GraphicsDesc.DepthStencilState.BackFacePassStencilOp), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.StencilReadMask, sizeof(Key.GraphicsDesc.DepthStencilState.StencilReadMask), KeyHash);
			KeyHash = FCrc::MemCrc32(&Key.GraphicsDesc.DepthStencilState.StencilWriteMask, sizeof(Key.GraphicsDesc.DepthStencilState.StencilWriteMask), KeyHash);

			break;
		}
		default:
		{
			check(false);
			break;
		}
		}
		FPlatformAtomics::InterlockedCompareExchange((volatile int32*)&Key.Hash, KeyHash, 0);
	}
	return FPlatformAtomics::AtomicRead((volatile int32*)&Key.Hash);
}

FRWLock FPipelineFileCache::FileCacheLock;
FPipelineCacheFile* FPipelineFileCache::FileCache = nullptr;
TMap<uint32, uint32> FPipelineFileCache::RunTimeToFileHashes;
TMap<uint32, FPipelineStateStats*> FPipelineFileCache::Stats;
TSet<FPipelineCacheFileFormatPSO> FPipelineFileCache::NewPSOs;
uint32 FPipelineFileCache::NumNewPSOs;
FPipelineFileCache::PSOOrder FPipelineFileCache::RequestedOrder = FPipelineFileCache::PSOOrder::Default;
bool FPipelineFileCache::FileCacheEnabled = false;
//FPipelineFileCache::FPipelineStateLoggedEvent FPipelineFileCache::PSOLoggedEvent;

void FPipelineFileCache::Shutdown()
{
	if (IsPipelineFileCacheEnabled())
	{
		FRWScopeLock Lock(FileCacheLock, SLT_Write);
		for (auto const& Pair : Stats)
		{
			delete Pair.Value;
		}
		Stats.Empty();
		NewPSOs.Empty();
		NumNewPSOs = 0;

		FileCacheEnabled = false;

		//SET_MEMORY_STAT(STAT_NewCachedPSOMemory, 0);
		//SET_MEMORY_STAT(STAT_PSOStatMemory, 0);
	}
}

void FPipelineFileCache::CacheGraphicsPSO(uint32 RunTimeHash, FGraphicsPipelineStateInitializer const& Initializer)
{
#if 0
	if (IsPipelineFileCacheEnabled() && (LogPSOtoFileCache() || ReportNewPSOs()))
	{
		FRWScopeLock Lock(FileCacheLock, SLT_ReadOnly);

		if (FileCache)
		{
			if (!RunTimeToFileHashes.Contains(RunTimeHash))
			{
				Lock.ReleaseReadOnlyLockAndAcquireWriteLock_USE_WITH_CAUTION();
				if (!RunTimeToFileHashes.Contains(RunTimeHash))
				{
					FPipelineCacheFileFormatPSO NewEntry;
					bool bOK = FPipelineCacheFileFormatPSO::Init(NewEntry, Initializer);
					check(bOK);

					uint32 PSOHash = GetTypeHash(NewEntry);
					RunTimeToFileHashes.Add(RunTimeHash, PSOHash);
					if (!FileCache->IsPSOEntryCached(NewEntry))
					{
						UE_LOG(LogRHI, Warning, TEXT("Encountered a new graphics PSO: %u"), PSOHash);
						if (GPSOFileCachePrintNewPSODescriptors > 0)
						{
							UE_LOG(LogRHI, Warning, TEXT("New Graphics PSO (%u) Description: %s"), PSOHash, *NewEntry.GraphicsDesc.ToString());
						}
						if (LogPSOtoFileCache())
						{
							NewPSOs.Add(NewEntry);
							INC_MEMORY_STAT_BY(STAT_NewCachedPSOMemory, sizeof(FPipelineCacheFileFormatPSO) + sizeof(uint32) + sizeof(uint32));
						}

						NumNewPSOs++;
						INC_DWORD_STAT(STAT_NewGraphicsPipelineStateCount);
						INC_DWORD_STAT(STAT_TotalGraphicsPipelineStateCount);

						if (ReportNewPSOs() && PSOLoggedEvent.IsBound())
						{
							PSOLoggedEvent.Broadcast(NewEntry);
						}
					}
				}
			}
		}
	}
#endif
}

FPipelineStateStats* FPipelineFileCache::RegisterPSOStats(uint32 RunTimeHash)
{
#if 0
	FPipelineStateStats* Stat = nullptr;
	if (IsPipelineFileCacheEnabled() && LogPSOtoFileCache())
	{
		FRWScopeLock Lock(FileCacheLock, SLT_ReadOnly);

		if (FileCache)
		{
			uint32 PSOHash = RunTimeToFileHashes.FindChecked(RunTimeHash);
			Stat = Stats.FindRef(PSOHash);
			if (!Stat)
			{
				Lock.ReleaseReadOnlyLockAndAcquireWriteLock_USE_WITH_CAUTION();
				Stat = Stats.FindRef(PSOHash);
				if (!Stat)
				{
					Stat = new FPipelineStateStats;
					Stat->PSOHash = PSOHash;
					Stats.Add(PSOHash, Stat);

					INC_MEMORY_STAT_BY(STAT_PSOStatMemory, sizeof(FPipelineStateStats) + sizeof(uint32));
				}
			}
			Stat->CreateCount++;
		}
	}
	return Stat;
#endif
	return nullptr;
}

FPipelineCacheFileFormatPSO::FPipelineCacheFileFormatPSO()
	: Hash(0)
{
}

/*static*/ bool FPipelineCacheFileFormatPSO::Init(FPipelineCacheFileFormatPSO& PSO, FRHIComputeShader const* Init)
{
	check(Init);

	PSO.Hash = 0;
	PSO.Type = DescriptorType::Compute;

	// Because of the cheat in the copy constructor - lets play this safe
	FMemory::Memset(&PSO.ComputeDesc, 0, sizeof(ComputeDescriptor));

	PSO.ComputeDesc.ComputeShader = Init->GetHash();

	return true;
}

/*static*/ bool FPipelineCacheFileFormatPSO::Init(FPipelineCacheFileFormatPSO& PSO, FGraphicsPipelineStateInitializer const& Init)
{
	bool bOK = true;

	PSO.Hash = 0;
	PSO.Type = DescriptorType::Graphics;

	// Because of the cheat in the copy constructor - lets play this safe
	FMemory::Memset(&PSO.GraphicsDesc, 0, sizeof(GraphicsDescriptor));

	check(Init.BoundShaderState.VertexDeclarationRHI);
	{
		bOK &= Init.BoundShaderState.VertexDeclarationRHI->GetInitializer(PSO.GraphicsDesc.VertexDescriptor);
		check(bOK);

		PSO.GraphicsDesc.VertexDescriptor.Sort([](FVertexElement const& A, FVertexElement const& B)
			{
				if (A.StreamIndex < B.StreamIndex)
				{
					return true;
				}
				if (A.StreamIndex > B.StreamIndex)
				{
					return false;
				}
				if (A.Offset < B.Offset)
				{
					return true;
				}
				if (A.Offset > B.Offset)
				{
					return false;
				}
				if (A.AttributeIndex < B.AttributeIndex)
				{
					return true;
				}
				if (A.AttributeIndex > B.AttributeIndex)
				{
					return false;
				}
				return false;
			});
	}

	if (Init.BoundShaderState.VertexShaderRHI)
	{
		PSO.GraphicsDesc.VertexShader = Init.BoundShaderState.VertexShaderRHI->GetHash();
	}

	if (Init.BoundShaderState.HullShaderRHI)
	{
		PSO.GraphicsDesc.HullShader = Init.BoundShaderState.HullShaderRHI->GetHash();
	}

	if (Init.BoundShaderState.DomainShaderRHI)
	{
		PSO.GraphicsDesc.DomainShader = Init.BoundShaderState.DomainShaderRHI->GetHash();
	}

	if (Init.BoundShaderState.PixelShaderRHI)
	{
		PSO.GraphicsDesc.FragmentShader = Init.BoundShaderState.PixelShaderRHI->GetHash();
	}

	if (Init.BoundShaderState.GeometryShaderRHI)
	{
		PSO.GraphicsDesc.GeometryShader = Init.BoundShaderState.GeometryShaderRHI->GetHash();
	}

	check(Init.BlendState);
	{
		bOK &= Init.BlendState->GetInitializer(PSO.GraphicsDesc.BlendState);
		check(bOK);
	}

	check(Init.RasterizerState);
	{
		FRasterizerStateInitializerRHI Temp;
		bOK &= Init.RasterizerState->GetInitializer(Temp);
		check(bOK);

		PSO.GraphicsDesc.RasterizerState = Temp;
	}

	check(Init.DepthStencilState);
	{
		bOK &= Init.DepthStencilState->GetInitializer(PSO.GraphicsDesc.DepthStencilState);
		check(bOK);
	}

	for (uint32 i = 0; i < MaxSimultaneousRenderTargets; i++)
	{
		PSO.GraphicsDesc.RenderTargetFormats[i] = Init.RenderTargetFormats[i];
		PSO.GraphicsDesc.RenderTargetFlags[i] = Init.RenderTargetFlags[i];
		PSO.GraphicsDesc.RenderTargetsLoad[i] = Init.RenderTargetLoadActions[i];
		PSO.GraphicsDesc.RenderTargetsStore[i] = Init.RenderTargetStoreActions[i];
	}

	PSO.GraphicsDesc.RenderTargetsActive = Init.RenderTargetsEnabled;
	PSO.GraphicsDesc.MSAASamples = Init.NumSamples;

	PSO.GraphicsDesc.DepthStencilFormat = Init.DepthStencilTargetFormat;
	PSO.GraphicsDesc.DepthStencilFlags = Init.DepthStencilTargetFlag;
	PSO.GraphicsDesc.DepthLoad = Init.DepthTargetLoadAction;
	PSO.GraphicsDesc.StencilLoad = Init.StencilTargetLoadAction;
	PSO.GraphicsDesc.DepthStore = Init.DepthTargetStoreAction;
	PSO.GraphicsDesc.StencilStore = Init.StencilTargetStoreAction;

	PSO.GraphicsDesc.PrimitiveType = Init.PrimitiveType;

	return bOK;
}

FPipelineCacheFileFormatPSO::~FPipelineCacheFileFormatPSO()
{

}

bool FPipelineCacheFileFormatPSO::operator==(const FPipelineCacheFileFormatPSO& Other) const
{
	bool bSame = true;
	if (this != &Other)
	{
		bSame = false;
		//if (Type == Other.Type && GetTypeHash(*this) == GetTypeHash(Other))
		{
			switch (Type)
			{
			case FPipelineCacheFileFormatPSO::DescriptorType::Compute:
			{
				// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above
				bSame = (FMemory::Memcmp(&ComputeDesc, &Other.ComputeDesc, sizeof(ComputeDescriptor)) == 0);
				break;
			}
			case FPipelineCacheFileFormatPSO::DescriptorType::Graphics:
			{
				// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above

				bSame = GraphicsDesc.VertexDescriptor.Num() == Other.GraphicsDesc.VertexDescriptor.Num();
				for (uint32 i = 0; i < (uint32)FMath::Min(GraphicsDesc.VertexDescriptor.Num(), Other.GraphicsDesc.VertexDescriptor.Num()); i++)
				{
					bSame &= (FMemory::Memcmp(&GraphicsDesc.VertexDescriptor[i], &Other.GraphicsDesc.VertexDescriptor[i], sizeof(FVertexElement)) == 0);
				}
				bSame &= GraphicsDesc.PrimitiveType == Other.GraphicsDesc.PrimitiveType && GraphicsDesc.VertexShader == Other.GraphicsDesc.VertexShader && GraphicsDesc.FragmentShader == Other.GraphicsDesc.FragmentShader && GraphicsDesc.GeometryShader == Other.GraphicsDesc.GeometryShader && GraphicsDesc.HullShader == Other.GraphicsDesc.HullShader && GraphicsDesc.DomainShader == Other.GraphicsDesc.DomainShader && GraphicsDesc.RenderTargetsActive == Other.GraphicsDesc.RenderTargetsActive &&
					GraphicsDesc.MSAASamples == Other.GraphicsDesc.MSAASamples && GraphicsDesc.DepthStencilFormat == Other.GraphicsDesc.DepthStencilFormat &&
					GraphicsDesc.DepthStencilFlags == Other.GraphicsDesc.DepthStencilFlags && GraphicsDesc.DepthLoad == Other.GraphicsDesc.DepthLoad &&
					GraphicsDesc.DepthStore == Other.GraphicsDesc.DepthStore && GraphicsDesc.StencilLoad == Other.GraphicsDesc.StencilLoad && GraphicsDesc.StencilStore == Other.GraphicsDesc.StencilStore &&
					FMemory::Memcmp(&GraphicsDesc.BlendState, &Other.GraphicsDesc.BlendState, sizeof(FBlendStateInitializerRHI)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.RasterizerState, &Other.GraphicsDesc.RasterizerState, sizeof(FPipelineFileCacheRasterizerState)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.DepthStencilState, &Other.GraphicsDesc.DepthStencilState, sizeof(FDepthStencilStateInitializerRHI)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.RenderTargetFormats, &Other.GraphicsDesc.RenderTargetFormats, sizeof(GraphicsDesc.RenderTargetFormats)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.RenderTargetFlags, &Other.GraphicsDesc.RenderTargetFlags, sizeof(GraphicsDesc.RenderTargetFlags)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.RenderTargetsLoad, &Other.GraphicsDesc.RenderTargetsLoad, sizeof(GraphicsDesc.RenderTargetsLoad)) == 0 &&
					FMemory::Memcmp(&GraphicsDesc.RenderTargetsStore, &Other.GraphicsDesc.RenderTargetsStore, sizeof(GraphicsDesc.RenderTargetsStore)) == 0;
				break;
			}
			default:
			{
				check(false);
				break;
			}
			}
		}
	}
	return bSame;
}

FPipelineCacheFileFormatPSO::FPipelineCacheFileFormatPSO(const FPipelineCacheFileFormatPSO& Other)
	: Type(Other.Type)
	, Hash(Other.Hash)
{
	switch (Type)
	{
	case FPipelineCacheFileFormatPSO::DescriptorType::Compute:
	{
		// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above
		FMemory::Memcpy(&ComputeDesc, &Other.ComputeDesc, sizeof(ComputeDescriptor));
		break;
	}
	case FPipelineCacheFileFormatPSO::DescriptorType::Graphics:
	{
		// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above
		FMemory::Memcpy(&GraphicsDesc, &Other.GraphicsDesc, sizeof(GraphicsDescriptor));
		break;
	}
	default:
	{
		check(false);
		break;
	}
	}
}

FPipelineCacheFileFormatPSO& FPipelineCacheFileFormatPSO::operator=(const FPipelineCacheFileFormatPSO& Other)
{
	if (this != &Other)
	{
		Type = Other.Type;
		Hash = Other.Hash;
		switch (Type)
		{
		case FPipelineCacheFileFormatPSO::DescriptorType::Compute:
		{
			// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above
			FMemory::Memcpy(&ComputeDesc, &Other.ComputeDesc, sizeof(ComputeDescriptor));
			break;
		}
		case FPipelineCacheFileFormatPSO::DescriptorType::Graphics:
		{
			// If we implement a classic copy constructor without memcpy - remove memset in ::Init() function above
			FMemory::Memcpy(&GraphicsDesc, &Other.GraphicsDesc, sizeof(GraphicsDescriptor));
			break;
		}
		default:
		{
			check(false);
			break;
		}
		}
	}
	return *this;
}


FString FPipelineFileCacheRasterizerState::ToString() const
{
	return FString::Printf(TEXT("<%f %f %u %u %u %u>")
		, DepthBias
		, SlopeScaleDepthBias
		, uint32(FillMode)
		, uint32(CullMode)
		, uint32(!!bAllowMSAA)
		, uint32(!!bEnableLineAA)
	);
}

void FPipelineFileCacheRasterizerState::FromString(const FString& InSrc)
{
	FString Src = InSrc;
	Src.ReplaceInline(TEXT("\r"), TEXT(" "));
	Src.ReplaceInline(TEXT("\n"), TEXT(" "));
	Src.ReplaceInline(TEXT("\t"), TEXT(" "));
	Src.ReplaceInline(TEXT("<"), TEXT(" "));
	Src.ReplaceInline(TEXT(">"), TEXT(" "));
	TArray<FString> Parts;
	Src.TrimStartAndEnd().ParseIntoArray(Parts, TEXT(" "));

	check(Parts.Num() == 6 && sizeof(FillMode) == 1 && sizeof(CullMode) == 1 && sizeof(bAllowMSAA) == 1 && sizeof(bEnableLineAA) == 1); //not a very robust parser
	LexFromString(DepthBias, *Parts[0]);
	LexFromString(SlopeScaleDepthBias, *Parts[1]);
	LexFromString((uint8&)FillMode, *Parts[2]);
	LexFromString((uint8&)CullMode, *Parts[3]);
	LexFromString((uint8&)bAllowMSAA, *Parts[4]);
	LexFromString((uint8&)bEnableLineAA, *Parts[5]);
}

bool FPipelineFileCache::IsPipelineFileCacheEnabled()
{
	static bool bOnce = false;
	static bool bCmdLineForce = false;
	if (!bOnce)
	{
		bOnce = true;
		//bCmdLineForce = FParse::Param(FCommandLine::Get(), TEXT("psocache"));
		//UE_CLOG(bCmdLineForce, LogRHI, Warning, TEXT("****************************** Forcing PSO cache from command line"));
	}
	return FileCacheEnabled && (bCmdLineForce || /*CVarPSOFileCacheEnabled.GetValueOnAnyThread()*/ 1 == 1);
}


void FPipelineFileCache::CacheComputePSO(uint32 RunTimeHash, FRHIComputeShader const* Initializer)
{
#if 0
	if (IsPipelineFileCacheEnabled() && (LogPSOtoFileCache() || ReportNewPSOs()))
	{
		FRWScopeLock Lock(FileCacheLock, SLT_ReadOnly);

		if (FileCache)
		{
			if (!RunTimeToFileHashes.Contains(RunTimeHash))
			{
				Lock.ReleaseReadOnlyLockAndAcquireWriteLock_USE_WITH_CAUTION();
				if (!RunTimeToFileHashes.Contains(RunTimeHash))
				{
					FPipelineCacheFileFormatPSO NewEntry;
					bool bOK = FPipelineCacheFileFormatPSO::Init(NewEntry, Initializer);
					check(bOK);

					uint32 PSOHash = GetTypeHash(NewEntry);
					RunTimeToFileHashes.Add(RunTimeHash, PSOHash);
					if (!FileCache->IsPSOEntryCached(NewEntry))
					{
						UE_LOG(LogRHI, Warning, TEXT("Encountered a new compute PSO: %u"), PSOHash);
						if (GPSOFileCachePrintNewPSODescriptors > 0)
						{
							UE_LOG(LogRHI, Warning, TEXT("New compute PSO (%u) Description: %s"), PSOHash, *NewEntry.ComputeDesc.ComputeShader.ToString());
						}

						if (LogPSOtoFileCache())
						{
							NewPSOs.Add(NewEntry);
							INC_MEMORY_STAT_BY(STAT_NewCachedPSOMemory, sizeof(FPipelineCacheFileFormatPSO) + sizeof(uint32) + sizeof(uint32));
						}

						NumNewPSOs++;
						INC_DWORD_STAT(STAT_NewComputePipelineStateCount);
						INC_DWORD_STAT(STAT_TotalComputePipelineStateCount);

						if (ReportNewPSOs() && PSOLoggedEvent.IsBound())
						{
							PSOLoggedEvent.Broadcast(NewEntry);
						}
					}
				}
			}
		}
	}
#endif
}
