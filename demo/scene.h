#pragma once
#include "gl.h"

class Scene
{
public:
	Scene() {}
	~Scene() {};

	void Init();
	void Draw();
private:
	class Mesh* LowPolyCat;
	GLuint LowPolyCatProgram;
};