#pragma once

#include "Mesh.h"
#include "Light.h"
#include "Camera.h"

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

private:
	std::vector<Camera*> Cameras;
	std::vector<Mesh*> Meshes;
	Camera* CurrentCamera;

	LIGHT_CBUFFER LightConstBuffer;

	ID3D11Buffer* ConstantBuffer;
};