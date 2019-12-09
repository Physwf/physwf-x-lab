#pragma once

#include <vector>
#include "gl.h"

class Texture2D
{
public:
	Texture2D() { name = 0; }
	~Texture2D() {}

	bool LoadFromJpg(const char* fileName);

	void UpdateRHI();
private:
	std::vector<unsigned char> RawData;
	int width;
	int height;
	int nrChannels;

	GLuint name;
};