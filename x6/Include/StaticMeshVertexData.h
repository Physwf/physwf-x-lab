#pragma once

#include "Containers/DyamiacRHIResourceArray.h"
#include "Rendering/StaticMeshVertexDataInterface.h"

template <typename VertexDataType>
class TStaicMeshVertexData :
	public XStaticMeshVertexDataInterface
{
	TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT> Data;

public:

	TStaicMeshVertexData(bool bNeedCPUAccess = false)
		:Data(bNeedCPUAccess)
	{

	}

	virtual void ResizeBuffer(uint32 NumVertices, EResizeBufferFlags BufferFlags = EResizeBufferFlags::None) override
	{

	}

	virtual void Empty(uint32 NumVertices) override
	{
		Data.Empty(NumVertices);
	}

	virtual bool IsValidIndex(uint32 Index) override
	{
		return Data.IsValidIndex(Index);
	}

	uint32 GetStride() const override
	{
		return sizeof(VertexDataType);
	}

	virtual uint8* GetDataPointer() override
	{
		return (uint8*)Data.data();
	}

	XResourceArrayInterface* GetResourceArray() override
	{
		return &Data;
	}

	SIZE_T GetResourceSize() const override
	{
		Data.GetAllocatedSize();
	}

	virtual int32 Num() const
	{
		return Data.Num();
	}

	bool GetAllowCPUAccess() const override
	{
		return Data.GetAllowCPUAccess();
	}
};