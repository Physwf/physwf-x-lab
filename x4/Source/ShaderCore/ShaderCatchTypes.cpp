#include "ShaderCore/ShaderCacheTypes.h"
#include "ShaderCore/ShaderCache.h"
#include "Serialization/MemoryWriter.h"

uint32 FShaderDrawKey::CurrentMaxResources = EShaderCacheMaxNumResources;

static FORCEINLINE uint32 CalculateSizeOfSamplerStateInitializer()
{
	static uint32 SizeOfSamplerStateInitializer = 0;
	if (SizeOfSamplerStateInitializer == 0)
	{
		TArray<uint8> Data;
		FMemoryWriter Writer(Data);
		FSamplerStateInitializerRHI State;
		Writer << State;
		SizeOfSamplerStateInitializer = Data.Num();
	}
	return SizeOfSamplerStateInitializer;
}

bool FSamplerStateInitializerRHIKeyFuncs::Matches(KeyInitType A, KeyInitType B)
{
	return FMemory::Memcmp(&A, &B, CalculateSizeOfSamplerStateInitializer()) == 0;
}

uint32 FSamplerStateInitializerRHIKeyFuncs::GetKeyHash(KeyInitType Key)
{
	return FCrc::MemCrc_DEPRECATED(&Key, CalculateSizeOfSamplerStateInitializer());
}
