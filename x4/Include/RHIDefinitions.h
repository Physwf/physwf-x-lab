#pragma once

#include "X4.h"

enum EShaderFrequency
{
	SF_Vertex		= 0,
	SF_Hull			= 1,
	SF_Domian		= 2,
	SF_Pixel,		= 3,
	SF_Geometry		= 4,
	SF_Compute		= 5,

	SF_NumFrequencies = 6,

	SF_NumBits = 3,
};

enum EShaderPlatform
{
	SP_PCD3D_SM5 = 0,
	SP_OPENGL_SM4 = 1,
	SP_PS4 = 2,
	/** Used when running in Feature Level ES2 in OpenGL. */
	SP_OPENGL_PCES2 = 3,
	SP_XBOXONE_D3D12 = 4,
	SP_PCD3D_SM4 = 5,
	SP_OPENGL_SM5 = 6,
	/** Used when running in Feature Level ES2 in D3D11. */
	SP_PCD3D_ES2 = 7,
	SP_OPENGL_ES2_ANDROID = 8,
	SP_OPENGL_ES2_WEBGL = 9,
	SP_OPENGL_ES2_IOS = 10,
	SP_METAL = 11,
	SP_METAL_MRT = 12,
	SP_OPENGL_ES31_EXT = 13,
	/** Used when running in Feature Level ES3_1 in D3D11. */
	SP_PCD3D_ES3_1 = 14,
	/** Used when running in Feature Level ES3_1 in OpenGL. */
	SP_OPENGL_PCES3_1 = 15,
	SP_METAL_SM5 = 16,
	SP_VULKAN_PCES3_1 = 17,
	SP_METAL_SM5_NOTESS = 18,
	SP_VULKAN_SM4 = 19,
	SP_VULKAN_SM5 = 20,
	SP_VULKAN_ES3_1_ANDROID = 21,
	SP_METAL_MACES3_1 = 22,
	SP_METAL_MACES2 = 23,
	SP_OPENGL_ES3_1_ANDROID = 24,
	SP_SWITCH = 25,
	SP_SWITCH_FORWARD = 26,
	SP_METAL_MRT_MAC = 27,
	SP_VULKAN_SM5_LUMIN = 28,
	SP_VULKAN_ES3_1_LUMIN = 29,

	SP_NumPlatforms = 30,
	SP_NumBits = 5,
};

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

enum EVertexElementType
{
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,	// FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4,
	VET_Short4N,		// 4 X 16 bit word, normalized 
	VET_UShort2,
	VET_UShort4,
	VET_UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
	VET_UShort4N,		// 4 X 16 bit word unsigned, normalized 
	VET_URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
	VET_MAX,

	VET_NumBits = 5,
};