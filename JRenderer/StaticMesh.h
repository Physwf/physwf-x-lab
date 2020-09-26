#pragma once

#include "RenderResources.h"
#include "MeshBatch.h"

#include <vector>

template <typename VertexDataType>
class TMeshVertexData
{
public:
	void ResizeBuffer(uint32 NumberVertiecs)
	{
		Data.resize(NumberVertiecs);
	}

	size_t GetStride() const 
	{
		return sizeof(VertexDataType);
	}

	uint8* GetDataPointer() const
	{
		return (uint8*)Data.data();
	}

	size_t GetResourceSize() const
	{
		return Data.size();
	}
private:
	std::vector<VertexDataType> Data;
};


struct PostionVertex
{
	FVector Postion;
};

class PostionVertexBuffer : public VertexBuffer
{
public:
	void Init(uint32 NumVertices);
	void Init(const std::vector<FVector>& InPostions);

	void CleanUp();

	virtual void InitRHI() override;
private:
	ID3D11ShaderResourceView* SRV;

	class PositionVertexData* VertexData;

	uint8* Data;

	uint32 Stride;
	uint32 NumberVertices;

	void AllocateData();
};

class RawIndexBuffer : public IndexBuffer
{
public:
	RawIndexBuffer();
	void SetIndices(const std::vector<uint32> InIndices);
	uint32 GetNumIndices() const;
	uint32 GetAllocatedSize() const;
	void Discard();
	virtual void InitRHI() override;
private:
	std::vector<uint32> IndexData;
};


struct MeshDataType
{
	VertexStreamComponent PositionComponent;
	VertexStreamComponent TangentBasisComponents[2];
	std::vector<VertexStreamComponent> TextureCoordinates;

	//FVertexStreamComponent LightMapCoordinateComponent;
	//FVertexStreamComponent ColorComponent;

	ID3D11ShaderResourceView* PostionComponentSRV;
	ID3D11ShaderResourceView* TangentsSRV;
	ID3D11ShaderResourceView* TextureCoordinatesSRV;
	//ID3D11ShaderResourceView* ColorComponentsSRV;

	//int LightMapCoordinateIndex = -1;
	int NumTexCoords = -1;
	uint32 ColorIndexMask = ~0u;
};

class LocalVertexFactory : public VertexFactory
{
public:
	virtual void InitRHI() override;
	virtual void ReleaseRHI() override
	{
		VertexFactory::ReleaseRHI();
	}
	void SetData(const MeshDataType& InData);
private:
	MeshDataType Data;
};

struct StaticMeshSection
{
	int32 MaterialIndex;

	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;

};

class MeshRenderData
{
public:
	PostionVertexBuffer PostionVB;
	RawIndexBuffer IB;
	LocalVertexFactory VF;
	std::vector<StaticMeshSection> Sections;

	bool GetMeshElement(int32 BatchIndex, int32 SectionIndex, MeshBatch& OutMeshBatch);
	void SetIndexSource(int32 ElementIndex, FMeshBatch& OutMeshElement, bool bWireframe, bool bRequiresAdjacencyInformation, bool bUseInversedIndices, bool bAllowPreCulledIndices) const;
};

class StaticMesh : public MeshBatch
{
public:
	StaticMesh(const FMeshBatch& InMesh) :MeshBatch(InMesh)
	{

	}
};