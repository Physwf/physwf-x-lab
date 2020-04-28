#include "RHI/RHI.h"
#include "Modules/ModuleManager.h"

void FRHIResource::FlushPendingDeletes(bool bFlushDeferredDeletes /*= false*/)
{

}

bool FRHIResource::Bypass()
{
	return GRHICommandList.Bypass();
}