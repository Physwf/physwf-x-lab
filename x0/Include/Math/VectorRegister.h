#pragma once

#include "CoreTypes.h"
#include "Math/XMathUtility.h"

#if PLATFORM_ENABLE_VECTORINTRINSICS
#include "Math/XMathSSE.h"
#else
#include "Math/XMathFPU.h"
#endif
