#pragma once

enum class EResizeBufferFlags
{
	None								= 0,
	AllowSlackOnGrow					= 1 << 0,
	AllowSlackOnReduce					= 1 << 1,
};

class XStaticMeshVertexDataInterface
{
public:

	virtual ~XStaticMeshVertexDataInterface() {}

	virtual void ResizeBuffer(uint32 NumVertices, EResizeBufferFlags BufferFlags = EResizeBufferFlags::None) = 0;

	virtual void Empty(uint32 NumVertices) = 0;

	virtual bool IsValidIndex(uint32 Index) = 0;

	virtual int32 Num() = 0;

	virtual uint8* GetDataPointer() = 0;

	virtual SIZE_T GetResourceSize() const = 0;

	virtual bool GetAllowCPUAccess() const = 0;
};