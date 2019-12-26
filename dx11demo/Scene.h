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
	void Draw();

	void OnKeyDown(unsigned int KeyCode);
	void OnKeyUp(unsigned int KeyCode);
	void OnMouseDown(int X,int Y);
	void OnMouseUp(int X, int Y);
	void OnRightMouseDown(int X, int Y);
	void OnRightMouseUp(int X, int Y);
	void OnMouseMove(int X, int Y);
private:
	std::vector<Camera*> Cameras;
	std::vector<Mesh*> Meshes;
	Camera* CurrentCamera;

	LIGHT_CBUFFER LightConstBuffer;

	ID3D11Buffer* ConstantBuffer;
};