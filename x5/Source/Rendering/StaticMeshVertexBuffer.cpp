#include "Rendering/StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"

#include <memory>

FStaticMeshVertexBuffer::FStaticMeshVertexBuffer():
	TangentsData(nullptr),
	TexcoordData(nullptr),
	TangentsDataPtr(nullptr),
	TexcoordDataPtr(nullptr),
	NumTexCoords(0),
	NumVertices(0),
	bUseFullPrecisionUVs(true/*!GVertexElementTypeSupport.IsSupported(VET_Half2)*/),
	bUseHighPrecisionTangentBasis(false)
{

}

FStaticMeshVertexBuffer::~FStaticMeshVertexBuffer()
{
	CleanUp();
}

void FStaticMeshVertexBuffer::CleanUp()
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

void FStaticMeshVertexBuffer::Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedsCPUAccess /*= true*/)
{
	NumTexCoords = InNumTexCoords;
	NumVertices = InNumVertices;

	// Allocate the vertex data storage type.
	AllocateData(bNeedsCPUAccess);

	// Allocate the vertex data buffer.
	TangentsData->ResizeBuffer(NumVertices);
	TangentsDataPtr = NumVertices ? TangentsData->GetDataPointer() : nullptr;
	TexcoordData->ResizeBuffer(NumVertices * GetNumTexCoords());
	TexcoordDataPtr = NumVertices ? TexcoordData->GetDataPointer() : nullptr;
}

void FStaticMeshVertexBuffer::Init(const std::vector<FStaticMeshBuildVertex>& InVertices, uint32 InNumTexCoords)
{
	Init(InVertices.size(), InNumTexCoords);

	// Copy the vertices into the buffer.
	for (int32 VertexIndex = 0; VertexIndex < (int32)InVertices.size(); VertexIndex++)
	{
		const FStaticMeshBuildVertex& SourceVertex = InVertices[VertexIndex];
		const uint32 DestVertexIndex = VertexIndex;
		SetVertexTangents(DestVertexIndex, SourceVertex.TangentX, SourceVertex.TangentY, SourceVertex.TangentZ);

		for (uint32 UVIndex = 0; UVIndex < NumTexCoords; UVIndex++)
		{
			SetVertexUV(DestVertexIndex, UVIndex, SourceVertex.UVs[UVIndex]);
		}
	}
}

void FStaticMeshVertexBuffer::Init(const FStaticMeshVertexBuffer& InVertexBuffer)
{
	NumTexCoords = InVertexBuffer.GetNumTexCoords();
	NumVertices = InVertexBuffer.GetNumVertices();
	bUseFullPrecisionUVs = InVertexBuffer.GetUseFullPrecisionUVs();
	bUseHighPrecisionTangentBasis = InVertexBuffer.GetUseHighPrecisionTangentBasis();

	if (NumVertices)
	{
		AllocateData();
		{
			check(TangentsData->GetStride() == InVertexBuffer.TangentsData->GetStride());
			TangentsData->ResizeBuffer(NumVertices);
			TangentsDataPtr = TangentsData->GetDataPointer();
			const uint8* InData = InVertexBuffer.TangentsDataPtr;
			memcpy(TangentsDataPtr, InData, TangentsData->GetStride() * NumVertices);
		}
		{
			check(TexcoordData->GetStride() == InVertexBuffer.TexcoordData->GetStride());
			check(GetNumTexCoords() == InVertexBuffer.GetNumTexCoords());
			const uint8* InData = InVertexBuffer.TexcoordDataPtr;

			// convert half float data to full float if the HW requires it.
			if (!GetUseFullPrecisionUVs()/* && !GVertexElementTypeSupport.IsSupported(VET_Half2)*/)
			{
				ConvertHalfTexcoordsToFloat(InData);
			}
			else
			{
				TexcoordData->ResizeBuffer(NumVertices * GetNumTexCoords());
				TexcoordDataPtr = TexcoordData->GetDataPointer();
				memcpy(TexcoordDataPtr, InData, TexcoordData->GetStride() * NumVertices * GetNumTexCoords());
			}
		}
	}
}

void FStaticMeshVertexBuffer::AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend)
{
	if ((TangentsData == nullptr || TexcoordData == nullptr) && NumVerticesToAppend > 0)
	{
		check(NumVertices == 0);
		NumTexCoords = 1;

		// Allocate the vertex data storage type if it has never been allocated before
		AllocateData();
	}

	if (NumVerticesToAppend > 0)
	{
		check(Vertices != nullptr);

		const uint32 FirstDestVertexIndex = NumVertices;
		NumVertices += NumVerticesToAppend;

		TangentsData->ResizeBuffer(NumVertices);
		TexcoordData->ResizeBuffer(NumVertices * GetNumTexCoords());

		if (NumVertices > 0)
		{
			TangentsDataPtr = TangentsData->GetDataPointer();
			TexcoordDataPtr = TexcoordData->GetDataPointer();

			// Copy the vertices into the buffer.
			for (uint32 VertexIter = 0; VertexIter < NumVerticesToAppend; ++VertexIter)
			{
				const FStaticMeshBuildVertex& SourceVertex = Vertices[VertexIter];

				const uint32 DestVertexIndex = FirstDestVertexIndex + VertexIter;

				SetVertexTangents(DestVertexIndex, SourceVertex.TangentX, SourceVertex.TangentY, SourceVertex.TangentZ);
				for (uint32 UVIndex = 0; UVIndex < NumTexCoords; UVIndex++)
				{
					SetVertexUV(DestVertexIndex, UVIndex, SourceVertex.UVs[UVIndex]);
				}
			}
		}
	}
}

void FStaticMeshVertexBuffer::operator=(const FStaticMeshVertexBuffer &Other)
{
	CleanUp();
	bUseFullPrecisionUVs = Other.bUseFullPrecisionUVs;
	bUseHighPrecisionTangentBasis = Other.bUseHighPrecisionTangentBasis;
}

void FStaticMeshVertexBuffer::InitRHI()
{
	check(TangentsData);
	FResourceArrayInterface* ResourceArray = TangentsData->GetResourceArray();
	if (ResourceArray->GetResourceDataSize())
	{
		//create vertex buffer
		TangentsVertexBuffer.VertexBufferRHI;
	}
}

void FStaticMeshVertexBuffer::ReleaseRHI()
{
	SafeRelase(TangentsSRV);
	SafeRelase(TextureCoordinatesSRV);
	TangentsVertexBuffer.ReleaseRHI();
	TexCoordVertexBuffer.ReleaseRHI();
}

void FStaticMeshVertexBuffer::InitResource()
{
	FRenderResource::InitResource();
	TangentsVertexBuffer.InitResource();
	TexCoordVertexBuffer.InitResource();
}

void FStaticMeshVertexBuffer::ReleaseResource()
{
	FRenderResource::ReleaseResource();
	TangentsVertexBuffer.ReleaseResource();
	TexCoordVertexBuffer.ReleaseResource();
}

void FStaticMeshVertexBuffer::BindTangentVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const
{
	Data.TangentsSRV = TangentsSRV;

	uint32 TangentSizeInBytes = 0;
	uint32 TangentXOffset = 0;
	uint32 TangentZOffset = 0;
	EVertexElementType TangentElemType = VET_None;
	if (GetUseHighPrecisionTangentBasis())
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::HighPrecision>::TangentTypeT> TangentType;
		TangentElemType = TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::HighPrecision>::VertexElementType;
		TangentXOffset = offsetof(TangentType, TangentX);
		TangentZOffset = offsetof(TangentType, TangentZ);
		TangentSizeInBytes = sizeof(TangentType);
	}
	else
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::Default>::TangentTypeT> TangentType;
		TangentElemType = TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::Default>::VertexElementType;
		TangentXOffset = offsetof(TangentType, TangentX);
		TangentZOffset = offsetof(TangentType, TangentZ);
		TangentSizeInBytes = sizeof(TangentType);
	}

	Data.TangentBasisComponent[0] = FVertexStreamComponent(
		&TangentsVertexBuffer,
		TangentXOffset,
		TangentSizeInBytes,
		TangentElemType,
		EVertexStreamUsage::ManualFetch
	);

	Data.TangentBasisComponent[1] = FVertexStreamComponent(
		&TangentsVertexBuffer,
		TangentZOffset,
		TangentSizeInBytes,
		TangentElemType,
		EVertexStreamUsage::ManualFetch
	);
}

void FStaticMeshVertexBuffer::BindTexCoordVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data, int ClampedNumTexCoords /*= -1*/) const
{

}

void FStaticMeshVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{
	// Clear any old VertexData before allocating.
	CleanUp();

	uint32 VertexStride = 0;
	if (GetUseHighPrecisionTangentBasis())
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::HighPrecision>::TangentTypeT> TangentType;
		TangentsStride = sizeof(TangentType);
		TangentsData = new TStaticMeshVertexData<TangentType>(bNeedsCPUAccess);
	}
	else
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::Default>::TangentTypeT> TangentType;
		TangentsStride = sizeof(TangentType);
		TangentsData = new TStaticMeshVertexData<TangentType>(bNeedsCPUAccess);
	}

	if (GetUseFullPrecisionUVs())
	{
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>::UVsTypeT> UVType;
		TexcoordStride = sizeof(UVType);
		TexcoordData = new TStaticMeshVertexData<UVType>(bNeedsCPUAccess);
	}
	else
	{
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::Default>::UVsTypeT> UVType;
		TexcoordStride = sizeof(UVType);
		TexcoordData = new TStaticMeshVertexData<UVType>(bNeedsCPUAccess);
	}
}

void FStaticMeshVertexBuffer::ConvertHalfTexcoordsToFloat(const uint8* InData)
{
	check(TexcoordData);
	SetUseFullPrecisionUVs(true);

	FStaticMeshVertexDataInterface* OriginalTexcoordData = TexcoordData;

	typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>::UVsTypeT> UVType;
	TexcoordData = new TStaticMeshVertexData<UVType>(OriginalTexcoordData->GetAllowCPUAccess());
	TexcoordData->ResizeBuffer(NumVertices * GetNumTexCoords());
	TexcoordDataPtr = TexcoordData->GetDataPointer();
	TexcoordStride = sizeof(UVType);

	FVector2D* DestTexcoordDataPtr = (FVector2D*)TexcoordDataPtr;
	FVector2DHalf* SourceTexcoordDataPtr = (FVector2DHalf*)(InData ? InData : OriginalTexcoordData->GetDataPointer());
	for (uint32 i = 0; i < NumVertices * GetNumTexCoords(); i++)
	{
		*DestTexcoordDataPtr++ = *SourceTexcoordDataPtr++;
	}

	delete OriginalTexcoordData;
	OriginalTexcoordData = nullptr;
}
