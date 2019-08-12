#include "StaticMeshVertexBuffer.h"

XStaticMeshVertexBuffer::XStaticMeshVertexBuffer()
	:TexcoordData(nullptr)
	,TexcoordData(nullptr),
	,TangentsDataPtr(nullptr)
	,TexcoordDataPtr(nullptr)
	,NumTexCoords(0)
	,NumVertices(0)
	,bUseFullPrecesionUVs(false)
	,bUseHighPrecisionTangentBasis(false)
{ }

XStaticMeshVertexBuffer::~XStaticMeshVertexBuffer()
{
	CleanUp();
}

void XStaticMeshVertexBuffer::CleanUp()
{
	if (TangentsData)
	{
		delete TangentsData;
		TangentsData = nullptr;
	}
	if (TexcoordData)
	{
		delete TexcoordData;
		TexcoordData = nullptr;
	}
}

void XStaticMeshVertexBuffer::Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedCPUAcess /*= true*/)
{
	NumTexCoords = InNumTexCoords;
	NumVertices = InNumVertices;

	AllocateData(bNeedCPUAcess);

	TangentsData->ResizeBuffer(NumVertices);
	TangentsDataPtr = NumVertices ? TangentsData->GetDataPointer() : nullptr;
	TexcoordData->ResizeBuffer(NumTexCoords);
	TexcoordDataPtr = NumTexCoords ? TexcoordData->GetDataPointer() : nullptr;
}

void XStaticMeshVertexBuffer::InitRHI()
{

}

void XStaticMeshVertexBuffer::ReleaseRHI()
{

}

void XStaticMeshVertexBuffer::InitResource()
{

}

void XStaticMeshVertexBuffer::ReleaseResource()
{

}

void XStaticMeshVertexBuffer::AllocateData(bool bNeedCPUAccess /*= true*/)
{
	CleanUp();

	uint32 VertexStride = 0;
	if (GetUseHighPrecisionTangentBasis())
	{

	}
	else
	{

	}

	if (GetUseFullPrecisionUVs())
	{

	}
	else
	{

	}
}
