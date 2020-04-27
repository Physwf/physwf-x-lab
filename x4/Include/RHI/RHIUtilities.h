#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "Math/XMathUtility.h"
#include "Containers/UnrealString.h"
#include "RHIDefinitions.h"
#include "RHICommandList.h"

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

/** Encapsulates a GPU read/write buffer with its UAV and SRV. */
struct FRWBuffer
{
	FVertexBufferRHIRef Buffer;
	FUnorderedAccessViewRHIRef UAV;
	FShaderResourceViewRHIRef SRV;
	uint32 NumBytes;

	FRWBuffer()
		: NumBytes(0)
	{}

	~FRWBuffer()
	{
		Release();
	}

	// @param AdditionalUsage passed down to RHICreateVertexBuffer(), get combined with "BUF_UnorderedAccess | BUF_ShaderResource" e.g. BUF_Static
	void Initialize(uint32 BytesPerElement, uint32 NumElements, EPixelFormat Format, uint32 AdditionalUsage = 0, const TCHAR* InDebugName = NULL, FResourceArrayInterface *InResourceArray = nullptr)
	{
		check(GMaxRHIFeatureLevel == ERHIFeatureLevel::SM5
			|| IsVulkanPlatform(GMaxRHIShaderPlatform)
			|| IsMetalPlatform(GMaxRHIShaderPlatform)
			|| (GMaxRHIFeatureLevel == ERHIFeatureLevel::ES3_1 && GSupportsResourceView)
		);

		// Provide a debug name if using Fast VRAM so the allocators diagnostics will work
		ensure(!((AdditionalUsage & BUF_FastVRAM) && !InDebugName));
		NumBytes = BytesPerElement * NumElements;
		FRHIResourceCreateInfo CreateInfo;
		CreateInfo.ResourceArray = InResourceArray;
		CreateInfo.DebugName = InDebugName;
		Buffer = RHICreateVertexBuffer(NumBytes, BUF_UnorderedAccess | BUF_ShaderResource | AdditionalUsage, CreateInfo);
		UAV = RHICreateUnorderedAccessView(Buffer, Format);
		SRV = RHICreateShaderResourceView(Buffer, BytesPerElement, Format);
	}

	void AcquireTransientResource()
	{
		RHIAcquireTransientResource(Buffer);
	}
	void DiscardTransientResource()
	{
		RHIDiscardTransientResource(Buffer);
	}

	void Release()
	{
		int32 BufferRefCount = Buffer ? Buffer->GetRefCount() : -1;

		if (BufferRefCount == 1)
		{
			DiscardTransientResource();
		}

		NumBytes = 0;
		Buffer.SafeRelease();
		UAV.SafeRelease();
		SRV.SafeRelease();
	}
};

/** Encapsulates a GPU read buffer with its SRV. */
struct FReadBuffer
{
	FVertexBufferRHIRef Buffer;
	FShaderResourceViewRHIRef SRV;
	uint32 NumBytes;

	FReadBuffer() : NumBytes(0) {}

	void Initialize(uint32 BytesPerElement, uint32 NumElements, EPixelFormat Format, uint32 AdditionalUsage = 0)
	{
		check(GSupportsResourceView);
		NumBytes = BytesPerElement * NumElements;
		FRHIResourceCreateInfo CreateInfo;
		Buffer = RHICreateVertexBuffer(NumBytes, BUF_ShaderResource | AdditionalUsage, CreateInfo);
		SRV = RHICreateShaderResourceView(Buffer, BytesPerElement, Format);
	}

	void Release()
	{
		NumBytes = 0;
		Buffer.SafeRelease();
		SRV.SafeRelease();
	}
};

/** Encapsulates a GPU read buffer with its SRV. */
struct FReadBuffer
{
	FVertexBufferRHIRef Buffer;
	FShaderResourceViewRHIRef SRV;
	uint32 NumBytes;

	FReadBuffer() : NumBytes(0) {}

	void Initialize(uint32 BytesPerElement, uint32 NumElements, EPixelFormat Format, uint32 AdditionalUsage = 0)
	{
		check(GSupportsResourceView);
		NumBytes = BytesPerElement * NumElements;
		FRHIResourceCreateInfo CreateInfo;
		Buffer = RHICreateVertexBuffer(NumBytes, BUF_ShaderResource | AdditionalUsage, CreateInfo);
		SRV = RHICreateShaderResourceView(Buffer, BytesPerElement, Format);
	}

	void Release()
	{
		NumBytes = 0;
		Buffer.SafeRelease();
		SRV.SafeRelease();
	}
};

/** Encapsulates a GPU read/write structured buffer with its UAV and SRV. */
struct FRWBufferStructured
{
	FStructuredBufferRHIRef Buffer;
	FUnorderedAccessViewRHIRef UAV;
	FShaderResourceViewRHIRef SRV;
	uint32 NumBytes;

	FRWBufferStructured() : NumBytes(0) {}

	~FRWBufferStructured()
	{
		Release();
	}

	void Initialize(uint32 BytesPerElement, uint32 NumElements, uint32 AdditionalUsage = 0, const TCHAR* InDebugName = NULL, bool bUseUavCounter = false, bool bAppendBuffer = false)
	{
		check(GMaxRHIFeatureLevel == ERHIFeatureLevel::SM5);
		// Provide a debug name if using Fast VRAM so the allocators diagnostics will work
		ensure(!((AdditionalUsage & BUF_FastVRAM) && !InDebugName));

		NumBytes = BytesPerElement * NumElements;
		FRHIResourceCreateInfo CreateInfo;
		CreateInfo.DebugName = InDebugName;
		Buffer = RHICreateStructuredBuffer(BytesPerElement, NumBytes, BUF_UnorderedAccess | BUF_ShaderResource | AdditionalUsage, CreateInfo);
		UAV = RHICreateUnorderedAccessView(Buffer, bUseUavCounter, bAppendBuffer);
		SRV = RHICreateShaderResourceView(Buffer);
	}

	void Release()
	{
		int32 BufferRefCount = Buffer ? Buffer->GetRefCount() : -1;

		if (BufferRefCount == 1)
		{
			DiscardTransientResource();
		}

		NumBytes = 0;
		Buffer.SafeRelease();
		UAV.SafeRelease();
		SRV.SafeRelease();
	}

	void AcquireTransientResource()
	{
		RHIAcquireTransientResource(Buffer);
	}
	void DiscardTransientResource()
	{
		RHIDiscardTransientResource(Buffer);
	}
};

struct FByteAddressBuffer
{
	FStructuredBufferRHIRef Buffer;
	FShaderResourceViewRHIRef SRV;
	uint32 NumBytes;

	FByteAddressBuffer() : NumBytes(0) {}

	void Initialize(uint32 InNumBytes, uint32 AdditionalUsage = 0)
	{
		NumBytes = InNumBytes;
		check(GMaxRHIFeatureLevel == ERHIFeatureLevel::SM5);
		check(NumBytes % 4 == 0);
		FRHIResourceCreateInfo CreateInfo;
		Buffer = RHICreateStructuredBuffer(4, NumBytes, BUF_ShaderResource | BUF_ByteAddressBuffer | AdditionalUsage, CreateInfo);
		SRV = RHICreateShaderResourceView(Buffer);
	}

	void Release()
	{
		NumBytes = 0;
		Buffer.SafeRelease();
		SRV.SafeRelease();
	}
};

/** Encapsulates a GPU read/write ByteAddress buffer with its UAV and SRV. */
struct FRWByteAddressBuffer : public FByteAddressBuffer
{
	FUnorderedAccessViewRHIRef UAV;

	void Initialize(uint32 InNumBytes, uint32 AdditionalUsage = 0)
	{
		FByteAddressBuffer::Initialize(InNumBytes, BUF_UnorderedAccess | AdditionalUsage);
		UAV = RHICreateUnorderedAccessView(Buffer, false, false);
	}

	void Release()
	{
		FByteAddressBuffer::Release();
		UAV.SafeRelease();
	}
};

struct FDynamicReadBuffer : public FReadBuffer
{
	/** Pointer to the vertex buffer mapped in main memory. */
	uint8* MappedBuffer;

	/** Default constructor. */
	FDynamicReadBuffer()
		: MappedBuffer(nullptr)
	{
	}

	virtual ~FDynamicReadBuffer()
	{
		Release();
	}

	virtual void Initialize(uint32 BytesPerElement, uint32 NumElements, EPixelFormat Format, uint32 AdditionalUsage = 0)
	{
		ensure(
			AdditionalUsage & (BUF_Dynamic | BUF_Volatile | BUF_Static) &&								// buffer should be Dynamic or Volatile or Static
			(AdditionalUsage & (BUF_Dynamic | BUF_Volatile)) ^ (BUF_Dynamic | BUF_Volatile) // buffer should not be both
		);

		FReadBuffer::Initialize(BytesPerElement, NumElements, Format, AdditionalUsage);
	}

	/**
	* Locks the vertex buffer so it may be written to.
	*/
	void Lock()
	{
		check(MappedBuffer == nullptr);
		check(IsValidRef(Buffer));
		MappedBuffer = (uint8*)RHILockVertexBuffer(Buffer, 0, NumBytes, RLM_WriteOnly);
	}

	/**
	* Unocks the buffer so the GPU may read from it.
	*/
	void Unlock()
	{
		check(MappedBuffer);
		check(IsValidRef(Buffer));
		RHIUnlockVertexBuffer(Buffer);
		MappedBuffer = nullptr;
	}
};