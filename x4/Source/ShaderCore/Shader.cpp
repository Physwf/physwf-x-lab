#include "ShaderCore/Shader.h"
#include "Misc/CoreMisc.h"
//#include "Stats/StatsMisc.h"
//#include "Serialization/MemoryWriter.h"
#include "ShaderCore/VertexFactory.h"
//#include "ProfilingDebugging/DiagnosticTable.h"
//#include "Interfaces/ITargetPlatform.h"
//#include "Interfaces/ITargetPlatformManagerModule.h"
#include "ShaderCore/ShaderCache.h"
#include "ShaderCore/ShaderCodeLibrary.h"
#include "ShaderCore/ShaderCore.h"
//#include "Misc/ConfigCacheIni.h"
//#include "UObject/RenderingObjectVersion.h"
//#include "UObject/FortniteMainBranchObjectVersion.h"

#include "X4.h"
#if 0
X4_API bool UsePreExposure(EShaderPlatform Platform)
{
	// Mobile platforms are excluded because they use a different pre-exposure logic in MobileBasePassPixelShader.usf
	static const auto CVarUsePreExposure = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.UsePreExposure"));
	return CVarUsePreExposure->GetValueOnAnyThread() != 0 && !IsMobilePlatform(Platform) && IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
}

static const ECompressionFlags ShaderCompressionFlag = ECompressionFlags::COMPRESS_ZLIB;

static TAutoConsoleVariable<int32> CVarUsePipelines(
	TEXT("r.ShaderPipelines"),
	1,
	TEXT("Enable using Shader pipelines."));
#endif

static TLinkedList<FShaderType*>*			GShaderTypeList = nullptr;
static TLinkedList<FShaderPipelineType*>*	GShaderPipelineList = nullptr;

static FSHAHash ShaderSourceDefaultHash; //will only be read (never written) for the cooking case

/**
* Find the shader pipeline type with the given name.
* @return NULL if no type matched.
*/
inline const FShaderPipelineType* FindShaderPipelineType(FString TypeName)
{
	for (TLinkedList<FShaderPipelineType*>::TIterator ShaderPipelineTypeIt(FShaderPipelineType::GetTypeList()); ShaderPipelineTypeIt; ShaderPipelineTypeIt.Next())
	{
		if (ShaderPipelineTypeIt->GetFName() == TypeName)
		{
			return *ShaderPipelineTypeIt;
		}
	}
	return nullptr;
}

void FShaderParameterMap::VerifyBindingsAreComplete(const TCHAR* ShaderTypeName, FShaderTarget Target, FVertexFactoryType* InVertexFactoryType) const
{
#if 0// WITH_EDITORONLY_DATA
	// Only people working on shaders (and therefore have LogShaders unsuppressed) will want to see these errors
	if (UE_LOG_ACTIVE(LogShaders, Warning))
	{
		const TCHAR* VertexFactoryName = InVertexFactoryType ? InVertexFactoryType->GetName() : TEXT("?");

		bool bBindingsComplete = true;
		FString UnBoundParameters = TEXT("");
		for (TMap<FString, FParameterAllocation>::TConstIterator ParameterIt(ParameterMap); ParameterIt; ++ParameterIt)
		{
			const FString& ParamName = ParameterIt.Key();
			const FParameterAllocation& ParamValue = ParameterIt.Value();
			if (!ParamValue.bBound)
			{
				// Only valid parameters should be in the shader map
				checkSlow(ParamValue.Size > 0);
				bBindingsComplete = bBindingsComplete && ParamValue.bBound;
				UnBoundParameters += FString(TEXT("		Parameter ")) + ParamName + TEXT(" not bound!\n");
			}
		}

		if (!bBindingsComplete)
		{
			FString ErrorMessage = FString(TEXT("Found unbound parameters being used in shadertype ")) + ShaderTypeName + TEXT(" (VertexFactory: ") + VertexFactoryName + TEXT(")\n") + UnBoundParameters;

			// There will be unbound parameters for Metal's "Hull" shader stage as it is merely a placeholder to provide binding indices to the RHI
			if (!IsMetalPlatform((EShaderPlatform)Target.Platform) || Target.Frequency != SF_Hull)
			{
				// We use a non-Slate message box to avoid problem where we haven't compiled the shaders for Slate.
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *ErrorMessage, TEXT("Error"));
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void FShaderParameterMap::UpdateHash(FSHA1& HashState) const
{
	for (TMap<FString, FParameterAllocation>::TConstIterator ParameterIt(ParameterMap); ParameterIt; ++ParameterIt)
	{
		const FString& ParamName = ParameterIt.Key();
		const FParameterAllocation& ParamValue = ParameterIt.Value();
		HashState.Update((const uint8*)*ParamName, ParamName.Len() * sizeof(TCHAR));
		HashState.Update((const uint8*)&ParamValue.BufferIndex, sizeof(ParamValue.BufferIndex));
		HashState.Update((const uint8*)&ParamValue.BaseIndex, sizeof(ParamValue.BaseIndex));
		HashState.Update((const uint8*)&ParamValue.Size, sizeof(ParamValue.Size));
	}
}

bool FShaderType::bInitializedSerializationHistory = false;

FShaderType::FShaderType(
	EShaderTypeForDynamicCast InShaderTypeForDynamicCast,
	const TCHAR* InName,
	const TCHAR* InSourceFilename,
	const TCHAR* InFunctionName,
	uint32 InFrequency,
	int32 InTotalPermutationCount,
	ConstructSerializedType InConstructSerializedRef,
	GetStreamOutElementsType InGetStreamOutElementsRef
) :
	ShaderTypeForDynamicCast(InShaderTypeForDynamicCast),
	Name(InName),
	TypeName(InName),
	SourceFilename(InSourceFilename),
	FunctionName(InFunctionName),
	Frequency(InFrequency),
	TotalPermutationCount(InTotalPermutationCount),
	ConstructSerializedRef(InConstructSerializedRef),
	GetStreamOutElementsRef(InGetStreamOutElementsRef),
	GlobalListLink(this)
{
	bCachedUniformBufferStructDeclarations = false;

	// This will trigger if an IMPLEMENT_SHADER_TYPE was in a module not loaded before InitializeShaderTypes
	// Shader types need to be implemented in modules that are loaded before that
	checkf(!bInitializedSerializationHistory, TEXT("Shader type was loaded after engine init, use ELoadingPhase::PostConfigInit on your module to cause it to load earlier."));

	//make sure the name is shorter than the maximum serializable length
	//check(FCString::Strlen(InName) < NAME_SIZE);

	// Make sure the format of the source file path is right.
	check(CheckVirtualShaderFilePath(InSourceFilename));

	// register this shader type
	GlobalListLink.LinkHead(GetTypeList());
	GetNameToTypeMap().Add(TypeName, this);

	// Assign the shader type the next unassigned hash index.
	static uint32 NextHashIndex = 0;
	HashIndex = NextHashIndex++;
}

FShaderType::~FShaderType()
{
	GlobalListLink.Unlink();
	GetNameToTypeMap().Remove(TypeName);
}

TLinkedList<FShaderType*>*& FShaderType::GetTypeList()
{
	return GShaderTypeList;
}

FShaderType* FShaderType::GetShaderTypeByName(const TCHAR* Name)
{
	for (TLinkedList<FShaderType*>::TIterator It(GetTypeList()); It; It.Next())
	{
		FShaderType* Type = *It;

		if (FPlatformString::Strcmp(Name, Type->GetName()) == 0)
		{
			return Type;
		}
	}

	return nullptr;
}

TArray<FShaderType*> FShaderType::GetShaderTypesByFilename(const TCHAR* Filename)
{
	TArray<FShaderType*> OutShaders;
	for (TLinkedList<FShaderType*>::TIterator It(GetTypeList()); It; It.Next())
	{
		FShaderType* Type = *It;

		if (FPlatformString::Strcmp(Filename, Type->GetShaderFilename()) == 0)
		{
			OutShaders.Add(Type);
		}
	}
	return OutShaders;
}

TMap<FString, FShaderType*>& FShaderType::GetNameToTypeMap()
{
	static TMap<FString, FShaderType*>* GShaderNameToTypeMap = NULL;
	if (!GShaderNameToTypeMap)
	{
		GShaderNameToTypeMap = new TMap<FString, FShaderType*>();
	}
	return *GShaderNameToTypeMap;
}

inline bool FShaderType::GetOutdatedCurrentType(TArray<FShaderType*>& OutdatedShaderTypes, TArray<const FVertexFactoryType*>& OutdatedFactoryTypes) const
{
	bool bOutdated = false;
	for (TMap<FShaderId, FShader*>::TConstIterator ShaderIt(ShaderIdMap); ShaderIt; ++ShaderIt)
	{
		FShader* Shader = ShaderIt.Value();
		const FVertexFactoryParameterRef* VFParameterRef = Shader->GetVertexFactoryParameterRef();
		const FSHAHash& SavedHash = Shader->GetHash();
		const FSHAHash& CurrentHash = GetSourceHash();
		const bool bOutdatedShader = SavedHash != CurrentHash;
		const bool bOutdatedVertexFactory =
			VFParameterRef && VFParameterRef->GetVertexFactoryType() && VFParameterRef->GetVertexFactoryType()->GetSourceHash() != VFParameterRef->GetHash();

		if (bOutdatedShader)
		{
			OutdatedShaderTypes.AddUnique(Shader->Type);
			bOutdated = true;
		}

		if (bOutdatedVertexFactory)
		{
			OutdatedFactoryTypes.AddUnique(VFParameterRef->GetVertexFactoryType());
			bOutdated = true;
		}
	}

	return bOutdated;
}

void FShaderType::GetOutdatedTypes(TArray<FShaderType*>& OutdatedShaderTypes, TArray<const FVertexFactoryType*>& OutdatedFactoryTypes)
{
	for (TLinkedList<FShaderType*>::TIterator It(GetTypeList()); It; It.Next())
	{
		FShaderType* Type = *It;
		Type->GetOutdatedCurrentType(OutdatedShaderTypes, OutdatedFactoryTypes);
	}

	for (int32 TypeIndex = 0; TypeIndex < OutdatedShaderTypes.Num(); TypeIndex++)
	{
		UE_LOG(LogShaders, Warning, TEXT("		Recompiling %s"), OutdatedShaderTypes[TypeIndex]->GetName());
	}
	for (int32 TypeIndex = 0; TypeIndex < OutdatedFactoryTypes.Num(); TypeIndex++)
	{
		UE_LOG(LogShaders, Warning, TEXT("		Recompiling %s"), OutdatedFactoryTypes[TypeIndex]->GetName());
	}
}

FShader* FShaderType::FindShaderById(const FShaderId& Id)
{
	check(IsInGameThread());
	FShader* Result = ShaderIdMap.FindRef(Id);
	check(!Result || Result->GetId() == Id);
	return Result;
}

FShader* FShaderType::ConstructForDeserialization() const
{
	return (*ConstructSerializedRef)();
}

const FSHAHash& FShaderType::GetSourceHash() const
{
	return GetShaderFileHash(GetShaderFilename());
}

void FShaderType::Initialize(const TMap<FString, TArray<const TCHAR*> >& ShaderFileToUniformBufferVariables)
{
#if 0
	//#todo-rco: Need to call this only when Initializing from a Pipeline once it's removed from the global linked list
	if (!FPlatformProperties::RequiresCookedData())
	{
#if UE_BUILD_DEBUG
		TArray<FShaderType*> UniqueShaderTypes;
#endif
		for (TLinkedList<FShaderType*>::TIterator It(FShaderType::GetTypeList()); It; It.Next())
		{
			FShaderType* Type = *It;
#if UE_BUILD_DEBUG
			UniqueShaderTypes.Add(Type);
#endif
			GenerateReferencedUniformBuffers(Type->SourceFilename, Type->Name, ShaderFileToUniformBufferVariables, Type->ReferencedUniformBufferStructsCache);

			// Cache serialization history for each shader type
			// This history is used to detect when shader serialization changes without a corresponding .usf change
			{
				// Construct a temporary shader, which is initialized to safe values for serialization
				FShader* TempShader = Type->ConstructForDeserialization();
				check(TempShader != NULL);
				TempShader->Type = Type;

				// Serialize the temp shader to memory and record the number and sizes of serializations
				TArray<uint8> TempData;
				FMemoryWriter Ar(TempData, true);
				FShaderSaveArchive SaveArchive(Ar, Type->SerializationHistory);
				TempShader->SerializeBase(SaveArchive, false);

				// Destroy the temporary shader
				delete TempShader;
			}
		}

#if UE_BUILD_DEBUG
		// Check for duplicated shader type names
		UniqueShaderTypes.Sort([](const FShaderType& A, const FShaderType& B) { return (SIZE_T)&A < (SIZE_T)&B; });
		for (int32 Index = 1; Index < UniqueShaderTypes.Num(); ++Index)
		{
			checkf(UniqueShaderTypes[Index - 1] != UniqueShaderTypes[Index], TEXT("Duplicated FShader type name %s found, please rename one of them!"), UniqueShaderTypes[Index]->GetName());
		}
#endif
	}
#endif
	bInitializedSerializationHistory = true;
}

void FShaderType::Uninitialize()
{
	for (TLinkedList<FShaderType*>::TIterator It(FShaderType::GetTypeList()); It; It.Next())
	{
		FShaderType* Type = *It;
		Type->SerializationHistory = FSerializationHistory();
	}

	bInitializedSerializationHistory = false;
}

TMap<FShaderResourceId, FShaderResource*> FShaderResource::ShaderResourceIdMap;

FShaderResource::FShaderResource()
	: SpecificType(NULL)
	, SpecificPermutationId(0)
	, NumInstructions(0)
	, NumTextureSamplers(0)
	, NumRefs(0)
	, Canary(FShader::ShaderMagic_Uninitialized)
	, bCodeInSharedLocation(false)
{
	//INC_DWORD_STAT_BY(STAT_Shaders_NumShaderResourcesLoaded, 1);
}

FShaderResource::FShaderResource(const FShaderCompilerOutput& Output, FShaderType* InSpecificType, int32 InSpecificPermutationId)
	: SpecificType(InSpecificType)
	, SpecificPermutationId(InSpecificPermutationId)
	, NumInstructions(Output.NumInstructions)
	, NumTextureSamplers(Output.NumTextureSamplers)
	, NumRefs(0)
	, Canary(FShader::ShaderMagic_Initialized)
	, bCodeInSharedLocation(false)

{
	check(!(SpecificPermutationId != 0 && SpecificType == nullptr));

	Target = Output.Target;
	CompressCode(Output.ShaderCode.GetReadAccess());

	check(Code.Num() > 0);

	OutputHash = Output.OutputHash;
	checkSlow(OutputHash != FSHAHash());

	{
		check(IsInGameThread());
		ShaderResourceIdMap.Add(GetId(), this);
	}

	//INC_DWORD_STAT_BY_FName(GetMemoryStatType((EShaderFrequency)Target.Frequency).GetName(), Code.Num());
	//INC_DWORD_STAT_BY(STAT_Shaders_ShaderResourceMemory, GetSizeBytes());
	//INC_DWORD_STAT_BY(STAT_Shaders_NumShaderResourcesLoaded, 1);
}

FShaderResource::~FShaderResource()
{
	check(Canary == FShader::ShaderMagic_Uninitialized || Canary == FShader::ShaderMagic_CleaningUp || Canary == FShader::ShaderMagic_Initialized);
	check(NumRefs == 0);
	Canary = 0;

	//DEC_DWORD_STAT_BY_FName(GetMemoryStatType((EShaderFrequency)Target.Frequency).GetName(), Code.Num());
	//DEC_DWORD_STAT_BY(STAT_Shaders_ShaderResourceMemory, GetSizeBytes());
	//DEC_DWORD_STAT_BY(STAT_Shaders_NumShaderResourcesLoaded, 1);
}

void FShaderResource::UncompressCode(TArray<uint8>& UncompressedCode) const
{
#if 0
	if (Code.Num() != UncompressedCodeSize && RHISupportsShaderCompression((EShaderPlatform)Target.Platform))
	{
		UncompressedCode.SetNum(UncompressedCodeSize);
		auto bSucceed = FCompression::UncompressMemory(ShaderCompressionFlag, UncompressedCode.GetData(), UncompressedCodeSize, Code.GetData(), Code.Num());
		check(bSucceed);
	}
	else
	{
		UncompressedCode = Code;
	}
#endif
}

void FShaderResource::CompressCode(const TArray<uint8>& UncompressedCode)
{
#if 0
	UncompressedCodeSize = UncompressedCode.Num();
	Code = UncompressedCode;
	if (RHISupportsShaderCompression((EShaderPlatform)Target.Platform))
	{
		auto CompressedSize = Code.Num();
		if (FCompression::CompressMemory(ShaderCompressionFlag, Code.GetData(), CompressedSize, UncompressedCode.GetData(), UncompressedCode.Num()))
		{
			Code.SetNum(CompressedSize);
		}
		Code.Shrink();
	}
#endif
}

void FShaderResource::Register()
{
	check(IsInGameThread());
	ShaderResourceIdMap.Add(GetId(), this);

	if (FShaderCache::GetShaderCache())
	{
		FShaderCache::LogShader((EShaderPlatform)Target.Platform, (EShaderFrequency)Target.Frequency, OutputHash, UncompressedCodeSize, Code);
	}
}

void FShaderResource::AddRef()
{
	checkSlow(IsInGameThread());
	check(Canary != FShader::ShaderMagic_CleaningUp);
	++NumRefs;
}


void FShaderResource::Release()
{
	checkSlow(IsInGameThread());
	check(NumRefs != 0);
	if (--NumRefs == 0)
	{
		ShaderResourceIdMap.Remove(GetId());

		// Send a release message to the rendering thread when the shader loses its last reference.
		BeginReleaseResource(this);

		Canary = FShader::ShaderMagic_CleaningUp;
		BeginCleanup(this);

		if (bCodeInSharedLocation)
		{
			FShaderCodeLibrary::ReleaseShaderCode(OutputHash);
		}
	}
}

FShaderResource* FShaderResource::FindShaderResourceById(const FShaderResourceId& Id)
{
	check(IsInGameThread());
	FShaderResource* Result = ShaderResourceIdMap.FindRef(Id);
	return Result;
}


FShaderResource* FShaderResource::FindOrCreateShaderResource(const FShaderCompilerOutput& Output, FShaderType* SpecificType, int32 SpecificPermutationId)
{
	const FShaderResourceId ResourceId(Output.Target, Output.OutputHash, SpecificType ? SpecificType->GetName() : nullptr, SpecificPermutationId);
	FShaderResource* Resource = FindShaderResourceById(ResourceId);
	if (!Resource)
	{
		Resource = new FShaderResource(Output, SpecificType, SpecificPermutationId);
	}
	else
	{
		check(Resource->Canary == FShader::ShaderMagic_Initialized);
	}

	return Resource;
}

void FShaderResource::GetAllShaderResourceId(TArray<FShaderResourceId>& Ids)
{
	check(IsInGameThread());
	ShaderResourceIdMap.GetKeys(Ids);
}

bool FShaderResource::ArePlatformsCompatible(EShaderPlatform CurrentPlatform, EShaderPlatform TargetPlatform)
{
	bool bFeatureLevelCompatible = CurrentPlatform == TargetPlatform;

	if (!bFeatureLevelCompatible && IsPCPlatform(CurrentPlatform) && IsPCPlatform(TargetPlatform))
	{
		bFeatureLevelCompatible = GetMaxSupportedFeatureLevel(CurrentPlatform) >= GetMaxSupportedFeatureLevel(TargetPlatform);

		bool const bIsTargetD3D = TargetPlatform == SP_PCD3D_SM5 ||
			TargetPlatform == SP_PCD3D_SM4 ||
			TargetPlatform == SP_PCD3D_ES3_1 ||
			TargetPlatform == SP_PCD3D_ES2;

		bool const bIsCurrentPlatformD3D = CurrentPlatform == SP_PCD3D_SM5 ||
			CurrentPlatform == SP_PCD3D_SM4 ||
			TargetPlatform == SP_PCD3D_ES3_1 ||
			CurrentPlatform == SP_PCD3D_ES2;

		// For Metal in Editor we can switch feature-levels, but not in cooked projects when using Metal shader librariss.
		bool const bIsCurrentMetal = IsMetalPlatform(CurrentPlatform);
		bool const bIsTargetMetal = IsMetalPlatform(TargetPlatform);
		bool const bIsMetalCompatible = (bIsCurrentMetal == bIsTargetMetal)
#if !WITH_EDITOR	// Static analysis doesn't like (|| WITH_EDITOR)
			&& (!IsMetalPlatform(CurrentPlatform) || (CurrentPlatform == TargetPlatform))
#endif
			;

		bool const bIsCurrentOpenGL = IsOpenGLPlatform(CurrentPlatform);
		bool const bIsTargetOpenGL = IsOpenGLPlatform(TargetPlatform);

		bFeatureLevelCompatible = bFeatureLevelCompatible && (bIsCurrentPlatformD3D == bIsTargetD3D && bIsMetalCompatible && bIsCurrentOpenGL == bIsTargetOpenGL);
	}

	return bFeatureLevelCompatible;
}

static void SafeAssignHash(FRHIShader* InShader, const FSHAHash& Hash)
{
	if (InShader)
	{
		InShader->SetHash(Hash);
	}
}

void FShaderResource::InitRHI()
{
	checkf(bCodeInSharedLocation || Code.Num() > 0, TEXT("FShaderResource::InitRHI was called with empty bytecode, which can happen if the resource is initialized multiple times on platforms with no editor data."));

	// we can't have this called on the wrong platform's shaders
// 	if (!ArePlatformsCompatible(GMaxRHIShaderPlatform, (EShaderPlatform)Target.Platform))
// 	{
// 		if (FPlatformProperties::RequiresCookedData())
// 		{
// 			UE_LOG(LogShaders, Fatal, TEXT("FShaderResource::InitRHI got platform %s but it is not compatible with %s"),
// 				*LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString(), *LegacyShaderPlatformToShaderFormat(GMaxRHIShaderPlatform).ToString());
// 		}
// 		return;
// 	}

	TArray<uint8> UncompressedCode;
	if (!bCodeInSharedLocation)
	{
		UncompressCode(UncompressedCode);
	}

	//INC_DWORD_STAT_BY(STAT_Shaders_NumShadersUsedForRendering, 1);
	//SCOPE_CYCLE_COUNTER(STAT_Shaders_RTShaderLoadTime);

	if (Target.Frequency == SF_Vertex)
	{
		VertexShader = FShaderCache::CreateVertexShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(VertexShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}
	else if (Target.Frequency == SF_Pixel)
	{
		PixelShader = FShaderCache::CreatePixelShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(PixelShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}
	else if (Target.Frequency == SF_Hull)
	{
		HullShader = FShaderCache::CreateHullShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(HullShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}
	else if (Target.Frequency == SF_Domain)
	{
		DomainShader = FShaderCache::CreateDomainShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(DomainShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}
	else if (Target.Frequency == SF_Geometry)
	{
		if (SpecificType)
		{
			FStreamOutElementList ElementList;
			TArray<uint32> StreamStrides;
			int32 RasterizedStream = -1;
			SpecificType->GetStreamOutElements(ElementList, StreamStrides, RasterizedStream);
			checkf(ElementList.Num(), TEXT("Shader type %s was given GetStreamOutElements implementation that had no elements!"), SpecificType->GetName());

			//@todo - not using the cache
			GeometryShader = FShaderCodeLibrary::CreateGeometryShaderWithStreamOutput((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode, ElementList, StreamStrides.Num(), StreamStrides.GetData(), RasterizedStream);
		}
		else
		{
			GeometryShader = FShaderCache::CreateGeometryShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		}
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(GeometryShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}
	else if (Target.Frequency == SF_Compute)
	{
		ComputeShader = FShaderCache::CreateComputeShader((EShaderPlatform)Target.Platform, OutputHash, UncompressedCode);
		//UE_CLOG((bCodeInSharedLocation && !IsValidRef(ComputeShader)), LogShaders, Fatal, TEXT("FShaderResource::SerializeShaderCode can't find shader code for: [%s]"), *LegacyShaderPlatformToShaderFormat((EShaderPlatform)Target.Platform).ToString());
	}

	if (Target.Frequency != SF_Geometry)
	{
		checkf(!SpecificType, TEXT("Only geometry shaders can use GetStreamOutElements, shader type %s"), SpecificType->GetName());
	}
// 
// 	if (!FPlatformProperties::HasEditorOnlyData())
// 	{
// 		//DEC_DWORD_STAT_BY_FName(GetMemoryStatType((EShaderFrequency)Target.Frequency).GetName(), Code.Num());
// 		//DEC_DWORD_STAT_BY(STAT_Shaders_ShaderResourceMemory, Code.GetAllocatedSize());
// 		Code.Empty();
// 
// 		if (bCodeInSharedLocation)
// 		{
// 			FShaderCodeLibrary::ReleaseShaderCode(OutputHash);
// 			bCodeInSharedLocation = false;
// 		}
// 	}
}

void FShaderResource::ReleaseRHI()
{
	//DEC_DWORD_STAT_BY(STAT_Shaders_NumShadersUsedForRendering, 1);

	VertexShader.SafeRelease();
	PixelShader.SafeRelease();
	HullShader.SafeRelease();
	DomainShader.SafeRelease();
	GeometryShader.SafeRelease();
	ComputeShader.SafeRelease();
}

void FShaderResource::InitializeShaderRHI()
{
	if (!IsInitialized())
	{
		//STAT(double ShaderInitializationTime = 0);
		{
			//SCOPE_CYCLE_COUNTER(STAT_Shaders_FrameRTShaderInitForRenderingTime);
			//SCOPE_SECONDS_COUNTER(ShaderInitializationTime);

			InitResourceFromPossiblyParallelRendering();
		}

		//INC_FLOAT_STAT_BY(STAT_Shaders_TotalRTShaderInitForRenderingTime, (float)ShaderInitializationTime);
	}

	checkSlow(IsInitialized());
}

FShaderResourceId FShaderResource::GetId() const
{
	return FShaderResourceId(Target, OutputHash, SpecificType ? SpecificType->GetName() : nullptr, SpecificPermutationId);
}

FShaderId::FShaderId(const FSHAHash& InMaterialShaderMapHash, const FShaderPipelineType* InShaderPipeline, FVertexFactoryType* InVertexFactoryType, FShaderType* InShaderType, int32 InPermutationId, FShaderTarget InTarget)
	: MaterialShaderMapHash(InMaterialShaderMapHash)
	, ShaderPipeline(InShaderPipeline)
	, ShaderType(InShaderType)
	, PermutationId(InPermutationId)
	, SourceHash(InShaderType->GetSourceHash())
	, SerializationHistory(InShaderType->GetSerializationHistory())
	, Target(InTarget)
{
	if (InVertexFactoryType)
	{
		VFSerializationHistory = InVertexFactoryType->GetSerializationHistory((EShaderFrequency)InTarget.Frequency);
		VertexFactoryType = InVertexFactoryType;
		VFSourceHash = InVertexFactoryType->GetSourceHash();
	}
	else
	{
		VFSerializationHistory = nullptr;
		VertexFactoryType = nullptr;
	}
}

FSelfContainedShaderId::FSelfContainedShaderId() :
	Target(FShaderTarget(SF_NumFrequencies, SP_NumPlatforms))
{}

FSelfContainedShaderId::FSelfContainedShaderId(const FShaderId& InShaderId)
{
	MaterialShaderMapHash = InShaderId.MaterialShaderMapHash;
	VertexFactoryTypeName = InShaderId.VertexFactoryType ? InShaderId.VertexFactoryType->GetName() : TEXT("");
	ShaderPipelineName = InShaderId.ShaderPipeline ? InShaderId.ShaderPipeline->GetName() : TEXT("");
	VFSourceHash = InShaderId.VFSourceHash;
	VFSerializationHistory = InShaderId.VFSerializationHistory ? *InShaderId.VFSerializationHistory : FSerializationHistory();
	ShaderTypeName = InShaderId.ShaderType->GetName();
	PermutationId = InShaderId.PermutationId;
	SourceHash = InShaderId.SourceHash;
	SerializationHistory = InShaderId.SerializationHistory;
	Target = InShaderId.Target;
}

bool FSelfContainedShaderId::IsValid()
{
	FShaderType** TypePtr = FShaderType::GetNameToTypeMap().Find(FString(*ShaderTypeName));
	if (TypePtr && SourceHash == (*TypePtr)->GetSourceHash() && SerializationHistory == (*TypePtr)->GetSerializationHistory())
	{
		FVertexFactoryType* VFTypePtr = FVertexFactoryType::GetVFByName(VertexFactoryTypeName);

		if (VertexFactoryTypeName == TEXT("")
			|| (VFTypePtr && VFSourceHash == VFTypePtr->GetSourceHash() && VFSerializationHistory == *VFTypePtr->GetSerializationHistory((EShaderFrequency)Target.Frequency)))
		{
			return true;
		}
	}

	return false;
}

/**
* Used to construct a shader for deserialization.
* This still needs to initialize members to safe values since FShaderType::GenerateSerializationHistory uses this constructor.
*/
FShader::FShader() :
	SerializedResource(nullptr),
	ShaderPipeline(nullptr),
	VFType(nullptr),
	Type(nullptr),
	PermutationId(0),
	NumRefs(0),
	SetParametersId(0),
	Canary(ShaderMagic_Uninitialized)
{
	// set to undefined (currently shared with SF_Vertex)
	Target.Frequency = 0;
	Target.Platform = GShaderPlatformForFeatureLevel[GMaxRHIFeatureLevel];
}

/**
* Construct a shader from shader compiler output.
*/
FShader::FShader(const CompiledShaderInitializerType& Initializer) :
	SerializedResource(nullptr),
	MaterialShaderMapHash(Initializer.MaterialShaderMapHash),
	ShaderPipeline(Initializer.ShaderPipeline),
	VFType(Initializer.VertexFactoryType),
	Type(Initializer.Type),
	PermutationId(Initializer.PermutationId),
	Target(Initializer.Target),
	NumRefs(0),
	SetParametersId(0),
	Canary(ShaderMagic_Initialized)
{
	OutputHash = Initializer.OutputHash;
	checkSlow(OutputHash != FSHAHash());

	check(Type);

	// Store off the source hash that this shader was compiled with
	// This will be used as part of the shader key in order to identify when shader files have been changed and a recompile is needed
	SourceHash = Type->GetSourceHash();

	if (VFType)
	{
		// Store off the VF source hash that this shader was compiled with
		VFSourceHash = VFType->GetSourceHash();
	}

	// Bind uniform buffer parameters automatically 
	for (TLinkedList<FUniformBufferStruct*>::TIterator StructIt(FUniformBufferStruct::GetStructList()); StructIt; StructIt.Next())
	{
		if (Initializer.ParameterMap.ContainsParameterAllocation(StructIt->GetShaderVariableName()))
		{
			UniformBufferParameterStructs.Add(*StructIt);
			UniformBufferParameters.Add(StructIt->ConstructTypedParameter());
			FShaderUniformBufferParameter* Parameter = UniformBufferParameters.Last();
			Parameter->Bind(Initializer.ParameterMap, StructIt->GetShaderVariableName(), SPF_Mandatory);
		}
	}

	SetResource(Initializer.Resource);

	// Register the shader now that it is valid, so that it can be reused
	Register();
}

FShader::~FShader()
{
	check(Canary == ShaderMagic_Uninitialized || Canary == ShaderMagic_CleaningUp || Canary == ShaderMagic_Initialized);
	check(NumRefs == 0);
	Canary = 0;

	for (int32 StructIndex = 0; StructIndex < UniformBufferParameters.Num(); StructIndex++)
	{
		delete UniformBufferParameters[StructIndex];
	}
}

const FSHAHash& FShader::GetHash() const
{
	return SourceHash;
}

void FShader::AddRef()
{
	check(Canary != ShaderMagic_CleaningUp);
	++NumRefs;
	if (NumRefs == 1)
	{
		//INC_DWORD_STAT_BY(STAT_Shaders_ShaderMemory, GetSizeBytes());
		//INC_DWORD_STAT_BY(STAT_Shaders_NumShadersLoaded, 1);
	}
}

void FShader::Release()
{
	if (--NumRefs == 0)
	{
		//DEC_DWORD_STAT_BY(STAT_Shaders_ShaderMemory, GetSizeBytes());
		//DEC_DWORD_STAT_BY(STAT_Shaders_NumShadersLoaded, 1);

		// Deregister the shader now to eliminate references to it by the type's ShaderIdMap
		Deregister();

		Canary = ShaderMagic_CleaningUp;
		BeginCleanup(this);
	}
}

void FShader::Register()
{
	FShaderId ShaderId = GetId();
	check(ShaderId.MaterialShaderMapHash != FSHAHash());
	//check(ShaderId.SourceHash != FSHAHash() || FPlatformProperties::RequiresCookedData());
	check(Resource);
	Type->AddToShaderIdMap(ShaderId, this);
}


void FShader::Deregister()
{
	Type->RemoveFromShaderIdMap(GetId());
}

FShaderId FShader::GetId() const
{
	FShaderId ShaderId(Type->GetSerializationHistory());
	ShaderId.MaterialShaderMapHash = MaterialShaderMapHash;
	ShaderId.ShaderPipeline = ShaderPipeline;
	ShaderId.VertexFactoryType = VFType;
	ShaderId.VFSourceHash = VFSourceHash;
	ShaderId.VFSerializationHistory = VFType ? VFType->GetSerializationHistory((EShaderFrequency)GetTarget().Frequency) : NULL;
	ShaderId.ShaderType = Type;
	ShaderId.PermutationId = PermutationId;
	ShaderId.SourceHash = SourceHash;
	ShaderId.Target = Target;
	return ShaderId;
}

void FShader::RegisterSerializedResource()
{
	if (SerializedResource)
	{
		FShaderResource* ExistingResource = FShaderResource::FindShaderResourceById(SerializedResource->GetId());

		// Reuse an existing shader resource if a matching one already exists in memory
		if (ExistingResource)
		{
			delete SerializedResource;
			SerializedResource = ExistingResource;
		}
		else
		{
			// Register the newly loaded shader resource so it can be reused by other shaders
			SerializedResource->Register();
		}

		SetResource(SerializedResource);
	}
}

void FShader::SetResource(FShaderResource* InResource)
{
	check(InResource && InResource->Target == Target);
	Resource = InResource;
}

void FShader::DumpDebugInfo()
{
	UE_LOG(LogConsoleResponse, Display, TEXT("      FShader  :MaterialShaderMapHash %s"), *MaterialShaderMapHash.ToString());
	UE_LOG(LogConsoleResponse, Display, TEXT("               :Target %s"), GetShaderFrequencyString((EShaderFrequency)Target.Frequency));
	UE_LOG(LogConsoleResponse, Display, TEXT("               :Target %s"), *LegacyShaderPlatformToShaderFormat(EShaderPlatform(Target.Platform)).ToString());
	UE_LOG(LogConsoleResponse, Display, TEXT("               :VFType %s"), VFType ? VFType->GetName() : TEXT("null"));
	UE_LOG(LogConsoleResponse, Display, TEXT("               :VFSourceHash %s"), *VFSourceHash.ToString());
	UE_LOG(LogConsoleResponse, Display, TEXT("               :Type %s"), Type->GetName());
	UE_LOG(LogConsoleResponse, Display, TEXT("               :PermutationId %d"), PermutationId);
	UE_LOG(LogConsoleResponse, Display, TEXT("               :SourceHash %s"), *SourceHash.ToString());
	UE_LOG(LogConsoleResponse, Display, TEXT("               :OutputHash %s"), *OutputHash.ToString());
}

void FShader::SaveShaderStableKeys(EShaderPlatform TargetShaderPlatform, const FStableShaderKeyAndValue& InSaveKeyVal)
{
#if WITH_EDITOR
	if (TargetShaderPlatform == EShaderPlatform::SP_NumPlatforms || EShaderPlatform(Target.Platform) == TargetShaderPlatform)
	{
		FStableShaderKeyAndValue SaveKeyVal(InSaveKeyVal);
		SaveKeyVal.TargetFrequency = FName(GetShaderFrequencyString((EShaderFrequency)Target.Frequency));
		SaveKeyVal.TargetPlatform = FName(*LegacyShaderPlatformToShaderFormat(EShaderPlatform(Target.Platform)).ToString());
		SaveKeyVal.VFType = FName(VFType ? VFType->GetName() : TEXT("null"));
		SaveKeyVal.PermutationId = FName(*FString::Printf(TEXT("Perm_%d"), PermutationId));
		SaveKeyVal.OutputHash = OutputHash;
		if (Type)
		{
			Type->GetShaderStableKeyParts(SaveKeyVal);
		}
		FShaderCodeLibrary::AddShaderStableKeyValue(EShaderPlatform(Target.Platform), SaveKeyVal);
	}
#endif
}

bool FShaderPipelineType::bInitialized = false;

FShaderPipelineType::FShaderPipelineType(
	const TCHAR* InName,
	const FShaderType* InVertexShader,
	const FShaderType* InHullShader,
	const FShaderType* InDomainShader,
	const FShaderType* InGeometryShader,
	const FShaderType* InPixelShader,
	bool bInShouldOptimizeUnusedOutputs) :
	Name(InName),
	TypeName(Name),
	GlobalListLink(this),
	bShouldOptimizeUnusedOutputs(bInShouldOptimizeUnusedOutputs)
{
	checkf(Name && *Name, TEXT("Shader Pipeline Type requires a valid Name!"));

	checkf(InVertexShader, TEXT("A Shader Pipeline always requires a Vertex Shader"));

	checkf((InHullShader == nullptr && InDomainShader == nullptr) || (InHullShader != nullptr && InDomainShader != nullptr), TEXT("Both Hull & Domain shaders are needed for tessellation on Pipeline %s"), Name);

	//make sure the name is shorter than the maximum serializable length
	//check(FCString::Strlen(InName) < NAME_SIZE);

	FMemory::Memzero(AllStages);

	if (InPixelShader)
	{
		Stages.Add(InPixelShader);
		AllStages[SF_Pixel] = InPixelShader;
	}
	if (InGeometryShader)
	{
		Stages.Add(InGeometryShader);
		AllStages[SF_Geometry] = InGeometryShader;
	}
	if (InDomainShader)
	{
		Stages.Add(InDomainShader);
		AllStages[SF_Domain] = InDomainShader;

		Stages.Add(InHullShader);
		AllStages[SF_Hull] = InHullShader;
	}
	Stages.Add(InVertexShader);
	AllStages[SF_Vertex] = InVertexShader;

	static uint32 TypeHashCounter = 0;
	++TypeHashCounter;
	HashIndex = TypeHashCounter;

	GlobalListLink.LinkHead(GetTypeList());
	GetNameToTypeMap().Add(FString(InName), this);

	// This will trigger if an IMPLEMENT_SHADER_TYPE was in a module not loaded before InitializeShaderTypes
	// Shader types need to be implemented in modules that are loaded before that
	checkf(!bInitialized, TEXT("Shader Pipeline was loaded after Engine init, use ELoadingPhase::PostConfigInit on your module to cause it to load earlier."));
}

FShaderPipelineType::~FShaderPipelineType()
{
	GetNameToTypeMap().Remove(FString(Name));
	GlobalListLink.Unlink();
}

TMap<FString, FShaderPipelineType*>& FShaderPipelineType::GetNameToTypeMap()
{
	static TMap<FString, FShaderPipelineType*>* GShaderPipelineNameToTypeMap = NULL;
	if (!GShaderPipelineNameToTypeMap)
	{
		GShaderPipelineNameToTypeMap = new TMap<FString, FShaderPipelineType*>();
	}
	return *GShaderPipelineNameToTypeMap;
}

TLinkedList<FShaderPipelineType*>*& FShaderPipelineType::GetTypeList()
{
	return GShaderPipelineList;
}

TArray<const FShaderPipelineType*> FShaderPipelineType::GetShaderPipelineTypesByFilename(const TCHAR* Filename)
{
	TArray<const FShaderPipelineType*> PipelineTypes;
	for (TLinkedList<FShaderPipelineType*>::TIterator It(FShaderPipelineType::GetTypeList()); It; It.Next())
	{
		auto* PipelineType = *It;
		for (auto* ShaderType : PipelineType->Stages)
		{
			if (FPlatformString::Strcmp(Filename, ShaderType->GetShaderFilename()) == 0)
			{
				PipelineTypes.AddUnique(PipelineType);
				break;
			}
		}
	}
	return PipelineTypes;
}

void FShaderPipelineType::Initialize()
{
	check(!bInitialized);

	TSet<FString> UsedNames;

#if UE_BUILD_DEBUG
	TArray<const FShaderPipelineType*> UniqueShaderPipelineTypes;
#endif
	for (TLinkedList<FShaderPipelineType*>::TIterator It(FShaderPipelineType::GetTypeList()); It; It.Next())
	{
		const auto* PipelineType = *It;

#if UE_BUILD_DEBUG
		UniqueShaderPipelineTypes.Add(PipelineType);
#endif

		// Validate stages
		for (int32 Index = 0; Index < SF_NumFrequencies; ++Index)
		{
			check(!PipelineType->AllStages[Index] || PipelineType->AllStages[Index]->GetFrequency() == (EShaderFrequency)Index);
		}

		auto& Stages = PipelineType->GetStages();

		// #todo-rco: Do we allow mix/match of global/mesh/material stages?
		// Check all shaders are the same type, start from the top-most stage
		const FGlobalShaderType* GlobalType = Stages[0]->GetGlobalShaderType();
		const FMeshMaterialShaderType* MeshType = Stages[0]->GetMeshMaterialShaderType();
		const FMaterialShaderType* MateriallType = Stages[0]->GetMaterialShaderType();
		for (int32 Index = 1; Index < Stages.Num(); ++Index)
		{
			if (GlobalType)
			{
				checkf(Stages[Index]->GetGlobalShaderType(), TEXT("Invalid combination of Shader types on Pipeline %s"), PipelineType->Name);
			}
			else if (MeshType)
			{
				checkf(Stages[Index]->GetMeshMaterialShaderType(), TEXT("Invalid combination of Shader types on Pipeline %s"), PipelineType->Name);
			}
			else if (MateriallType)
			{
				checkf(Stages[Index]->GetMaterialShaderType(), TEXT("Invalid combination of Shader types on Pipeline %s"), PipelineType->Name);
			}
		}

		FString PipelineName = PipelineType->GetFName();
		checkf(!UsedNames.Contains(PipelineName), TEXT("Two Pipelines with the same name %s found!"), PipelineType->Name);
		UsedNames.Add(PipelineName);
	}

#if UE_BUILD_DEBUG
	// Check for duplicated shader pipeline type names
	UniqueShaderPipelineTypes.Sort([](const FShaderPipelineType& A, const FShaderPipelineType& B) { return (SIZE_T)&A < (SIZE_T)&B; });
	for (int32 Index = 1; Index < UniqueShaderPipelineTypes.Num(); ++Index)
	{
		checkf(UniqueShaderPipelineTypes[Index - 1] != UniqueShaderPipelineTypes[Index], TEXT("Duplicated FShaderPipeline type name %s found, please rename one of them!"), UniqueShaderPipelineTypes[Index]->GetName());
	}
#endif

	bInitialized = true;
}

void FShaderPipelineType::Uninitialize()
{
	check(bInitialized);

	bInitialized = false;
}

void FShaderPipelineType::GetOutdatedTypes(TArray<FShaderType*>& OutdatedShaderTypes, TArray<const FShaderPipelineType*>& OutdatedShaderPipelineTypes, TArray<const FVertexFactoryType*>& OutdatedFactoryTypes)
{
	for (TLinkedList<FShaderPipelineType*>::TIterator It(FShaderPipelineType::GetTypeList()); It; It.Next())
	{
		const auto* PipelineType = *It;
		auto& Stages = PipelineType->GetStages();
		bool bOutdated = false;
		for (const FShaderType* ShaderType : Stages)
		{
			bOutdated = ShaderType->GetOutdatedCurrentType(OutdatedShaderTypes, OutdatedFactoryTypes) || bOutdated;
		}

		if (bOutdated)
		{
			OutdatedShaderPipelineTypes.AddUnique(PipelineType);
		}
	}

	for (int32 TypeIndex = 0; TypeIndex < OutdatedShaderPipelineTypes.Num(); TypeIndex++)
	{
		UE_LOG(LogShaders, Warning, TEXT("		Recompiling Pipeline %s"), OutdatedShaderPipelineTypes[TypeIndex]->GetName());
	}
}

const FShaderPipelineType* FShaderPipelineType::GetShaderPipelineTypeByName(FString Name)
{
	for (TLinkedList<FShaderPipelineType*>::TIterator It(GetTypeList()); It; It.Next())
	{
		const FShaderPipelineType* Type = *It;
		if (Name == Type->GetFName())
		{
			return Type;
		}
	}

	return nullptr;
}

const FSHAHash& FShaderPipelineType::GetSourceHash() const
{
	TArray<FString> Filenames;
	for (const FShaderType* ShaderType : Stages)
	{
		Filenames.Add(ShaderType->GetShaderFilename());
	}
	return GetShaderFilesHash(Filenames);
}

FShaderPipeline::FShaderPipeline(
	const FShaderPipelineType* InPipelineType,
	FShader* InVertexShader,
	FShader* InHullShader,
	FShader* InDomainShader,
	FShader* InGeometryShader,
	FShader* InPixelShader) :
	PipelineType(InPipelineType),
	VertexShader(InVertexShader),
	HullShader(InHullShader),
	DomainShader(InDomainShader),
	GeometryShader(InGeometryShader),
	PixelShader(InPixelShader)
{
	check(InPipelineType);
	Validate();
}

FShaderPipeline::FShaderPipeline(const FShaderPipelineType* InPipelineType, const TArray<FShader*>& InStages) :
	PipelineType(InPipelineType),
	VertexShader(nullptr),
	HullShader(nullptr),
	DomainShader(nullptr),
	GeometryShader(nullptr),
	PixelShader(nullptr)
{
	check(InPipelineType);
	for (FShader* Shader : InStages)
	{
		if (Shader)
		{
			switch (Shader->GetType()->GetFrequency())
			{
			case SF_Vertex:
				check(!VertexShader);
				VertexShader = Shader;
				break;
			case SF_Pixel:
				check(!PixelShader);
				PixelShader = Shader;
				break;
			case SF_Hull:
				check(!HullShader);
				HullShader = Shader;
				break;
			case SF_Domain:
				check(!DomainShader);
				DomainShader = Shader;
				break;
			case SF_Geometry:
				check(!GeometryShader);
				GeometryShader = Shader;
				break;
			default:
				checkf(0, TEXT("Invalid stage %u found!"), (uint32)Shader->GetType()->GetFrequency());
				break;
			}
		}
	}

	Validate();
}

FShaderPipeline::FShaderPipeline(const FShaderPipelineType* InPipelineType, const TArray< TRefCountPtr<FShader> >& InStages) :
	PipelineType(InPipelineType),
	VertexShader(nullptr),
	HullShader(nullptr),
	DomainShader(nullptr),
	GeometryShader(nullptr),
	PixelShader(nullptr)
{
	check(InPipelineType);
	for (FShader* Shader : InStages)
	{
		if (Shader)
		{
			switch (Shader->GetType()->GetFrequency())
			{
			case SF_Vertex:
				check(!VertexShader);
				VertexShader = Shader;
				break;
			case SF_Pixel:
				check(!PixelShader);
				PixelShader = Shader;
				break;
			case SF_Hull:
				check(!HullShader);
				HullShader = Shader;
				break;
			case SF_Domain:
				check(!DomainShader);
				DomainShader = Shader;
				break;
			case SF_Geometry:
				check(!GeometryShader);
				GeometryShader = Shader;
				break;
			default:
				checkf(0, TEXT("Invalid stage %u found!"), (uint32)Shader->GetType()->GetFrequency());
				break;
			}
		}
	}

	Validate();
}

FShaderPipeline::~FShaderPipeline()
{
	// Manually set references to nullptr, helps debugging
	VertexShader = nullptr;
	HullShader = nullptr;
	DomainShader = nullptr;
	GeometryShader = nullptr;
	PixelShader = nullptr;
}

void FShaderPipeline::Validate()
{
	for (const FShaderType* Stage : PipelineType->GetStages())
	{
		switch (Stage->GetFrequency())
		{
		case SF_Vertex:
			check(VertexShader && VertexShader->GetType() == Stage);
			break;
		case SF_Pixel:
			check(PixelShader && PixelShader->GetType() == Stage);
			break;
		case SF_Hull:
			check(HullShader && HullShader->GetType() == Stage);
			break;
		case SF_Domain:
			check(DomainShader && DomainShader->GetType() == Stage);
			break;
		case SF_Geometry:
			check(GeometryShader && GeometryShader->GetType() == Stage);
			break;
		default:
			// Can never happen :)
			break;
		}
	}
}

void FShaderPipeline::CookPipeline(FShaderPipeline* Pipeline)
{
#if WITH_EDITOR
	FShaderCodeLibrary::AddShaderPipeline(Pipeline);
#endif
}

FShaderType* FindShaderTypeByName(FString ShaderTypeName)
{
	FShaderType** FoundShader = FShaderType::GetNameToTypeMap().Find(ShaderTypeName);
	if (FoundShader)
	{
		return *FoundShader;
	}

	return nullptr;
}

void DispatchComputeShader(
	FRHICommandList& RHICmdList,
	FShader* Shader,
	uint32 ThreadGroupCountX,
	uint32 ThreadGroupCountY,
	uint32 ThreadGroupCountZ)
{
	RHICmdList.DispatchComputeShader(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void DispatchComputeShader(
	FRHIAsyncComputeCommandListImmediate& RHICmdList,
	FShader* Shader,
	uint32 ThreadGroupCountX,
	uint32 ThreadGroupCountY,
	uint32 ThreadGroupCountZ)
{
	RHICmdList.DispatchComputeShader(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void DispatchIndirectComputeShader(
	FRHICommandList& RHICmdList,
	FShader* Shader,
	FVertexBufferRHIParamRef ArgumentBuffer,
	uint32 ArgumentOffset)
{
	RHICmdList.DispatchIndirectComputeShader(ArgumentBuffer, ArgumentOffset);
}
