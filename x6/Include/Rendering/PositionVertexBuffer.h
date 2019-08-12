#pragma once

#include "RenderResource.h"

struct XPositionVertex
{
	FVector Position;
};

class XPositionVetexBuffer : public XVertexBuffer
{
public:
	XPositionVetexBuffer();

	~XPositionVetexBuffer();

	void CleanUp();

	void Init(int32 NumVertices, bool bNeedCPUAcess = true);

	void Init(const TArray<FVector>& InPositions);

	uint32 GetStride() const
	{
		return Stride;
	}

	uint32 GetNumVertices() const
	{
		return NumVertices;
	}

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;

	void* GetVertexData()
	{
		return Data;
	}
private:
	XShaderResourceViewRHIRef PositionComponentSRV;

	class XPositionVertexData* VertexData;

	uint8* Data;

	uint32 Stride;

	uint32 NumVertices;

	void AllocateData(bool bNeedCPUAccess = true);
};