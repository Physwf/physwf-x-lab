#include "Rendering/PositionVertexBuffer.h"
#include "StaticMeshVertexData.h"

class FPositionVertexData : public TStaticMeshVertexData<FPositionVertex>
{
public:
	FPositionVertexData(bool bNeedCPUAccess = false):
		TStaticMeshVertexData<FPositionVertex>(bNeedCPUAccess)
	{
	}
};

FPositionVertexBuffer::FPositionVertexBuffer():
	VertexData(nullptr),
	Data(nullptr),
	Stride(0),
	NumVertices(0)
{

}

FPositionVertexBuffer::~FPositionVertexBuffer()
{
	CleanUp();
}

void FPositionVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		delete VertexData;
		VertexData = nullptr;
	}
}

void FPositionVertexBuffer::Init(uint32 InNumVertices, bool bNeedsCPUAccess /*= true*/)
{
	NumVertices = InNumVertices;

	AllocateData(bNeedsCPUAccess);

	VertexData->ResizeBuffer(NumVertices);
	Data = NumVertices ? VertexData->GetDataPointer() : nullptr;
}

void FPositionVertexBuffer::Init(const std::vector<FStaticMeshBuildVertex>& InVertices)
{
	Init(InVertices.size());

	for (uint32 VertexIndex = 0; VertexIndex < InVertices.size(); VertexIndex++)
	{
		const FStaticMeshBuildVertex& SourceVertex = InVertices[VertexIndex];
		const uint32 DestVertexIndex = VertexIndex;
		VertexPosition(DestVertexIndex) = SourceVertex.Position;
	}
}

void FPositionVertexBuffer::Init(const FPositionVertexBuffer& InVertexBuffer)
{
	if (InVertexBuffer.GetNumVertices())
	{
		Init(InVertexBuffer.GetNumVertices());

		check(Stride == InVertexBuffer.GetStride());

		const uint8* InData = InVertexBuffer.Data;
		memcpy(Data, InData, Stride * NumVertices);
	}
}

void FPositionVertexBuffer::Init(const std::vector<FVector>& InPositions)
{
	NumVertices = InPositions.size();
	if (NumVertices)
	{
		AllocateData();
		check(Stride == sizeof(std::vector<FVector>::size_type));
		VertexData->ResizeBuffer(NumVertices);
		Data = VertexData->GetDataPointer();
		memcpy(Data, InPositions.data(), Stride * NumVertices);
	}
}

void FPositionVertexBuffer::AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend)
{
	if (VertexData == nullptr && NumVerticesToAppend > 0)
	{
		// Allocate the vertex data storage type if the buffer was never allocated before
		AllocateData();
	}

	if (NumVerticesToAppend > 0)
	{
		check(VertexData != nullptr);
		check(Vertices != nullptr);

		const uint32 FirstDestVertexIndex = NumVertices;
		NumVertices += NumVerticesToAppend;
		VertexData->ResizeBuffer(NumVertices);
		if (NumVertices > 0)
		{
			Data = VertexData->GetDataPointer();

			// Copy the vertices into the buffer.
			for (uint32 VertexIter = 0; VertexIter < NumVerticesToAppend; ++VertexIter)
			{
				const FStaticMeshBuildVertex& SourceVertex = Vertices[VertexIter];

				const uint32 DestVertexIndex = FirstDestVertexIndex + VertexIter;
				VertexPosition(DestVertexIndex) = SourceVertex.Position;
			}
		}
	}
}

void FPositionVertexBuffer::operator=(const FPositionVertexBuffer &Other)
{
	VertexData = NULL;
}

void FPositionVertexBuffer::InitRHI()
{
	check(VertexData);
	FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
	if (ResourceArray->GetResourceDataSize())
	{

	}
}

void FPositionVertexBuffer::ReleaseRHI()
{
	SafeRelase(PositionComponentSRV);

	FVertexBuffer::ReleaseRHI();
}

void FPositionVertexBuffer::BindPositionVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const
{
	Data.PositionComponent = FVertexStreamComponent(
		this,
		offsetof(FPositionVertex,Position),
		GetStride(),
		VET_Float3
	);

}

void FPositionVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{
	CleanUp();

	VertexData = new FPositionVertexData(bNeedsCPUAccess);

	Stride = VertexData->GetStride();
}

