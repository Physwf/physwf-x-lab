#pragma once

#include "Mesh.h"
#include "Light.h"

struct LIGHT_CBUFFER
{
	AmbientLight AmLight;
	DirectionalLight DirLight;
	PointLight PointLights[8];
};

class Scene
{
public:
	void InitResource();
	void ReleaseResource();
	void Setup();
	void Draw();
private:
	std::vector<Mesh*> Meshes;

	LIGHT_CBUFFER LightConstBuffer;

	ID3D11Buffer* ConstantBuffer;
};