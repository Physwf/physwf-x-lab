#pragma once

#include "RenderResource.h"

#include <vector>
#include <string>

struct FStaticMeshBuildVertex;


/**
* A vertex buffer of colors.
*/
class FColorVertexBuffer : public FVertexBuffer
{
public:
	enum class NullBindStride
	{
		FColorSizeForComponentOverride, //when we want to bind null buffer with the expectation that it must be overridden.  Stride must be correct so override binds correctly
		ZeroForDefaultBufferBind, //when we want to bind the null color buffer for it to actually be used for draw.  Stride must be 0 so the IA gives correct data for all verts.
	};

	/** Default constructor. */
	FColorVertexBuffer();

	/** Destructor. */
	~FColorVertexBuffer();

	/** Delete existing resources */
	void CleanUp();

	void Init(uint32 InNumVertices);

	/**
	* Initializes the buffer with the given vertices, used to convert legacy layouts.
	* @param InVertices - The vertices to initialize the buffer with.
	*/
	void Init(const TArray<FStaticMeshBuildVertex>& InVertices);

	/**
	* Initializes this vertex buffer with the contents of the given vertex buffer.
	* @param InVertexBuffer - The vertex buffer to initialize from.
	*/
	void Init(const FColorVertexBuffer& InVertexBuffer);

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
	void operator=(const FColorVertexBuffer &Other);

	inline FColor& VertexColor(uint32 VertexIndex)
	{
		checkSlow(VertexIndex < GetNumVertices());
		return *(FColor*)(Data + VertexIndex * Stride);
	}

	inline const FColor& VertexColor(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());
		return *(FColor*)(Data + VertexIndex * Stride);
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

	/** Useful for memory profiling. */
	uint32 GetAllocatedSize() const;

	/**
	* Gets all vertex colors in the buffer
	*
	* @param OutColors	The populated list of colors
	*/
	void GetVertexColors(std::vector<FColor>& OutColors) const;

	/**
	* Load from a array of colors
	* @param InColors - must not be 0
	* @param Count - must be > 0
	* @param Stride - in bytes, usually sizeof(FColor) but can be 0 to use a single input color or larger.
	*/
	void InitFromColorArray(const FColor *InColors, uint32 Count, uint32 Stride = sizeof(FColor));

	/**
	* Load from raw color array.
	* @param InColors - InColors must not be empty
	*/
	void InitFromColorArray(const TArray<FColor> &InColors)
	{
		InitFromColorArray(InColors.GetData(), InColors.Num());
	}

	/**
	* Load from single color.
	* @param Count - must be > 0
	*/
	void InitFromSingleColor(const FColor &InColor, uint32 Count)
	{
		InitFromColorArray(&InColor, Count, 0);
	}

	// FRenderResource interface.
	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;
	virtual std::string GetFriendlyName() const override { return ("ColorOnly Mesh Vertices"); }

	//void BindColorVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& StaticMeshData) const;
	//static void BindDefaultColorVertexBuffer(const class FVertexFactory* VertexFactory, struct FStaticMeshDataType& StaticMeshData, NullBindStride BindStride);

	inline const ID3D11ShaderResourceView* GetColorComponentsSRV() const
	{
		return ColorComponentsSRV;
	}

	void* GetVertexData()
	{
		return Data;
	}

private:

	/** The vertex data storage type */
	class FColorVertexData* VertexData;

	ID3D11ShaderResourceView* ColorComponentsSRV;

	/** The cached vertex data pointer. */
	uint8* Data;

	/** The cached vertex stride. */
	uint32 Stride;

	/** The cached number of vertices. */
	uint32 NumVertices;

	/** Allocates the vertex data storage type. */
	void AllocateData(bool bNeedsCPUAccess = true);

	/** Purposely hidden */
	FColorVertexBuffer(const FColorVertexBuffer &rhs);
};