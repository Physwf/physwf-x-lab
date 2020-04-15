#include "Rendering/ColorVertexBuffer.h"
#include "StaticMeshVertexData.h"

class FColorVertexData :
	public TStaticMeshVertexData<FColor>
{
public:
	FColorVertexData(bool InNeedsCPUAccess = false)
		: TStaticMeshVertexData<FColor>(InNeedsCPUAccess)
	{
	}
};


FColorVertexBuffer::FColorVertexBuffer():
	VertexData(NULL),
	Data(NULL),
	Stride(0),
	NumVertices(0)
{

}

FColorVertexBuffer::FColorVertexBuffer(const FColorVertexBuffer &rhs) :
	VertexData(NULL),
	Data(NULL),
	Stride(0),
	NumVertices(0)
{
	if (rhs.VertexData)
	{
		InitFromColorArray(reinterpret_cast<FColor*>(rhs.VertexData->GetDataPointer()), rhs.VertexData->Num());
	}
	else
	{
		CleanUp();
	}
}

FColorVertexBuffer::~FColorVertexBuffer()
{
	CleanUp();
}

void FColorVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		delete VertexData;
		VertexData = NULL;
	}
}

void FColorVertexBuffer::Init(uint32 InNumVertices)
{
	NumVertices = InNumVertices;

	AllocateData();

	VertexData->ResizeBuffer(NumVertices);
	Data = InNumVertices ? VertexData->GetDataPointer() : nullptr;
}

void FColorVertexBuffer::Init(const std::vector<FStaticMeshBuildVertex>& InVertices)
{
	const uint32 InVertexCount = InVertices.size();
	bool bAllColorsAreOpaqueWhite = true;
	bool bAllColorsAreEqual = true;

	if (InVertexCount > 0)
	{
		const FColor FirstColor = InVertices[0].Color;

		for (uint32 CurVertexIndex = 0; CurVertexIndex < InVertexCount; ++CurVertexIndex)
		{
			const FColor CurColor = InVertices[CurVertexIndex].Color;

			if (CurColor.R != 255 || CurColor.G != 255 || CurColor.B != 255 || CurColor.A != 255)
			{
				bAllColorsAreOpaqueWhite = false;
			}

			if (CurColor.R != FirstColor.R || CurColor.G != FirstColor.G || CurColor.B != FirstColor.B || CurColor.A != FirstColor.A)
			{
				bAllColorsAreEqual = false;
			}

			if (!bAllColorsAreEqual && !bAllColorsAreOpaqueWhite)
			{
				break;
			}
		}
	}

	if (bAllColorsAreOpaqueWhite)
	{
		// Ensure no vertex data is allocated.
		CleanUp();

		// Clear the vertex count and stride.
		Stride = 0;
		NumVertices = 0;
	}
	else
	{
		Init(InVertexCount);

		// Copy the vertices into the buffer.
		for (uint32 VertexIndex = 0; VertexIndex < InVertices.size(); VertexIndex++)
		{
			const FStaticMeshBuildVertex& SourceVertex = InVertices[VertexIndex];
			const uint32 DestVertexIndex = VertexIndex;
			VertexColor(DestVertexIndex) = SourceVertex.Color;
		}
	}
}

void FColorVertexBuffer::Init(const FColorVertexBuffer& InVertexBuffer)
{
	if (NumVertices)
	{
		Init(InVertexBuffer.GetNumVertices());
		const uint8* InData = InVertexBuffer.Data;
		memcpy(Data, InData, Stride * NumVertices);
	}
}

void FColorVertexBuffer::AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend)
{
	if (VertexData == nullptr && NumVerticesToAppend > 0)
	{
		check(NumVertices == 0);

		// Allocate the vertex data storage type if the buffer was never allocated before
		AllocateData();
	}

	if (NumVerticesToAppend > 0)
	{
		// @todo: check if all opaque white, and if so append nothing

		check(VertexData != nullptr);	// Must only be called after Init() has already initialized the buffer!
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
				VertexColor(DestVertexIndex) = SourceVertex.Color;
			}
		}
	}
}

void FColorVertexBuffer::operator=(const FColorVertexBuffer &Other)
{
	VertexData = NULL;
}

void FColorVertexBuffer::GetVertexColors(std::vector<FColor>& OutColors) const
{
	if (VertexData != NULL && NumVertices > 0)
	{
		OutColors.resize(NumVertices);

		memcpy(OutColors.data(), VertexData->GetDataPointer(), NumVertices * VertexData->GetStride());
	}
}

void FColorVertexBuffer::InitFromColorArray(const FColor *InColors, uint32 Count, uint32 InStride /*= sizeof(FColor)*/)
{
	check(Count > 0);

	NumVertices = Count;

	// Allocate the vertex data storage type.
	AllocateData();

	// Copy the colors
	{
		VertexData->ResizeBuffer(Count);

		const uint8 *Src = (const uint8 *)InColors;
		FColor *Dst = (FColor *)VertexData->GetDataPointer();

		for (uint32 i = 0; i < Count; ++i)
		{
			*Dst++ = *(const FColor*)Src;

			Src += InStride;
		}
	}

	// Make a copy of the vertex data pointer.
	Data = VertexData->GetDataPointer();
}

uint32 FColorVertexBuffer::GetAllocatedSize() const
{
	if (VertexData)
	{
		return VertexData->GetResourceSize();
	}
	else
	{
		return 0;
	}
}



void FColorVertexBuffer::InitRHI()
{
	if (VertexData != NULL)
	{
		FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
		if (ResourceArray->GetResourceDataSize())
		{
			
		}
	}
}

void FColorVertexBuffer::ReleaseRHI()
{
	SafeRelase(ColorComponentsSRV);

	FVertexBuffer::ReleaseRHI();
}

void FColorVertexBuffer::BindColorVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& StaticMeshData) const
{
	if (GetNumVertices() == 0)
	{
		BindDefaultColorVertexBuffer(VertexFactory, StaticMeshData, NullBindStride::ZeroForDefaultBufferBind);
		return;
	}

	StaticMeshData.ColorComponentSRV = ColorComponentsSRV;
	StaticMeshData.ColorIndexMask = ~0u;

	{
		StaticMeshData.ColorComponent = FVertexStreamComponent(
			this,
			0,
			GetStride(),
			VET_Color,
			EVertexStreamUsage::ManualFetch
		);
	}
}

void FColorVertexBuffer::BindDefaultColorVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& StaticMeshData, NullBindStride BindStride)
{
	StaticMeshData.ColorComponentSRV = GNullColorVertexBuffer.VertexBufferSRV;
	StaticMeshData.ColorIndexMask = 0;

	{
		const bool bBindForDrawOverride = BindStride == NullBindStride::FColorSizeForComponentOverride;

		StaticMeshData.ColorComponent = FVertexStreamComponent(
			&GNullColorVertexBuffer,
			0,
			bBindForDrawOverride ? sizeof(FColor) : 0,
			VET_Color,
			bBindForDrawOverride ? (EVertexStreamUsage::ManualFetch | EVertexStreamUsage::Overridden) : (EVertexStreamUsage::ManualFetch));
	}
}

void FColorVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{
	CleanUp();

	VertexData = new FColorVertexData(bNeedsCPUAccess);
	Stride = VertexData->GetStride();
}
