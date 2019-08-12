#pragma once

#include "RenderResource.h"

class XColorVertexBuffer : public XVertexBuffer
{
public:
	XColorVertexBuffer();
	~XColorVertexBuffer();

	void CleanUp();

	void Init(uint32 NumVertices);

	uint32 GetStride()
	{
		return Stride;
	}

	uint32 GetNumVertices()
	{
		return NumVertices;
	}

	uint32 GetAllocatedSize() const;

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;

private:
	class XColorVertexData* VertexData;

	XShaderResourceViewRHIRef ColorComponentSRV;

	uint8* Data;

	uint32 Stride;

	uint32 NumVertices;

	void AllocateData(bool bNeedCPUAccess = true);
};