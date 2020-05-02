#include "ShaderCore/GlobalShader.h"
//
//#include "Misc/MessageDialog.h"
//#include "HAL/FileManager.h"
//#include "Serialization/NameAsStringProxyArchive.h"
//#include "Misc/Paths.h"
//#include "Serialization/MemoryWriter.h"
//#include "Serialization/MemoryReader.h"
//#include "Misc/ScopedSlowTask.h"
#include "Misc/App.h"
#include "ShaderCore/StaticBoundShaderState.h"

/** The global shader map. */
TShaderMap<FGlobalShaderType>* GGlobalShaderMap[SP_NumPlatforms] = {};

IMPLEMENT_SHADER_TYPE(, FNULLPS, TEXT("/Engine/Private/NullPixelShader.usf"), TEXT("Main"), SF_Pixel);

/** Used to identify the global shader map in compile queues. */
const int32 GlobalShaderMapId = 0;

FGlobalShaderMapId::FGlobalShaderMapId(EShaderPlatform Platform)
{
	TArray<FShaderType*> ShaderTypes;
	TArray<const FShaderPipelineType*> ShaderPipelineTypes;

	for (TLinkedList<FShaderType*>::TIterator ShaderTypeIt(FShaderType::GetTypeList()); ShaderTypeIt; ShaderTypeIt.Next())
	{
		FGlobalShaderType* GlobalShaderType = ShaderTypeIt->GetGlobalShaderType();
		if (!GlobalShaderType)
		{
			continue;
		}

		bool bList = false;
		for (int32 PermutationId = 0; PermutationId < GlobalShaderType->GetPermutationCount(); PermutationId++)
		{
			if (GlobalShaderType->ShouldCompilePermutation(Platform, PermutationId))
			{
				bList = true;
				break;
			}
		}

		if (bList)
		{
			ShaderTypes.Add(GlobalShaderType);
		}
	}

	for (TLinkedList<FShaderPipelineType*>::TIterator ShaderPipelineIt(FShaderPipelineType::GetTypeList()); ShaderPipelineIt; ShaderPipelineIt.Next())
	{
		const FShaderPipelineType* Pipeline = *ShaderPipelineIt;
		if (Pipeline->IsGlobalTypePipeline())
		{
			int32 NumStagesNeeded = 0;
			auto& StageTypes = Pipeline->GetStages();
			for (const FShaderType* Shader : StageTypes)
			{
				const FGlobalShaderType* GlobalShaderType = Shader->GetGlobalShaderType();
				if (GlobalShaderType->ShouldCompilePermutation(Platform, /* PermutationId = */ 0))
				{
					++NumStagesNeeded;
				}
				else
				{
					break;
				}
			}

			if (NumStagesNeeded == StageTypes.Num())
			{
				ShaderPipelineTypes.Add(Pipeline);
			}
		}
	}

	// Individual shader dependencies
	ShaderTypes.Sort(FCompareShaderTypes());
	for (int32 TypeIndex = 0; TypeIndex < ShaderTypes.Num(); TypeIndex++)
	{
		FShaderTypeDependency Dependency;
		Dependency.ShaderType = ShaderTypes[TypeIndex];
		Dependency.SourceHash = ShaderTypes[TypeIndex]->GetSourceHash();
		ShaderTypeDependencies.Add(Dependency);
	}

	// Shader pipeline dependencies
	ShaderPipelineTypes.Sort(FCompareShaderPipelineNameTypes());
	for (int32 TypeIndex = 0; TypeIndex < ShaderPipelineTypes.Num(); TypeIndex++)
	{
		const FShaderPipelineType* Pipeline = ShaderPipelineTypes[TypeIndex];
		FShaderPipelineTypeDependency Dependency;
		Dependency.ShaderPipelineType = Pipeline;
		Dependency.StagesSourceHash = Pipeline->GetSourceHash();
		ShaderPipelineTypeDependencies.Add(Dependency);
	}
}

FGlobalShader::FGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FShader(Initializer)
{
}


TShaderMap<FGlobalShaderType>* GetGlobalShaderMap(EShaderPlatform Platform)
{
	// If the global shader map hasn't been created yet
	check(GGlobalShaderMap[Platform]);

	return GGlobalShaderMap[Platform];
}
