#pragma once

#include "CoreTypes.h"
#include "Templates/RefCounting.h"

class XRHIResource
{
public:
	XRHIResource() :NumRefs(0)
	{

	}

	virtual ~XRHIResource()
	{
	}

	uint32 AddRef() const
	{
		return NumRefs++;
	}

	uint32 Release() const
	{
		uint32 NewValue = --NumRefs;
		if (NewValue == 0)
		{
			delete this;
		}
	}

	uint32 GetRefCount() const 
	{
		return NumRefs;
	}


private:

	mutable uint32 NumRefs;
};

class XRHIIndexBuffer : public XRHIResource
{
public:
	XRHIIndexBuffer(uint32 InStride, uint32 InSize, uint32 InUsage)
		:Stride(InStride),
		,Size(InSize)
		,Usage(InUsage)
	{}

	uint32 GetStride() const { return Stride; }

	uint32 GetSize() const { return Size; }

	uint32 GetUsage() const { return Usage; }
private:
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};

class XRHIVertexBuffer : public XRHIResource
{
public:
	XRHIVertexBuffer(uint32 InSize, uint32 InUsage)
		:Size(InSize)
		,Usage(InUsage)
	{}

	uint32 GetSize() const { return Size; }

	uint32 GetUsage() const { return Usage; }
private:
	uint32 Size;
	uint32 Usage;
};


typedef XRHIVertexBuffer*				XVertexBufferRHIParamRef;
typedef TRefCountPtr<XRHIVertexBuffer>	XVertexBufferRef;

typedef XRHIIndexBuffer*				XIndexBufferRHIParamRef;
typedef TRefCountPtr<XRHIIndexBuffer>	XIndexBufferRef;