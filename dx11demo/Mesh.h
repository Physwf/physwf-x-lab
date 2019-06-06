#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Actor.h"

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

	ID3D11InputLayout* InputLayout = NULL;
	ID3D11Buffer* ConstantBuffer = NULL;
	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;
	ID3D11VertexShader* VertexShader = NULL;
	ID3D11PixelShader* PixelShader = NULL;
};