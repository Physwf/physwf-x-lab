#pragma once

#include <vector>
#include <d3d11.h>

struct FVector
{
	float X, Y, Z;
};

struct FVector2
{
	float X, Y, Z;
};

struct Vertex
{
	FVector Postion;
	FVector Normal;
	FVector2 UV1;
	FVector2 UV2;
};

class Mesh
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
	std::vector<unsigned short> mIndices;

	ID3D11InputLayout* InputLayout = NULL;
	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;
	ID3D11VertexShader* VertexShader = NULL;
	ID3D11PixelShader* PixelShader = NULL;
};