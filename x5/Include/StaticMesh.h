#pragma once

#include "fbxsdk.h"

#include <vector>

#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Color.h"

#include "MeshDescription/MeshDescription.h"

class UStaticMesh
{

private:
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
	static void Import(const char* InFileName, UStaticMesh* OutMesh);
};

class FObjImporter
{
public:
	static void Import(const char* InFileName, UStaticMesh* OutMesh);
};
