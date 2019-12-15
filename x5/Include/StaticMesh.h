#pragma once

#include "fbxsdk.h"

#include <vector>

class UStaticMesh
{

};

class FFbxImporter
{
public:
	static void ReadAllMeshNodes(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray);

	static void Import(const char* InFileName, UStaticMesh* OutMesh);

};

class FObjImporter
{
public:
	static void Import(const char* InFileName, UStaticMesh* OutMesh);
};
