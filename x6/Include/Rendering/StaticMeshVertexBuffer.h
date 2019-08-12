#pragma once

#include "RenderResource.h"
#include "StaticMeshVertexData.h"

template<typename TangentTypeT>
struct TStaticMeshVertexTangentDatum
{
	TangentTypeT TangentX;
	TangentTypeT TangentZ;

	FVector GetTangentX() const
	{

	}

	FVector4 GetTangentZ() const
	{

	}

	FVector GetTangentY() const
	{

	}

	void SetTangents(FVector X, FVector Y, FVector Z)
	{
		TangentX = X;
	}

};

template <typename UVTypeT>
struct TStaticMeshVertexUVsDatum
{
	UVTypeT UVs;

	FVector2D GetUV()
	{
		return UVs;
	}

	void SetUV(FVector2D UV)
	{
		UVs = UV;
	}
};

class XStaticMeshVertexBuffer : public XRenderResource
{
public:

	XStaticMeshVertexBuffer();

	virtual ~XStaticMeshVertexBuffer();

	void CleanUp();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedCPUAcess = true);

	uint32 GetNumVertices() const
	{
		return NumVertices;
	}

	uint32 GetNumTexCoords() const
	{
		return NumTexCoords;
	}

	bool GetUseFullPrecisionUVs() const
	{
		return bUseFullPrecesionUVs;
	}

	void SetUseFullPrecisionUVs(bool UseFull)
	{
		bUseFullPrecesionUVs = UseFull;
	}

	bool GetUseHighPrecisionTangentBasis() const
	{
		return bUseHighPrecisionTangentBasis;
	}

	void SetUseHighPrecisionTangentBasis(bool bUseHighPrecision)
	{
		bUseHighPrecisionTangentBasis = bUseHighPrecision;
	}

	uint32 GetResourceSize() const
	{
		return (TangentsStride + (TexcoordStride * GetNumTexCoords())) * NumVertices;
	}

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
private:
	XStaticMeshVertexDataInterface* TangentsData;
	XShaderResourceViewRHIRef		TangentsSRV;

	XStaticMeshVertexDataInterface* TexcoordData;
	XShaderResourceViewRHIRef		TextureCoordinatesSRV;

	uint8* TangentsDataPtr;
	uint8* TexcoordDataPtr;

	uint32 TangentsStride;
	uint32 TexcoordStride;

	uint32 NumTexCoords;

	uint32 NumVertices;

	bool bUseFullPrecesionUVs;
	bool bUseHighPrecisionTangentBasis;

	void AllocateData(bool bNeedCPUAccess = true);

};