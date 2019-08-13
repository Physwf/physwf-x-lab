#pragma once

#include "CoreTypes.h"

struct XStaticMeshSection
{
	int32 MaterialIndex;

	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;
};

struct XStaticMeshVertexBuffers
{
	XStaticMeshVertexBuffer StaticMeshVertexBuffer;
	XPositionVetexBuffer PositionVertexBuffer;
	XColorVertexBuffer ColorVertexBuffer;
};

struct XStaticMeshLODResources
{
	XStaticMeshVertexBuffers VertexBuffers;

	XRawStaticIndexBuffer IndexBuffer;

	XRawStaticIndexBuffer ReversedIndexBuffer;

	XRawStaticIndexBuffer DepthOnlyIndexBuffer;

	XRawStaticIndexBuffer ReversedDepthOnlyIndexBuffer;

	XRawStaticIndexBuffer WireframeIndexBuffer;

	XRawStaticIndexBuffer AdjacencyIndexBuffer;

	TArray<XStaticMeshSection> Sections;

	float MaxDeviation;

	uint32 bHasAdjacencyInfo:1;

	uint32 bHasDepthOnlyIndices : 1;

	uint32 bHasReversedIndices : 1;

	uint32 bHasReversedDepthOnlyIndices : 1;

	uint32 DepthOnlyNumTriangles;

	XStaticMeshLODResources();

	~XStaticMeshLODResources()();

	void InitResources();

	void ReleaseResources();

	int32 GetNumTriangles() const;
	int32 GetNumVertices() const;
	int32 GetNumTexcoords() const;
};

struct XStaticMeshVertexFactories
{

};

