#pragma once

#include <string>
#include <vector>

#include "CoreTypes.h"
#include "Math/Vector2DHalf.h"
#include "RenderCore/RenderResource.h"
#include "RenderCore/PackedNormal.h"
#include "Components.h"
#include "RHI/RHIDefinitions.h"
#include "Rendering/StaticMeshVertexDataInterface.h"
#include "Misc/AssertionMacros.h"
#include "RenderCore/RenderUtils.h"

class FStaticMeshVertexDataInterface;

template<typename TangentTypeT>
struct TStaticMeshVertexTangentDatum
{
	TangentTypeT TangentX;
	TangentTypeT TangentZ;

	inline FVector GetTangentX() const
	{
		return TangentX.ToFVector();
	}

	inline FVector4 GetTangentZ() const
	{
		return TangentZ.ToFVector4();
	}

	inline FVector GetTangentY() const
	{
		FVector  TanX = GetTangentX();
		FVector4 TanZ = GetTangentZ();

		return (FVector(TanZ) ^ TanX) * TanZ.W;
	}

	inline void SetTangents(FVector X, FVector Y, FVector Z)
	{
		TangentX = X;
		TangentZ = FVector4(Z, GetBasisDeterminantSign(X, Y, Z));
	}

};

template<typename UVTypeT>
struct TStaticMeshVertexUVsDatum
{
	UVTypeT UVs;

	inline FVector2D GetUV() const
	{
		return UVs;
	}

	inline void SetUV(FVector2D UV)
	{
		UVs = UV;
	}
};

enum class EStaticMeshVertexTangentBasisType
{
	Default,
	HighPrecision,
};

enum class EStaticMeshVertexUVType
{
	Default,
	HighPrecision,
};

template<EStaticMeshVertexTangentBasisType TangentBasisType>
struct TStaticMeshVertexTangentTypeSelector
{
};

template<>
struct TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::Default>
{
	typedef FPackedNormal TangentTypeT;
	static const EVertexElementType VertexElementType = VET_PackedNormal;
};

template<>
struct TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::HighPrecision>
{
	typedef FPackedRGBA16N TangentTypeT;
	static const EVertexElementType VertexElementType = VET_Short4N;
};

template<EStaticMeshVertexUVType UVType>
struct TStaticMeshVertexUVsTypeSelector
{
};

template<>
struct TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::Default>
{
	typedef FVector2DHalf UVsTypeT;
};

template<>
struct TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>
{
	typedef FVector2D UVsTypeT;
};


/** Vertex buffer for a static mesh LOD */
class FStaticMeshVertexBuffer : public FRenderResource
{
public:

	/** Default constructor. */
	FStaticMeshVertexBuffer();

	/** Destructor. */
	~FStaticMeshVertexBuffer();

	/** Delete existing resources */
	void CleanUp();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedsCPUAccess = true);

	/**
	* Initializes the buffer with the given vertices.
	* @param InVertices - The vertices to initialize the buffer with.
	* @param InNumTexCoords - The number of texture coordinate to store in the buffer.
	*/
	void Init(const std::vector<FStaticMeshBuildVertex>& InVertices, uint32 InNumTexCoords);

	/**
	* Initializes this vertex buffer with the contents of the given vertex buffer.
	* @param InVertexBuffer - The vertex buffer to initialize from.
	*/
	void Init(const FStaticMeshVertexBuffer& InVertexBuffer);

	/**
	* Appends the specified vertices to the end of the buffer
	*
	* @param	Vertices	The vertex data to be appended.  Must not be nullptr.
	* @param	NumVerticesToAppend		How many vertices should be added
	*/
	void AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend);

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	inline FVector4 VertexTangentX_Typed(uint32 VertexIndex) const
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT> TangentType;
		TangentType* ElementData = reinterpret_cast<TangentType*>(TangentsDataPtr);
		check((void*)((&ElementData[VertexIndex]) + 1) <= (void*)(TangentsDataPtr + TangentsData->GetResourceSize()));
		check((void*)((&ElementData[VertexIndex]) + 0) >= (void*)(TangentsDataPtr));
		return ElementData[VertexIndex].GetTangentX();
	}

	FVector4 VertexTangentX(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());

		if (GetUseHighPrecisionTangentBasis())
		{
			return VertexTangentX_Typed<EStaticMeshVertexTangentBasisType::HighPrecision>(VertexIndex);
		}
		else
		{
			return VertexTangentX_Typed<EStaticMeshVertexTangentBasisType::Default>(VertexIndex);
		}
	}

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	inline FVector4 VertexTangentZ_Typed(uint32 VertexIndex) const
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT> TangentType;
		TangentType* ElementData = reinterpret_cast<TangentType*>(TangentsDataPtr);
		check((void*)((&ElementData[VertexIndex]) + 1) <= (void*)(TangentsDataPtr + TangentsData->GetResourceSize()));
		check((void*)((&ElementData[VertexIndex]) + 0) >= (void*)(TangentsDataPtr));
		return ElementData[VertexIndex].GetTangentZ();
	}

	inline FVector VertexTangentZ(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());

		if (GetUseHighPrecisionTangentBasis())
		{
			return VertexTangentZ_Typed<EStaticMeshVertexTangentBasisType::HighPrecision>(VertexIndex);
		}
		else
		{
			return VertexTangentZ_Typed<EStaticMeshVertexTangentBasisType::Default>(VertexIndex);
		}
	}

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	inline FVector4 VertexTangentY_Typed(uint32 VertexIndex)const
	{
		typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT> TangentType;
		TangentType* ElementData = reinterpret_cast<TangentType*>(TangentsDataPtr);
		check((void*)((&ElementData[VertexIndex]) + 1) <= (void*)(TangentsDataPtr + TangentsData->GetResourceSize()));
		check((void*)((&ElementData[VertexIndex]) + 0) >= (void*)(TangentsDataPtr));
		return ElementData[VertexIndex].GetTangentY();
	}

	inline FVector VertexTangentY(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());

		if (GetUseHighPrecisionTangentBasis())
		{
			return VertexTangentY_Typed<EStaticMeshVertexTangentBasisType::HighPrecision>(VertexIndex);
		}
		else
		{
			return VertexTangentY_Typed<EStaticMeshVertexTangentBasisType::Default>(VertexIndex);
		}
	}

	inline void SetVertexTangents(uint32 VertexIndex, FVector X, FVector Y, FVector Z)
	{
		checkSlow(VertexIndex < GetNumVertices());

		if (GetUseHighPrecisionTangentBasis())
		{
			typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::HighPrecision>::TangentTypeT> TangentType;
			TangentType* ElementData = reinterpret_cast<TangentType*>(TangentsDataPtr);
			check((void*)((&ElementData[VertexIndex]) + 1) <= (void*)(TangentsDataPtr + TangentsData->GetResourceSize()));
			check((void*)((&ElementData[VertexIndex]) + 0) >= (void*)(TangentsDataPtr));
			ElementData[VertexIndex].SetTangents(X, Y, Z);
		}
		else
		{
			typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<EStaticMeshVertexTangentBasisType::Default>::TangentTypeT> TangentType;
			TangentType* ElementData = reinterpret_cast<TangentType*>(TangentsDataPtr);
			check((void*)((&ElementData[VertexIndex]) + 1) <= (void*)(TangentsDataPtr + TangentsData->GetResourceSize()));
			check((void*)((&ElementData[VertexIndex]) + 0) >= (void*)(TangentsDataPtr));
			ElementData[VertexIndex].SetTangents(X, Y, Z);
		}
	}

	inline void SetVertexUV(uint32 VertexIndex, uint32 UVIndex, const FVector2D& Vec2D)
	{
		checkSlow(VertexIndex < GetNumVertices());
		checkSlow(UVIndex < GetNumTexCoords());

		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>::UVsTypeT> UVType;
		size_t UvStride = sizeof(UVType) * GetNumTexCoords();

		UVType* ElementData = reinterpret_cast<UVType*>(TexcoordDataPtr + (VertexIndex * UvStride));
		check((void*)((&ElementData[UVIndex]) + 1) <= (void*)(TexcoordDataPtr + TexcoordData->GetResourceSize()));
		check((void*)((&ElementData[UVIndex]) + 0) >= (void*)(TexcoordDataPtr));
		ElementData[UVIndex].SetUV(Vec2D);
	}

	template<EStaticMeshVertexUVType UVTypeT>
	inline FVector2D GetVertexUV_Typed(uint32 VertexIndex, uint32 UVIndex) const
	{
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<UVTypeT>::UVsTypeT> UVType;
		size_t UvStride = sizeof(UVType) * GetNumTexCoords();

		UVType* ElementData = reinterpret_cast<UVType*>(TexcoordDataPtr + (VertexIndex * UvStride));
		check((void*)((&ElementData[UVIndex]) + 1) <= (void*)(TexcoordDataPtr + TexcoordData->GetResourceSize()));
		check((void*)((&ElementData[UVIndex]) + 0) >= (void*)(TexcoordDataPtr));
		return ElementData[UVIndex].GetUV();
	}

	inline FVector2D GetVertexUV(uint32 VertexIndex, uint32 UVIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());
		checkSlow(UVIndex < GetNumTexCoords());

		if (GetUseFullPrecisionUVs())
		{
			return GetVertexUV_Typed<EStaticMeshVertexUVType::HighPrecision>(VertexIndex, UVIndex);
		}
		else
		{
			return GetVertexUV_Typed<EStaticMeshVertexUVType::Default>(VertexIndex, UVIndex);
		}
	}

	void operator=(const FStaticMeshVertexBuffer &Other);

	inline uint32 GetNumVertices() const
	{
		return NumVertices;
	}

	inline uint32 GetNumTexCoords() const
	{
		return NumTexCoords;
	}

	inline bool GetUseFullPrecisionUVs() const
	{
		return bUseFullPrecisionUVs;
	}

	inline void SetUseFullPrecisionUVs(bool UseFull)
	{
		bUseFullPrecisionUVs = UseFull;
	}

	inline bool GetUseHighPrecisionTangentBasis() const
	{
		return bUseHighPrecisionTangentBasis;
	}

	inline void SetUseHighPrecisionTangentBasis(bool bUseHighPrecision)
	{
		bUseHighPrecisionTangentBasis = bUseHighPrecision;
	}

	inline uint32 GetResourceSize() const
	{
		return (TangentsStride + (TexcoordStride * GetNumTexCoords())) * NumVertices;
	}

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
	virtual void InitResource() override;
	virtual void ReleaseResource() override;
	virtual std::string GetFriendlyName() const override { return ("Static-mesh vertices"); }

	void BindTangentVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const;
	void BindTexCoordVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data, int ClampedNumTexCoords = -1) const;
	//void BindPackedTexCoordVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const;
	//void BindLightMapVertexBuffer(const FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const;

	inline void* GetTangentData()
	{
		return TangentsDataPtr;
	}

	inline void* GetTexCoordData()
	{
		return TexcoordDataPtr;
	}

	class FTangentsVertexBuffer : public FVertexBuffer
	{
		virtual std::string GetFriendlyName() const override { return ("FTangentsVertexBuffer"); }
	} TangentsVertexBuffer;

	class FTexcoordVertexBuffer : public FVertexBuffer
	{
		virtual std::string GetFriendlyName() const override { return ("FTexcoordVertexBuffer"); }
	} TexCoordVertexBuffer;
private:
	/** The vertex data storage type */
	FStaticMeshVertexDataInterface * TangentsData;
	ID3D11ShaderResourceView* TangentsSRV;

	FStaticMeshVertexDataInterface* TexcoordData;
	ID3D11ShaderResourceView* TextureCoordinatesSRV;

	/** The cached vertex data pointer. */
	uint8* TangentsDataPtr;
	uint8* TexcoordDataPtr;

	/** The cached Tangent stride. */
	uint32 TangentsStride;

	/** The cached Texcoord stride. */
	uint32 TexcoordStride;

	/** The number of texcoords/vertex in the buffer. */
	uint32 NumTexCoords;

	/** The cached number of vertices. */
	uint32 NumVertices;

	/** Corresponds to UStaticMesh::UseFullPrecisionUVs. if true then 32 bit UVs are used */
	bool bUseFullPrecisionUVs;

	/** If true then RGB10A2 is used to store tangent else RGBA8 */
	bool bUseHighPrecisionTangentBasis;

	/** Allocates the vertex data storage type. */
	void AllocateData(bool bNeedsCPUAccess = true);

	void ConvertHalfTexcoordsToFloat(const uint8* InData);
};