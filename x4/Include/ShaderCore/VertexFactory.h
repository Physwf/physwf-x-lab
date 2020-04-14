#pragma once

#include "CoreTypes.h"
#include "RenderCore/RenderResource.h"
#include "RHI/RHIDefinitions.h"
#include "Misc/EnumClassFlags.h"
#include "Misc/AssertionMacros.h"

enum class EVertexStreamUsage : uint8
{
	Default			= 0 << 0,
	Instancing		= 1 << 0,
	Overridden		= 1 << 1,
	ManualFetch		= 1 << 2,
};
ENUM_CLASS_FLAGS(EVertexStreamUsage);

struct FVertexStreamComponent
{
	const FVertexBuffer* VertexBuffer = nullptr;

	uint32 StreamOffset = 0;

	uint8 Offset = 0;

	uint8 Stride;

	EVertexElementType Type = VET_None;

	EVertexStreamUsage VertexStreamUsage = EVertexStreamUsage::Default;

	FVertexStreamComponent()
	{}

	FVertexStreamComponent(const FVertexBuffer* InVertexBuffer, uint32 InOffset, uint32 InStride, EVertexElementType InType, EVertexStreamUsage Usage = EVertexStreamUsage::Default) :
		VertexBuffer(InVertexBuffer),
		StreamOffset(0),
		Offset(InOffset),
		Stride(InStride),
		Type(InType),
		VertexStreamUsage(Usage)
	{
		check(InStride <= 0xFF);
		check(InOffset <= 0xFF);
	}

	FVertexStreamComponent(const FVertexBuffer* InVertexBuffer, uint32 InStreamOffset, uint32 InOffset, uint32 InStride, EVertexElementType InType, EVertexStreamUsage Usage = EVertexStreamUsage::Default) :
		VertexBuffer(InVertexBuffer),
		StreamOffset(InStreamOffset),
		Offset(InOffset),
		Stride(InStride),
		Type(InType),
		VertexStreamUsage(Usage)
	{
		check(InStride <= 0xFF);
		check(InOffset <= 0xFF);
	}
};

class FVertexFactoryShaderParameters
{

};


class FVertexFactoryType
{

};

class FVertexFactory : public FRenderResource
{
public:

	FVertexFactory()
	{

	}

	virtual FVertexFactoryType* GetType() const { return NULL; }
	
	//void SetStreams() const;

	bool bSupportsManualVertexFetch = false;
private:

};