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
void* ReadCSG(const char* desc, std::size_t size);
void Setup(Scene* s, struct Camera* c);
void Render(const Scene* s, const struct Camera* c);

extern Scene* scene;

