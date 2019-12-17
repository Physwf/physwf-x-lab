#pragma once

#include <vector>
#include "MeshElementArray.h"
#include "MeshTypes.h"

struct FMeshVertex
{
	FMeshVertex()
	{}

	/** All of vertex instances which reference this vertex (for split vertex support) */
	std::vector<FVertexInstanceID> VertexInstanceIDs;

	/** The edges connected to this vertex */
	std::vector<FEdgeID> ConnectedEdgeIDs;


};


struct FMeshVertexInstance
{
	FMeshVertexInstance()
		: VertexID(FVertexID::Invalid)
	{}

	/** The vertex this is instancing */
	FVertexID VertexID;

	/** List of connected polygons */
	std::vector<FPolygonID> ConnectedPolygons;
};


struct FMeshEdge
{
	FMeshEdge()
	{
		VertexIDs[0] = FVertexID::Invalid;
		VertexIDs[1] = FVertexID::Invalid;
	}

	/** IDs of the two editable mesh vertices that make up this edge.  The winding direction is not defined. */
	FVertexID VertexIDs[2];

	/** The polygons that share this edge.  It's best if there are always only two polygons that share
		the edge, and those polygons are facing the same direction */
	std::vector<FPolygonID> ConnectedPolygons;
};

struct FMeshPolygonContour
{
	/** The ordered list of vertex instances which make up the polygon contour. The winding direction is counter-clockwise. */
	std::vector<FVertexInstanceID> VertexInstanceIDs;

};

struct FMeshTriangle
{

	FMeshTriangle()
		:VertexInstanceID0(FVertexInstanceID::Invalid),
		VertexInstanceID1(FVertexInstanceID::Invalid),
		VertexInstanceID2(FVertexInstanceID::Invalid)
	{}

	/** First vertex instance that makes up this triangle.  Indices must be ordered counter-clockwise. */
	FVertexInstanceID VertexInstanceID0;

	/** Second vertex instance that makes up this triangle.  Indices must be ordered counter-clockwise. */
	FVertexInstanceID VertexInstanceID1;

	/** Third vertex instance that makes up this triangle.  Indices must be ordered counter-clockwise. */
	FVertexInstanceID VertexInstanceID2;

	/** Gets the specified triangle vertex instance ID.  Pass an index between 0 and 2 inclusive. */
	inline FVertexInstanceID GetVertexInstanceID(const int32 Index) const
	{
		//checkSlow(Index >= 0 && Index <= 2);
		return reinterpret_cast<const FVertexInstanceID*>(this)[Index];
	}

	/** Sets the specified triangle vertex instance ID.  Pass an index between 0 and 2 inclusive, and the new vertex instance ID to store. */
	inline void SetVertexInstanceID(const int32 Index, const FVertexInstanceID NewVertexInstanceID)
	{
		//checkSlow(Index >= 0 && Index <= 2);
		(reinterpret_cast<FVertexInstanceID*>(this)[Index]) = NewVertexInstanceID;
	}

};

struct FMeshPolygon
{
	FMeshPolygon()
		: PolygonGroupID(FPolygonGroupID::Invalid)
	{}

	/** The outer boundary edges of this polygon */
	FMeshPolygonContour PerimeterContour;

	/** Optional inner contours of this polygon that define holes inside of the polygon.  For the geometry to
		be considered valid, the hole contours should reside within the boundary of the polygon perimeter contour,
		and must not overlap each other.  No "nesting" of polygons inside the holes is supported -- those are
		simply separate polygons */
	std::vector<FMeshPolygonContour> HoleContours;

	/** List of triangles which make up this polygon */
	std::vector<FMeshTriangle> Triangles;

	/** The polygon group which contains this polygon */
	FPolygonGroupID PolygonGroupID;

};


struct FMeshPolygonGroup
{
	FMeshPolygonGroup()
	{}

	/** All polygons in this group */
	std::vector<FPolygonID> Polygons;
};

using FVertexArray = TMeshElementArray<FMeshVertex, FVertexID>;
using FVertexInstanceArray = TMeshElementArray<FMeshVertexInstance, FVertexInstanceID>;
using FEdgeArray = TMeshElementArray<FMeshEdge, FEdgeID>;
using FPolygonArray = TMeshElementArray<FMeshPolygon, FPolygonID>;
using FPolygonGroupArray = TMeshElementArray<FMeshPolygonGroup, FPolygonGroupID>;

class FMeshDescription
{
private:
	FVertexArray VertexArray;
	FVertexInstanceArray VertexInstanceArray;
	FEdgeArray EdgeArray;
	FPolygonArray PolygonArray;
	FPolygonGroupArray PolygonGroupArray;
};
