#pragma once

#include <vector>

#include "Containers/DynamicRHIResourceArray.h"
#include "Rendering/StaticMeshVertexDataInterface.h"

template<typename VertexDataType>
class TStaticMeshVertexData :
	public FStaticMeshVertexDataInterface
{
	TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT> Data;
public:

	/**
	* Constructor
	* @param InNeedsCPUAccess - true if resource array data should be CPU accessible
	*/
	TStaticMeshVertexData(bool InNeedsCPUAccess = false)
		: Data(InNeedsCPUAccess)
	{
	}

	/**
	* Resizes the vertex data buffer, discarding any data which no longer fits.
	*
	* @param NumVertices - The number of vertices to allocate the buffer for.
	* @param BufferFlags - Flags to define the expected behavior of the buffer
	*/
	void ResizeBuffer(uint32 NumVertices, EResizeBufferFlags BufferFlags = EResizeBufferFlags::None) override
	{
		if ((uint32)Data.size() < NumVertices)
		{
			// Enlarge the array.
			if (!EnumHasAnyFlags(BufferFlags, EResizeBufferFlags::AllowSlackOnGrow))
			{
				Data.reserve(NumVertices);
			}

			Data.resize(NumVertices);
		}
		else if ((uint32)Data.size() > NumVertices)
		{
			// Shrink the array.
			//bool AllowShinking = !EnumHasAnyFlags(BufferFlags, EResizeBufferFlags::AllowSlackOnReduce);
			//Data.RemoveAt(NumVertices, Data.Num() - NumVertices, AllowShinking);
			Data.erase(Data.begin() + NumVertices, Data.end());
		}
	}

	void Empty(uint32 NumVertices) override
	{
		Data.clear();
	}

	bool IsValidIndex(uint32 Index) override
	{
		return (Index >= 0 && Index < Data.size());
	}

	/**
	* @return stride of the vertex type stored in the resource data array
	*/
	uint32 GetStride() const override
	{
		return sizeof(VertexDataType);
	}
	/**
	* @return uint8 pointer to the resource data array
	*/
	uint8* GetDataPointer() override
	{
		return (uint8*)Data.data();
	}

	/**
	* @return resource array interface access
	*/
	FResourceArrayInterface* GetResourceArray() override
	{
		return &Data;
	}
	/**
	* Assignment. This is currently the only method which allows for
	* modifying an existing resource array
	*/
	void Assign(const std::vector<VertexDataType>& Other)
	{
		ResizeBuffer((int32)Other.size());
		if (Other.size())
		{
			memcpy(GetDataPointer(), &Other[0], Other.Num() * sizeof(VertexDataType));
		}
	}

	/**
	* Helper function to return the amount of memory allocated by this
	* container.
	*
	* @returns Number of bytes allocated by this container.
	*/
	uint32 GetResourceSize() const override
	{
		return Data.size() * sizeof(VertexDataType);;
	}

	/**
	* Helper function to return the number of elements by this
	* container.
	*
	* @returns Number of elements allocated by this container.
	*/
	virtual int32 Num() const override
	{
		return (int32)Data.size();
	}

	bool GetAllowCPUAccess() const override
	{
		return Data.GetAllowCPUAccess();
	}
};