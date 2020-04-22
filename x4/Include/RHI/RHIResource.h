#pragma once

#include "X4.h"

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Misc/Crc.h"
#include "Containers/UnrealString.h"
#include "HAL/ThreadSafeCounter.h"
#include "RHIDefinitions.h"
#include "Templates/RefCounting.h"
#include "PixelFormat.h"

class FRHIResource
{
public:
	FRHIResource(bool InbDoNotDeferDelete = false)
		: MarkedForDelete(0)
		, bDoNotDeferDelete(InbDoNotDeferDelete)
		, bCommitted(true)
	{
	}

	FORCEINLINE_DEBUGGABLE uint32 AddRef() const
	{
		int32 NewValue = NumRefs.Increment();
		checkSlow(NewValue > 0);
		return uint32(NewValue);
	}
	FORCEINLINE_DEBUGGABLE uint32 Release() const
	{
		int32 NewValue = NumRefs.Decrement();
		if (NewValue == 0)
		{
			delete this;
		}
		checkSlow(NewValue >= 0);
		return uint32(NewValue);
	}
	FORCEINLINE_DEBUGGABLE uint32 GetRefCount() const
	{
		int32 CurrentValue = NumRefs.GetValue();
		checkSlow(CurrentValue >= 0);
		return uint32(CurrentValue);
	}

	// Transient resource tracking
	// We do this at a high level so we can catch errors even when transient resources are not supported
	void SetCommitted(bool bInCommitted)
	{
		//check(IsInRenderingThread());
		bCommitted = bInCommitted;
	}
	bool IsCommitted() const
	{
		//check(IsInRenderingThread());
		return bCommitted;
	}
private:
	mutable FThreadSafeCounter NumRefs;
	mutable int32 MarkedForDelete;
	bool bDoNotDeferDelete;
	bool bCommitted;


};

class FRHISamplerState : public FRHIResource {};
class FRHIRasterizerState : public FRHIResource
{
public:
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};
class FRHIDepthStencilState : public FRHIResource
{
public:
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};
class FRHIBlendState : public FRHIResource
{
public:
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};

typedef TArray<struct FVertexElement, TFixedAllocator<MaxVertexElementCount> > FVertexDeclarationElementList;
class FRHIVertexDeclaration : public FRHIResource
{
public:
	virtual bool GetInitializer(FVertexDeclarationElementList& Init) { return false; }
};

class FRHIBoundShaderState : public FRHIResource {};


class FRHIShader : public FRHIResource
{
// public:
// 	void SetHash(FSHAHash InHash) { Hash = InHash; }
// 	FSHAHash GetHash() const { return Hash; }
// 
// private:
// 	FSHAHash Hash;
};

class FRHIVertexShader : public FRHIShader {};
class FRHIHullShader : public FRHIShader {};
class FRHIDomainShader : public FRHIShader {};
class FRHIPixelShader : public FRHIShader {};
class FRHIGeometryShader : public FRHIShader {};
class X4_API FRHIComputeShader : public FRHIShader
{
public:
	FRHIComputeShader() : Stats(nullptr) {}

	inline void SetStats(struct FPipelineStateStats* Ptr) { Stats = Ptr; }
	void UpdateStats();

private:
	struct FPipelineStateStats* Stats;
};

class FRHIGraphicsPipelineState : public FRHIResource {};
class FRHIComputePipelineState : public FRHIResource {};


struct FRHIUniformBufferLayout
{
	uint32 ConstantBufferSize;
	TArray<uint16> ResourceOffsets;
	TArray<uint8> Resources;

	inline uint32 GetHash() const 
	{
		checkSlow(Hash != 0);
		return Hash;
	}

	void ComputeHash()
	{
		uint32 TmpHash = ConstantBufferSize << 16;

		for (int32 ResourceIndex = 0; ResourceIndex < ResourceOffsets.Num(); ResourceIndex++)
		{
			// Offset and therefore hash must be the same regardless of pointer size
			checkSlow(ResourceOffsets[ResourceIndex] == Align(ResourceOffsets[ResourceIndex], 8));
			TmpHash ^= ResourceOffsets[ResourceIndex];
		}

		uint32 N = Resources.Num();
		while (N >= 4)
		{
			TmpHash ^= (Resources[--N] << 0);
			TmpHash ^= (Resources[--N] << 8);
			TmpHash ^= (Resources[--N] << 16);
			TmpHash ^= (Resources[--N] << 24);
		}
		while (N >= 2)
		{
			TmpHash ^= Resources[--N] << 0;
			TmpHash ^= Resources[--N] << 16;
		}
		while (N > 0)
		{
			TmpHash ^= Resources[--N];
		}
		Hash = TmpHash;
	}

	explicit FRHIUniformBufferLayout(FString InName) :
		ConstantBufferSize(0),
		Name(InName),
		Hash(0)
	{
	}

	enum EInit
	{
		Zero
	};
	explicit FRHIUniformBufferLayout(EInit) :
		ConstantBufferSize(0),
		Name(FString()),
		Hash(0)
	{
	}

	void CopyFrom(const FRHIUniformBufferLayout& Source)
	{
		ConstantBufferSize = Source.ConstantBufferSize;
		ResourceOffsets = Source.ResourceOffsets;
		Resources = Source.Resources;
		Name = Source.Name;
		Hash = Source.Hash;
	}
private:
	// for debugging / error message
	FString Name;

	uint32 Hash;
};

inline bool operator==(const FRHIUniformBufferLayout& A, const FRHIUniformBufferLayout& B)
{
	return A.ConstantBufferSize == B.ConstantBufferSize
		&& A.ResourceOffsets == B.ResourceOffsets
		&& A.Resources == B.Resources;
}

class FRHIUniformBuffer : public FRHIResource
{
public:
	FRHIUniformBuffer(const FRHIUniformBufferLayout& InLayout)
		: Layout(&InLayout)
		, LayoutConstantBufferSize(InLayout.ConstantBufferSize)
	{}

	uint32 GetSize() const
	{
		check(LayoutConstantBufferSize == Layout->ConstantBufferSize);
		return LayoutConstantBufferSize;
	}

	const FRHIUniformBufferLayout& GetLayout() const { return *Layout; }
private:
	const FRHIUniformBufferLayout* Layout;

	uint32 LayoutConstantBufferSize;
};

class FRHIIndexBuffer : public FRHIResource
{
public:
	FRHIIndexBuffer(uint32 InStride, uint32 InSize, uint32 InUsage)
		: Stride(InStride)
		, Size(InSize)
		, Usage(InUsage)
	{}

	uint32 GetStride() const { return Stride; }
	uint32 GetSize() const { return Size; }
	uint32 GetUsage() const { return Usage; }
private:
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};

class FRHIVertexBuffer : public FRHIResource
{
public:
	FRHIVertexBuffer(uint32 InSize,uint32 InUsage)
		: Size(InSize)
		, Usage(InUsage)
	{}
private:
	uint32 Size;
	uint32 Usage;
};

class FRHIStructureBuffer : public FRHIResource
{
public:
	FRHIStructureBuffer(uint32 InStride, uint32 InSize, uint32 InUsage)
		: Stride(InStride)
		, Size(InSize)
		, Usage(InUsage)
	{}

	uint32 GetStride() const { return Stride; }
	uint32 GetSize() const { return Size; }
	uint32 GetUsage() const { return Usage; }
private:
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};

class X4_API FLastRenderTimeContainer
{
	FLastRenderTimeContainer():LastRenderTime(-FLT_MAX) {}

	double GetLastRenderTime() const { return LastRenderTime; }
	inline void SetLastRenderTime(double InLastRenderTime)
	{
		if (LastRenderTime != InLastRenderTime)
		{
			LastRenderTime = InLastRenderTime;
		}
	}
private:
	double LastRenderTime;
};

class X4_API FRHITexture : public FRHIResource
{
public:
private:
	FClearValueBinding
};