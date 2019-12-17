#pragma once

#include "fbxsdk.h"

#include <vector>
#include <memory>

#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Color.h"

#include "MeshDescription/MeshDescription.h"

class UStaticMeshDescriptions
{
public:
	// Define these in a concrete compilation unit so the TUniquePtr deleter works
	UStaticMeshDescriptions();
	~UStaticMeshDescriptions();

	/** Empties the mesh description container */
	void Empty();

	/** Returns the number of mesh descriptions in the container. This should always equal the number of LODs */
	int32 Num() const;

	/** Set the number of mesh descriptions in the container */
	void SetNum(const int32 Num);

	/** Gets mesh description for the given LOD */
	FMeshDescription* Get(int32 Index) const;

	/** Creates an empty mesh description for the given LOD */
	FMeshDescription* Create(int32 Index);

	/** Clears the mesh description for the given LOD */
	void Reset(int32 Index);

	/** Inserts new LODs at the given index */
	void InsertAt(int32 Index, int32 Count = 1);

	/** Deletes LODs at the given index */
	void RemoveAt(int32 Index, int32 Count = 1);

private:
	std::vector<std::unique_ptr<FMeshDescription>> MeshDescriptions;
};

class UStaticMesh
{
public:
	FMeshDescription* GetOriginalMeshDescription(int32 LodIndex);
private:
	UStaticMeshDescriptions * MeshDescriptions;
	FMeshDescription* MeshDescription;
};

class FFbxImporter
{
private:
	static void ReadAllMeshNodes(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray);
	static void ParseVertexPosition(FbxMesh* pMesh,int ControlPointIndex,int VertexCounter);
	static void ParseVertexColor(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter);
	static void ParseVertexNormal(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter);
	static void ParseVertexBinormal(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter);
	static void ParseVertexTangent(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter);
public:
	static void ImportStaticMeshAsSingle(const char* InFileName, UStaticMesh* OutMesh);
};

class FObjImporter
{
public:
	static void Import(const char* InFileName, UStaticMesh* OutMesh);
};
