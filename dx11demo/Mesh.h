#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Actor.h"
#include "Math.h"

struct Vertex
{
	Vector Postion;
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

	void InitResource();
	void ReleaseResource();

	void Draw();
private:
	int CreateVertex();
private:
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