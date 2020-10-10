#pragma once

#include "Mesh.h"
#include "Light.h"
#include "Camera.h"


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

};