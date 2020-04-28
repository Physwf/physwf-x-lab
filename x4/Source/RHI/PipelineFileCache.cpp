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