#pragma once

#include "CoreTypes.h"

struct FElementID	// @todo mesheditor script: BP doesn't have name spaces, so we might need a more specific display name, or just rename our various types
{
	FElementID()
	{
	}

	explicit FElementID(const int32 InitIDValue)
		: IDValue(InitIDValue)
	{
	}

	inline int32 GetValue() const
	{
		return IDValue;
	}

	inline bool operator==(const FElementID& Other) const
	{
		return IDValue == Other.IDValue;
	}

	inline bool operator!=(const FElementID& Other) const
	{
		return IDValue != Other.IDValue;
	}

	static const FElementID Invalid;

protected:
	/** The actual mesh element index this ID represents.  Read-only. */
	int32 IDValue;
};

struct FVertexID : public FElementID
{

	FVertexID()
	{
	}

	explicit FVertexID(const FElementID InitElementID)
		: FElementID(InitElementID.GetValue())
	{
	}

	explicit FVertexID(const int32 InitIDValue)
		: FElementID(InitIDValue)
	{
	}

	/** Invalid vertex ID */
	static const FVertexID Invalid;
};

struct FVertexInstanceID : public FElementID
{

	FVertexInstanceID()
	{
	}

	explicit FVertexInstanceID(const FElementID InitElementID)
		: FElementID(InitElementID.GetValue())
	{
	}

	explicit FVertexInstanceID(const uint32 InitIDValue)
		: FElementID(InitIDValue)
	{
	}

	/** Invalid rendering vertex ID */
	static const FVertexInstanceID Invalid;
};


struct FEdgeID : public FElementID
{

	FEdgeID()
	{
	}

	explicit FEdgeID(const FElementID InitElementID)
		: FElementID(InitElementID.GetValue())
	{
	}

	explicit FEdgeID(const int32 InitIDValue)
		: FElementID(InitIDValue)
	{
	}

	/** Invalid edge ID */
	static const FEdgeID Invalid;
};


struct FPolygonGroupID : public FElementID
{

	FPolygonGroupID()
	{
	}

	explicit FPolygonGroupID(const FElementID InitElementID)
		: FElementID(InitElementID.GetValue())
	{
	}

	explicit FPolygonGroupID(const int32 InitIDValue)
		: FElementID(InitIDValue)
	{
	}

	/** Invalid section ID */
	static const FPolygonGroupID Invalid;
};


struct FPolygonID : public FElementID
{

	FPolygonID()
	{
	}

	explicit FPolygonID(const FElementID InitElementID)
		: FElementID(InitElementID.GetValue())
	{
	}

	explicit FPolygonID(const int32 InitIDValue)
		: FElementID(InitIDValue)
	{
	}

	/** Invalid polygon ID */
	static const FPolygonID Invalid;	// @todo mesheditor script: Can we expose these to BP nicely?	Do we even need to?
};

