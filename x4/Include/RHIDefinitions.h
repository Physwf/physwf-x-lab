#pragma once

#include "X4.h"

namespace ERHIFeatureLevel
{
	enum Type
	{
		ES2,
		ES3_1,

		SM4,
		SM5,
		Num
	};
}


enum ERasterizerFillMode
{
	FM_Point,
	FM_WireFrame,
	FM_Solid,

	ERasterizerFillMode_Num,
	ERasterizerFillMode_NumBits = 2,
};

enum ERasterizerCullMode
{
	CM_None,
	CM_CW,
	CM_CCW,

	ERasterizerCullMode_Num,
	ERasterizerCullMode_NumBits = 2,
};