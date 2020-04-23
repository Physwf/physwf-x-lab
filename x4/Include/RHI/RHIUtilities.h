#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "Math/XMathUtility.h"
#include "Containers/UnrealString.h"
#include "RHIDefinitions.h"
//#include "RHICommandList.h"

class FExclusiveDepthStencil;
class FRHIDepthRenderTargetView;
class FRHIRenderTargetView;
class FRHISetRenderTargetsInfo;
struct FRHIResourceCreateInfo;
enum class EResourceTransitionAccess;
enum class ESimpleRenderTargetMode;


static inline bool IsDepthOrStencilFormat(EPixelFormat Format)
{
	switch (Format)
	{
	case PF_D24:
	case PF_DepthStencil:
	case PF_X24_G8:
	case PF_ShadowDepth:
		return true;

	default:
		break;
	}

	return false;
}
