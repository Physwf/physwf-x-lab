#pragma once

#include "CoreMinimal.h"
#include "Containers/List.h"
#include "Containers/StaticArray.h"
#include "RHI/RHI.h"
#include "RenderingThread.h"
#include "RenderResource.h"
#include "Templates/IsArrayOrRefOfType.h"

class FShaderUniformBufferParameter;
template<typename TBufferStruct> class TShaderUniformBufferParameter;

namespace EShaderPrecisionModifier
{
	enum Type
	{
		Float,
		Half,
		Fixed
	};
};

/** A uniform buffer resource. */
template<typename TBufferStruct>
class TUniformBuffer : public FRenderResource
{
public:

	TUniformBuffer()
		: BufferUsage(UniformBuffer_MultiFrame)
		, Contents(nullptr)
	{
	}

	~TUniformBuffer()
	{
		if (Contents)
		{
			FMemory::Free(Contents);
		}
	}

	/** Sets the contents of the uniform buffer. */
	void SetContents(const TBufferStruct& NewContents)
	{
		SetContentsNoUpdate(NewContents);
		UpdateRHI();
	}
	/** Sets the contents of the uniform buffer to all zeros. */
	void SetContentsToZero()
	{
		if (!Contents)
		{
			Contents = (uint8*)FMemory::Malloc(sizeof(TBufferStruct), UNIFORM_BUFFER_STRUCT_ALIGNMENT);
		}
		FMemory::Memzero(Contents, sizeof(TBufferStruct));
		UpdateRHI();
	}

	// FRenderResource interface.
	virtual void InitDynamicRHI() override
	{
		check(IsInRenderingThread());
		UniformBufferRHI.SafeRelease();
		if (Contents)
		{
			UniformBufferRHI = RHICreateUniformBuffer(Contents, TBufferStruct::StaticStruct.GetLayout(), BufferUsage);
		}
	}
	virtual void ReleaseDynamicRHI() override
	{
		UniformBufferRHI.SafeRelease();
	}

	// Accessors.
	FUniformBufferRHIParamRef GetUniformBufferRHI() const
	{
		check(UniformBufferRHI.GetReference()); // you are trying to use a UB that was never filled with anything
		return UniformBufferRHI;
	}

	EUniformBufferUsage BufferUsage;

protected:

	/** Sets the contents of the uniform buffer. Used within calls to InitDynamicRHI */
	void SetContentsNoUpdate(const TBufferStruct& NewContents)
	{
		check(IsInRenderingThread());
		if (!Contents)
		{
			Contents = (uint8*)FMemory::Malloc(sizeof(TBufferStruct), UNIFORM_BUFFER_STRUCT_ALIGNMENT);
		}
		FMemory::Memcpy(Contents, &NewContents, sizeof(TBufferStruct));
	}

private:

	FUniformBufferRHIRef UniformBufferRHI;
	uint8* Contents;
};

template<typename TBufferStruct>
class TUniformBufferRef : public FUniformBufferRHIRef
{
public:

	/** Initializes the reference to null. */
	TUniformBufferRef()
	{}

	/** Initializes the reference to point to a buffer. */
	TUniformBufferRef(const TUniformBuffer<TBufferStruct>& InBuffer)
		: FUniformBufferRHIRef(InBuffer.GetUniformBufferRHI())
	{}

	/** Creates a uniform buffer with the given value, and returns a structured reference to it. */
	static TUniformBufferRef<TBufferStruct> CreateUniformBufferImmediate(const TBufferStruct& Value, EUniformBufferUsage Usage)
	{
		check(IsInRenderingThread() || IsInRHIThread());
		return TUniformBufferRef<TBufferStruct>(RHICreateUniformBuffer(&Value, TBufferStruct::StaticStruct.GetLayout(), Usage));
	}
	/** Creates a uniform buffer with the given value, and returns a structured reference to it. */
	static FLocalUniformBuffer CreateLocalUniformBuffer(FRHICommandList& RHICmdList, const TBufferStruct& Value, EUniformBufferUsage Usage)
	{
		return RHICmdList.BuildLocalUniformBuffer(&Value, sizeof(TBufferStruct), TBufferStruct::StaticStruct.GetLayout());
	}

private:

	/** A private constructor used to coerce an arbitrary RHI uniform buffer reference to a structured reference. */
	TUniformBufferRef(FUniformBufferRHIParamRef InRHIRef)
		: FUniformBufferRHIRef(InRHIRef)
	{}
};