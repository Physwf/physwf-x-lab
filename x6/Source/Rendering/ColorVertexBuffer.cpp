#include "Rendering/ColorVertexBuffer.h"

class XColorVertexData : public TStaticMeshVertexData<FColor>
{
	XColorVertexData(bool bNeedsCPUAccess=false)
		:TStaticMeshVertexData<FColor>(bNeedsCPUAccess)
	{

	}
};

XColorVertexBuffer::XColorVertexBuffer()
	:VertexData(NULL),
	Data(NULL),
	Stride(0),
	NumVertices(0)
{
}

XColorVertexBuffer::~XColorVertexBuffer()
{
	CleanUp();
}

void XColorVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		delete VertexData;
		VertexData = nullptr;
	}
}

void XColorVertexBuffer::Init(uint32 InNumVertices)
{
	NumVertices = InNumVertices;

	AllocateData();

	VertexData->ResizeBuffer(InNumVertices);
	Data = NumVertices ? VertexData->GetDataPointer() : nullptr;
}

void XColorVertexBuffer::AllocateData(bool bNeedCPUAccess /*= true*/)
{

}

