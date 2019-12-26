#pragma once

#include <vector>
#include <d3d11.h>
#include "Actor.h"
#include "Math.h"

struct Vertex
{
	Vector Position;
	Vector Normal;
	Vector2 UV1;
	Vector2 UV2;
};

struct XStaticMeshSection
{
	int MaterialIndex;

	unsigned int FirstIndex;
	unsigned int NumTriangles;
	unsigned int MinVertexIndex;
	unsigned int MaxVertexIndex;
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
	int CreateVertex();
private:
	std::vector<Vector> mPoistions;
	std::vector<unsigned int> mVertexInstances;
	std::vector<Vector> mVertexInstanceNormals;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	
	std::vector<XStaticMeshSection> Sections;

	ID3D11InputLayout* InputLayout = NULL;
	ID3D11Buffer* ConstantBuffer = NULL;
	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;
	ID3D11VertexShader* VertexShader = NULL;
	ID3D11PixelShader* PixelShader = NULL;
};