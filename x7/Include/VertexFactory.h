#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RenderResource.h"

enum class EVertexStreamUsage : uint8
{
	Default				= 0 << 0,
	Instancing			= 1 << 0,
	Overridden			= 1 << 1,
	ManualFetch			= 1 << 2,
};

struct XVertexStreamComponent
{
	const XVertexBuffer* VertexBuffer = nullptr;

	uint32 StreamOffset = 0;

	uint8 Offset;

	uint8 Stride;

	TEnumAsByte<EVertexElementType> Type = VET_None;

	EVertexStreamUsage VertexStreamUsage = EVertexStreamUsage::Default;

	XVertexStreamComponent()
	{}

	XVertexStreamComponent(const XVertexBuffer* InVertexBuffer, uint32 InOffset, uint32 InStride, EVertexElementType InType, EVertexStreamUsage Usage = EVertexStreamUsage::Default) :
		VertexBuffer(InVertexBuffer),
		StreamOffset(0),
		Offset(InOffset),
		Stride(InStride),
		Type(InType),
		VertexStreamUsage(Usage)
	{

	}

	XVertexStreamComponent(const XVertexBuffer* InVertexBuffer, uint32 InStreamOffset, uint32 InOffset, uint32 InStride, EVertexElementType InType, EVertexStreamUsage Usage = EVertexStreamUsage::Default) :
		VertexBuffer(InVertexBuffer),
		StreamOffset(InStreamOffset),
		Offset(InOffset),
		Stride(InStride),
		Type(InType),
		VertexStreamUsage(Usage)
	{

	}
};

class XVertexFactoryShaderParameters
{
public:
	virtual ~XVertexFactoryShaderParameters();
	virtual void Bind(const class XShaderParameterMap& ParameterMap) = 0;
	virtual void SetMesh();

};