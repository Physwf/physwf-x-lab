#pragma once

#include "core.h"

struct Light
{
	Point position;
	Color color;
	Float intensity;
	Float attenuation;
};

struct Scene
{
	Comp*		modelroot;
	Light*		lights;
	int			numlight;
};
void* ReadCSG(const char* desc);
void Setup(Scene* s);
void Render(const Scene* s, const struct Camera* c);

extern Scene* scene;

