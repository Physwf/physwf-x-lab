#pragma once

#include <vector>
#include <d3d11.h>
#include "Actor.h"
#include "Math.h"
#include "MeshDescription.h"
#include "fbxsdk.h"

struct Vertex
{
	Vector Position;
	Vector Normal;
	Vector2 UV1;
	Vector2 UV2;
};

struct StaticMeshSection
{
	int MaterialIndex;

	unsigned int FirstIndex;
	unsigned int NumTriangles;
	unsigned int MinVertexIndex;
	unsigned int MaxVertexIndex;
};

struct MeshLODResources
{
	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	std::vector<StaticMeshSection> Sections;

	void InitResource();
	void ReleaseResource();
};

struct MeshShaderState
{
	ID3D11InputLayout* InputLayout = NULL;
	ID3D11VertexShader* VertexShader = NULL;
	ID3D11PixelShader* PixelShader = NULL;
};

struct FbxMaterial
{
	FbxSurfaceMaterial* fbxMaterial;

	std::string GetName() const { return fbxMaterial ? fbxMaterial->GetName() : "None"; }
};

class Mesh : public Actor
{
public:
	Mesh() {}
	~Mesh() {}

	void ImportFromFBX(const char* pFileName);
	void GeneratePlane(float InWidth, float InHeight,int InNumSectionW, int InNumSectionH);
	void GnerateBox(float InSizeX, float InSizeY, float InSizeZ, int InNumSectionX, int InNumSectionY, int InNumSectionZ);

	void InitResource();
	void ReleaseResource();

	void Draw();
private:
	void Build();
private:
	MeshDescription MD;
	MeshLODResources LODResource;
	MeshShaderState ShaderState;
	ID3D11Buffer* ConstantBuffer = NULL;
	
};

static void RegisterMeshAttributes(MeshDescription& MD);