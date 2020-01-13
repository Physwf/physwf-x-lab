#pragma once

#include <vector>
#include "MeshElementArray.h"
#include "MeshTypes.h"
#include "MeshAttributeArray.h"

struct FMeshVertex
{
	FMeshVertex()
	{}

	/** All of vertex instances which reference this vertex (for split vertex support) */
	std::vector<FVertexInstanceID> VertexInstanceIDs;

	/** The edges connected to this vertex */
	std::set<FEdgeID> ConnectedEdgeIDs;


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
public:
	FVertexArray& Vertices() { return VertexArray; }
	const FVertexArray& Vertices() const { return VertexArray; }

	FMeshVertex& GetVertex(const FVertexID VertexID) { return VertexArray[VertexID]; }
	const FMeshVertex& GetVertex(const FVertexID VertexID) const { return VertexArray[VertexID]; }

	FVertexInstanceArray& VertexInstances() { return VertexInstanceArray; }
	const FVertexInstanceArray& VertexInstances() const { return VertexInstanceArray; }

	FMeshVertexInstance& GetVertexInstance(const FVertexInstanceID VertexInstanceID) { return VertexInstanceArray[VertexInstanceID]; }
	const FMeshVertexInstance& GetVertexInstance(const FVertexInstanceID VertexInstanceID) const { return VertexInstanceArray[VertexInstanceID]; }

	FEdgeArray& Edges() { return EdgeArray; }
	const FEdgeArray& Edges() const { return EdgeArray; }

	FMeshEdge& GetEdge(const FEdgeID EdgeID) { return EdgeArray[EdgeID]; }
	const FMeshEdge& GetEdge(const FEdgeID EdgeID) const { return EdgeArray[EdgeID]; }

	FPolygonArray& Polygons() { return PolygonArray; }
	const FPolygonArray& Polygons() const { return PolygonArray; }

	FMeshPolygon& GetPolygon(const FPolygonID PolygonID) { return PolygonArray[PolygonID]; }
	const FMeshPolygon& GetPolygon(const FPolygonID PolygonID) const { return PolygonArray[PolygonID]; }

	FPolygonGroupArray& PolygonGroups() { return PolygonGroupArray; }
	const FPolygonGroupArray& PolygonGroups() const { return PolygonGroupArray; }

	FMeshPolygonGroup& GetPolygonGroup(const FPolygonGroupID PolygonGroupID) { return PolygonGroupArray[PolygonGroupID]; }
	const FMeshPolygonGroup& GetPolygonGroup(const FPolygonGroupID PolygonGroupID) const { return PolygonGroupArray[PolygonGroupID]; }

	TAttributesSet<FVertexID>& VertexAttributes() { return VertexAttributesSet; }
	const TAttributesSet<FVertexID>& VertexAttributes() const { return VertexAttributesSet; }

	TAttributesSet<FVertexInstanceID>& VertexInstanceAttributes() { return VertexInstanceAttributesSet; }
	const TAttributesSet<FVertexInstanceID>& VertexInstanceAttributes() const { return VertexInstanceAttributesSet; }

	TAttributesSet<FEdgeID>& EdgeAttributes() { return EdgeAttributesSet; }
	const TAttributesSet<FEdgeID>& EdgeAttributes() const { return EdgeAttributesSet; }

	TAttributesSet<FPolygonID>& PolygonAttributes() { return PolygonAttributesSet; }
	const TAttributesSet<FPolygonID>& PolygonAttributes() const { return PolygonAttributesSet; }

	TAttributesSet<FPolygonGroupID>& PolygonGroupAttributes() { return PolygonGroupAttributesSet; }
	const TAttributesSet<FPolygonGroupID>& PolygonGroupAttributes() const { return PolygonGroupAttributesSet; }
private:
	void CreateVertex_Internal(const FVertexID VertexID)
	{
		VertexAttributesSet.Insert(VertexID);
	}
public:
	FVertexID CreateVertex()
	{
		const FVertexID VertexID = VertexArray.Add();
		CreateVertex_Internal(VertexID);
		return VertexID;
	}
private:
	void CreateVertexInstance_Internal(const FVertexInstanceID VertexInstanceID, const FVertexID VertexID)
	{
		VertexInstanceArray[VertexInstanceID].VertexID = VertexID;
		//check(!VertexArray[VertexID].VertexInstanceIDs.Contains(VertexInstanceID));
		VertexArray[VertexID].VertexInstanceIDs.push_back(VertexInstanceID);
		VertexInstanceAttributesSet.Insert(VertexInstanceID);
	}

public:
	/** Adds a new vertex instance to the mesh and returns its ID */
	FVertexInstanceID CreateVertexInstance(const FVertexID VertexID)
	{
		const FVertexInstanceID VertexInstanceID = VertexInstanceArray.Add();
		CreateVertexInstance_Internal(VertexInstanceID, VertexID);
		return VertexInstanceID;
	}
private:
	void CreateEdge_Internal(const FEdgeID EdgeID, const FVertexID VertexID0, const FVertexID VertexID1, const std::vector<FPolygonID>& ConnectedPolygons)
	{
		FMeshEdge& Edge = EdgeArray[EdgeID];
		Edge.VertexIDs[0] = VertexID0;
		Edge.VertexIDs[1] = VertexID1;
		Edge.ConnectedPolygons = ConnectedPolygons;
		VertexArray[VertexID0].ConnectedEdgeIDs.insert(EdgeID);
		VertexArray[VertexID1].ConnectedEdgeIDs.insert(EdgeID);
		EdgeAttributesSet.Insert(EdgeID);
	}
public:
	FEdgeID CreateEdge(const FVertexID VertexID0, const FVertexID VertexID1, const std::vector<FPolygonID>& ConnectedPolygons = std::vector<FPolygonID>())
	{
		const FEdgeID EdgeID = EdgeArray.Add();
		CreateEdge_Internal(EdgeID, VertexID0, VertexID1, ConnectedPolygons);
		return EdgeID;
	}

	struct FContourPoint
	{
		FVertexInstanceID VertexInstanceID;
		FEdgeID EdgeID;
	};
private:
	void CreatePolygonContour_Internal(const FPolygonID PolygonID, const std::vector<FContourPoint>& ContourPoints, FMeshPolygonContour& Contour)
	{
		Contour.VertexInstanceIDs.clear();
		for (const FContourPoint ContourPoint : ContourPoints)
		{
			const FVertexInstanceID VertexInstanceID = ContourPoint.VertexInstanceID;
			const FEdgeID EdgeID = ContourPoint.EdgeID;

			Contour.VertexInstanceIDs.push_back(VertexInstanceID);
			//check(!VertexInstanceArray[VertexInstanceID].ConnectedPolygons.Contains(PolygonID));
			VertexInstanceArray[VertexInstanceID].ConnectedPolygons.push_back(PolygonID);

			//check(!EdgeArray[EdgeID].ConnectedPolygons.Contains(PolygonID));
			EdgeArray[EdgeID].ConnectedPolygons.push_back(PolygonID);
		}
	}

	void CreatePolygon_Internal(const FPolygonID PolygonID, const FPolygonGroupID PolygonGroupID, const std::vector<FContourPoint>& Perimeter, const std::vector<std::vector<FContourPoint>>& Holes)
	{
		FMeshPolygon& Polygon = PolygonArray[PolygonID];
		CreatePolygonContour_Internal(PolygonID, Perimeter, Polygon.PerimeterContour);
		for (const std::vector<FContourPoint>& Hole : Holes)
		{
			Polygon.HoleContours.emplace(Polygon.HoleContours.end());
			CreatePolygonContour_Internal(PolygonID, Hole, Polygon.HoleContours.back());
		}

		Polygon.PolygonGroupID = PolygonGroupID;
		PolygonGroupArray[PolygonGroupID].Polygons.push_back(PolygonID);

		PolygonAttributesSet.Insert(PolygonID);
	}
public:
	/** Adds a new polygon to the mesh and returns its ID */
	FPolygonID CreatePolygon(const FPolygonGroupID PolygonGroupID, const std::vector<FContourPoint>& Perimeter, const std::vector<std::vector<FContourPoint>>& Holes = std::vector<std::vector<FContourPoint>>())
	{
		const FPolygonID PolygonID = PolygonArray.Add();
		CreatePolygon_Internal(PolygonID, PolygonGroupID, Perimeter, Holes);
		return PolygonID;
	}
private:
	void CreatePolygonGroup_Internal(const FPolygonGroupID PolygonGroupID)
	{
		PolygonGroupAttributesSet.Insert(PolygonGroupID);
	}
public:
	/** Adds a new polygon group to the mesh and returns its ID */
	FPolygonGroupID CreatePolygonGroup()
	{
		const FPolygonGroupID PolygonGroupID = PolygonGroupArray.Add();
		CreatePolygonGroup_Internal(PolygonGroupID);
		return PolygonGroupID;
	}

	const std::vector<FVertexInstanceID>& GetPolygonPerimeterVertexInstances(const FPolygonID PolygonID) const
	{
		return PolygonArray[PolygonID].PerimeterContour.VertexInstanceIDs;
	}
	const std::vector<FMeshTriangle>& GetPolygonTriangles(const FPolygonID PolygonID) const
	{
		return PolygonArray[PolygonID].Triangles;
	}
	/** Return the polygon group associated with a polygon */
	FPolygonGroupID GetPolygonPolygonGroup(const FPolygonID PolygonID) const
	{
		return PolygonArray[PolygonID].PolygonGroupID;
	}
	/** Returns the vertex ID associated with the given vertex instance */
	FVertexID GetVertexInstanceVertex(const FVertexInstanceID VertexInstanceID) const
	{
		return VertexInstanceArray[VertexInstanceID].VertexID;
	}

	FEdgeID GetVertexPairEdge(const FVertexID VertexID0, const FVertexID VertexID1) const
	{
		for (const FEdgeID VertexConnectedEdgeID : VertexArray[VertexID0].ConnectedEdgeIDs)
		{
			const FVertexID EdgeVertexID0 = EdgeArray[VertexConnectedEdgeID].VertexIDs[0];
			const FVertexID EdgeVertexID1 = EdgeArray[VertexConnectedEdgeID].VertexIDs[1];
			if ((EdgeVertexID0 == VertexID0 && EdgeVertexID1 == VertexID1) || (EdgeVertexID0 == VertexID1 && EdgeVertexID1 == VertexID0))
			{
				return VertexConnectedEdgeID;
			}
		}

		return FEdgeID::Invalid;
	}
	void ComputePolygonTriangulation(const FPolygonID PolygonID, std::vector<FMeshTriangle>& OutTriangles)
	{
		OutTriangles.clear();

		// @todo mesheditor holes: Does not support triangles with holes yet!
		// @todo mesheditor: Perhaps should always attempt to triangulate by splitting polygons along the shortest edge, for better determinism.

		//	const FMeshPolygon& Polygon = GetPolygon( PolygonID );
		const std::vector<FVertexInstanceID>& PolygonVertexInstanceIDs = GetPolygonPerimeterVertexInstances(PolygonID);

		// Polygon must have at least three vertices/edges
		const int PolygonVertexCount = (int32)PolygonVertexInstanceIDs.size();
		//check(PolygonVertexCount >= 3);

		// If perimeter has 3 vertices, just copy content of perimeter out 
		if (PolygonVertexCount == 3)
		{
			OutTriangles.emplace(OutTriangles.end());
			FMeshTriangle& Triangle = OutTriangles.back();

			Triangle.SetVertexInstanceID(0, PolygonVertexInstanceIDs[0]);
			Triangle.SetVertexInstanceID(1, PolygonVertexInstanceIDs[1]);
			Triangle.SetVertexInstanceID(2, PolygonVertexInstanceIDs[2]);

			return;
		}
	}
private:
	FVertexArray VertexArray;
	FVertexInstanceArray VertexInstanceArray;
	FEdgeArray EdgeArray;
	FPolygonArray PolygonArray;
	FPolygonGroupArray PolygonGroupArray;

	TAttributesSet<FVertexID> VertexAttributesSet;
	TAttributesSet<FVertexInstanceID> VertexInstanceAttributesSet;
	TAttributesSet<FEdgeID> EdgeAttributesSet;
	TAttributesSet<FPolygonID> PolygonAttributesSet;
	TAttributesSet<FPolygonGroupID> PolygonGroupAttributesSet;
};
