#pragma once

#include "RenderResource.h"

struct FStaticMeshBuildVertex;

/** A vertex that stores just position. */
struct FPositionVertex
{
	FVector	Position;

};

/** A vertex buffer of positions. */
class FPositionVertexBuffer : public FVertexBuffer
{
public:

	/** Default constructor. */
	FPositionVertexBuffer();

	/** Destructor. */
	~FPositionVertexBuffer();

	/** Delete existing resources */
	void CleanUp();

	void Init(uint32 NumVertices, bool bNeedsCPUAccess = true);

	/**
	* Initializes the buffer with the given vertices, used to convert legacy layouts.
	* @param InVertices - The vertices to initialize the buffer with.
	*/
	void Init(const std::vector<FStaticMeshBuildVertex>& InVertices);

	/**
	* Initializes this vertex buffer with the contents of the given vertex buffer.
	* @param InVertexBuffer - The vertex buffer to initialize from.
	*/
	void Init(const FPositionVertexBuffer& InVertexBuffer);

	void Init(const std::vector<FVector>& InPositions);

	/**
	* Appends the specified vertices to the end of the buffer
	*
	* @param	Vertices	The vertex data to be appended.  Must not be nullptr.
	* @param	NumVerticesToAppend		How many vertices should be added
	*/
	void AppendVertices(const FStaticMeshBuildVertex* Vertices, const uint32 NumVerticesToAppend);

	/**
	* Specialized assignment operator, only used when importing LOD's.
	*/
	void operator=(const FPositionVertexBuffer &Other);

	// Vertex data accessors.
	inline FVector& VertexPosition(uint32 VertexIndex)
	{
		checkSlow(VertexIndex < GetNumVertices());
		return ((FPositionVertex*)(Data + VertexIndex * Stride))->Position;
	}
	inline const FVector& VertexPosition(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());
		return ((FPositionVertex*)(Data + VertexIndex * Stride))->Position;
	}
	// Other accessors.
	inline uint32 GetStride() const
	{
		return Stride;
	}
	inline uint32 GetNumVertices() const
	{
		return NumVertices;
	}

	// FRenderResource interface.
	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
	virtual std::string GetFriendlyName() const override { return ("PositionOnly Static-mesh vertices"); }

	//void BindPositionVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& Data) const;

	void* GetVertexData()
	{
		return Data;
	}

private:

	ID3D11ShaderResourceView* PositionComponentSRV;

	/** The vertex data storage type */
	class FPositionVertexData* VertexData;

	/** The cached vertex data pointer. */
	uint8* Data;

	/** The cached vertex stride. */
	uint32 Stride;

	/** The cached number of vertices. */
	uint32 NumVertices;

	/** Allocates the vertex data storage type. */
	void AllocateData(bool bNeedsCPUAccess = true);
};