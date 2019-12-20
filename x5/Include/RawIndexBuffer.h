#pragma once

#include <vector>

#include "CoreTypes.h"
#include "RenderResource.h"

class FRawIndexBuffer : public FIndexBuffer
{
public:
	std::vector<uint16> Indices;

	virtual void InitRHI() override;
};

class FRawIndexBuffer16or32 : public FIndexBuffer
{
public:
	std::vector<uint32> Indices;

	virtual void InitRHI() override;
};

/**
 * Desired stride when creating a static index buffer.
 */
namespace EIndexBufferStride
{
	enum Type
	{
		/** Forces all indices to be 16-bit. */
		Force16Bit = 1,
		/** Forces all indices to be 32-bit. */
		Force32Bit = 2,
		/** Use 16 bits unless an index exceeds MAX_uint16. */
		AutoDetect = 3
	};
}


class FRawStaticIndexBuffer : public FIndexBuffer
{
public:

	inline void SetIndex(const uint32 At, const uint32 NewIndexValue)
	{
		check(At >= 0 && At < (uint32)IndexStorage.Num());

		if (b32Bit)
		{
			uint32* Indices32Bit = (uint32*)IndexStorage.GetData();
			Indices32Bit[At] = NewIndexValue;
		}
		else
		{
			uint16* Indices16Bit = (uint16*)IndexStorage.GetData();
			Indices16Bit[At] = (uint16)NewIndexValue;
		}
	}

	/**
	 * Set the indices stored within this buffer.
	 * @param InIndices		The new indices to copy in to the buffer.
	 * @param DesiredStride	The desired stride (16 or 32 bits).
	 */
	void SetIndices(const std::vector<uint32>& InIndices, EIndexBufferStride::Type DesiredStride);

	/**
	 * Insert indices at the given position in the buffer
	 * @param	At					Index to insert at
	 * @param	IndicesToAppend		Pointer to the array of indices to insert
	 * @param	NumIndicesToAppend	How many indices are in the IndicesToAppend array
	 */
	void InsertIndices(const uint32 At, const uint32* IndicesToAppend, const uint32 NumIndicesToAppend);

	/**
	 * Append indices to the end of the buffer
	 * @param	IndicesToAppend		Pointer to the array of indices to add to the end
	 * @param	NumIndicesToAppend	How many indices are in the IndicesToAppend array
	 */
	void AppendIndices(const uint32* IndicesToAppend, const uint32 NumIndicesToAppend);


	/** @return Gets a specific index value */
	inline uint32 GetIndex(const uint32 At) const
	{
		check(At >= 0 && At < (uint32)IndexStorage.size());
		uint32 IndexValue;
		if (b32Bit)
		{
			const uint32* SrcIndices32Bit = (const uint32*)IndexStorage.GetData();
			IndexValue = SrcIndices32Bit[At];
		}
		else
		{
			const uint16* SrcIndices16Bit = (const uint16*)IndexStorage.GetData();
			IndexValue = SrcIndices16Bit[At];
		}

		return IndexValue;
	}


	/**
	 * Removes indices from the buffer
	 *
	 * @param	At	The index of the first index to remove
	 * @param	NumIndicesToRemove	How many indices to remove
	 */
	void RemoveIndicesAt(const uint32 At, const uint32 NumIndicesToRemove);

	/**
	 * Retrieve a copy of the indices in this buffer. Only valid if created with
	 * NeedsCPUAccess set to true or the resource has not yet been initialized.
	 * @param OutIndices	Array in which to store the copy of the indices.
	 */
	void GetCopy(std::vector<uint32>& OutIndices) const;

	/**
	 * Get the direct read access to index data
	 * Only valid if NeedsCPUAccess = true and indices are 16 bit
	 */
	const uint16* AccessStream16() const;

	/**
	 * Retrieves an array view in to the index buffer. The array view allows code
	 * to retrieve indices as 32-bit regardless of how they are stored internally
	 * without a copy. The array view is valid only if:
	 *		The buffer was created with NeedsCPUAccess = true
	 *		  OR the resource has not yet been initialized
	 *		  AND SetIndices has not been called since.
	 */
	//FIndexArrayView GetArrayView() const;

	/**
	 * Computes the number of indices stored in this buffer.
	 */
	inline int32 GetNumIndices() const
	{
		return b32Bit ? (IndexStorage.Num() / 4) : (IndexStorage.Num() / 2);
	}

	/**
	 * Computes the amount of memory allocated to store the indices.
	 */
	inline uint32 GetAllocatedSize() const
	{
		return IndexStorage.GetAllocatedSize();
	}
	/**
	 * Discard
	 * discards the serialized data when it is not needed
	 */
	void Discard();

	// FRenderResource interface.
	virtual void InitRHI() override;

	inline bool Is32Bit() const { return b32Bit; }
private:
	/** Storage for indices. */
	std::vector<uint8> IndexStorage;
	/** 32bit or 16bit? */
	bool b32Bit;
};