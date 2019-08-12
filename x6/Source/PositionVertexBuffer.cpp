#include "Rendering/PositionVertexBuffer.h"

#include "CoreMinimal.h"
#include "RHI.h"

#include "StaticMeshVertexData.h"

class XPositionVertexData :
	public TStaticMeshVertexData<XPositionVertex>
{
public:
	XPositionVertexData(bool InNeedsCPUAccess = false):TStaticMeshVertexData<XPositionVertex>(InNeedsCPUAccess)
	{

	}
};


XPositionVetexBuffer::XPositionVetexBuffer():
	VertexData,
	Data(NULL),
	Stride(0),
	NumVertices(0)
{

}

XPositionVetexBuffer::~XPositionVetexBuffer()
{
	CleanUp();
}

void XPositionVetexBuffer::CleanUp()
{
	if (VertexData)
	{
		delete VertexData;
		VertexData = nullptr;
	}
}

void XPositionVetexBuffer::Init(int32 InNumVertices, bool bNeedCPUAcess /*= true*/)
{
	NumVertices = InNumVertices;

	AllocateData(bNeedCPUAcess);

	VertexData->ResizeBuffer(NumVertices);
	Data = NumVertices ? VertexData->GetDataPointer() : nullptr;
}

void XPositionVetexBuffer::Init(const TArray<FVector>& InPositions)
{

}

void XPositionVetexBuffer::InitRHI()
{

}

void XPositionVetexBuffer::ReleaseRHI()
{

}

void XPositionVetexBuffer::AllocateData(bool bNeedCPUAccess /*= true*/)
{
	CleanUp();

	VertexData = new XPositionVertexData(bNeedCPUAccess);

	Stride = VertexData->GetStride();
}
