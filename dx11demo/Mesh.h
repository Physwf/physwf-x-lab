#pragma once

#include <vector>
#include <d3d11.h>
#include "Actor.h"
#include "Math.h"
#include "MeshDescription.h"
#include "fbxsdk.h"

struct StaticMeshBuildVertex
{
	Vector Position;

	Vector TangentX;
	float TangentYSign;
	Vector TangentZ;

	Color C;
	Vector2 UVs;
	//Vector2 LightMapCoordinate;
};

struct LocalVertex
{
	Vector4 Position;
	Vector  TangentX;
	Vector4 TangentZ;
	Vector4 Color;
	Vector2 TexCoords;
	Vector2	LightMapCoordinate;
};

struct PositionOnlyLocalVertex
{
	Vector4 Position;
};

struct StaticMeshSection
{
	int MaterialIndex;

	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;
};

struct MeshLODResources
{
	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* PositionOnlyVertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;

	std::vector<LocalVertex> Vertices;
	std::vector<PositionOnlyLocalVertex> PositionOnlyVertices;
	std::vector<uint32> Indices;

	std::vector<StaticMeshSection> Sections;

	void InitResource();
	void ReleaseResource();
};

struct MeshRenderState
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

struct PrimitiveUniform
{
	Matrix LocalToWorld;
	Matrix WorldToLocal;
	float PerObjectGBufferData;
	Vector4 InvNonUniformScale;
};

struct MeshElement
{
	ID3D11Buffer* PrimitiveUniformBuffer = nullptr;
	int MaterialIndex;

	uint32 FirstIndex;
	uint32 NumTriangles;
};

struct MeshBatch
{
	std::vector<MeshElement> Elements;

	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* PositionOnlyVertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;
};

struct MeshMaterial
{
	ID3D11Texture2D* BaseColor;
	ID3D11Texture2D* NormalMap;
	ID3D11Texture2D* MetalTexture;
	ID3D11Texture2D* RoughnessTexture;
	ID3D11Texture2D* SpecularTexture;
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

	int GetNumberBatches() { return 1; }
	bool GetMeshElement(int BatchIndex, int SectionIndex, MeshBatch& OutMeshBatch);
	void Draw(ID3D11DeviceContext* Context, const MeshRenderState& ShaderState);
	void DrawStaticElement();
private:
	void Build();
	void BuildVertexBuffer(const MeshDescription& MD2, std::vector<std::vector<uint32> >& OutPerSectionIndices, std::vector<StaticMeshBuildVertex>& StaticMeshBuildVertices);
	void GetRenderMeshDescription(const MeshDescription& InOriginalMeshDescription, MeshDescription& OutRenderMeshDescription);
private:
	MeshDescription MD;
	MeshLODResources LODResource;
	ID3D11InputLayout* InputLayout = NULL;
	ID3D11Buffer* PrimitiveUniformBuffer = NULL;
	std::vector<MeshMaterial> Materials;
};

static void RegisterMeshAttributes(MeshDescription& MD);