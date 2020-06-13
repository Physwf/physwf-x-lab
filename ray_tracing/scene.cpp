#include "scene.h"
#include "camera.h"
#include "primitives.h"
#include <cstdio>

Scene* scene = nullptr;

void* ReadCSG(const char* desc, std::size_t& size)
{
	char startchar = desc[size];
	if (startchar == '{')
	{
		Comp* comp = new Comp();
		comp->flag = 1;
		size += 1;
		comp->left = (Comp*)ReadCSG(desc, size);

		size += 1;
		if ((desc[size] == '&' || desc[size] == '|' || desc[size] == '-'))
		{
			comp->op = desc[size];
			size += 1;
			comp->right = (Comp*)ReadCSG(desc, size);
		}
		size += 1;
		return comp;
	}
	else if (startchar == 'S')
	{
		Prim* prim = SphereRead(desc,size);
		return prim;
	}
	else if (startchar == 'B')
	{
		Prim* prim = BoxRead(desc, size);
		return prim;
	}
	return nullptr;
}

void Setup(Scene* s, Camera* c)
{
	FILE* csgfile;
	fopen_s(&csgfile,"scene.txt", "r");
	if (!csgfile)
	{
		return;
	}
	std::fseek(csgfile, 0L, SEEK_END);
	auto filesize = std::ftell(csgfile);
	std::fseek(csgfile, 0L, SEEK_SET);
	char* buffer = new char[filesize];
	std::fread(buffer, sizeof(buffer[0]), filesize, csgfile);
	std::size_t size = 0;
	s->modelroot = (Comp*)ReadCSG(buffer, size);
	delete buffer;
	std::fclose(csgfile);
	s->numlight = 1;
	s->lights = new Light[s->numlight];
	s->lights[0].position = { 0.0,-100.0,0.0};
	s->lights[0].color = { 1.0,0.50, 1.0 };
	s->lights[0].intensity = 30000.0;
	s->lights[0].attenuation = 10.0;

	c->position = {.0, .0, .0,};
	c->direction = Vec{ 0.0, 0.0, 1.0, };
	c->len = 300.0;
	c->screen = new Screen();
	c->screen->Width = 500;
	c->screen->Height = 500;
	c->screen->buffer = new unsigned int[500 * 500];
}

void Render(const Scene* s,const Camera* c)
{
	for (int y = 0; y < c->screen->Height; ++y)
	{
		for (int x = 0; x < c->screen->Height; ++x)
		{
			Ray r;
			//GetRayFromPixel(c, 338, 220, &r);
			//GetRayFromPixel(c, 347, 217, &r);
			GetRayFromPixel(c, x, y, &r);
			Color color = Trace(0, 1.0, &r);
			c->screen->buffer[y * 500 + x] = ToRGBA(color);
		}
	}
}

void Render(const Scene * s, const Camera * c, unsigned int x, unsigned int y)
{
	Ray r;
	GetRayFromPixel(c, x, y, &r);
	Color color = Trace(0, 1.0, &r);
	//c->screen->buffer[y * 500 + x] = ToRGBA(color);
}
