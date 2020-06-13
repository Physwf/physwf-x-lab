#include "scene.h"
#include "camera.h"
#include "primitives.h"
#include <cstdio>

Scene* scene = nullptr;

void* ReadCSG(const char* desc, std::size_t& size)
{
	char startchar = desc[0];
	if (startchar == '{')
	{
		Comp* comp = new Comp();
		comp->flag = 1;
		comp->left = (Comp*)ReadCSG(&desc[1], size);

		int i = size + 1;
		int numbraces = 0;
		while (desc[i++] != '}')
		{
			if (desc[i] == '(') numbraces++;
			else if(desc[i] == ')') numbraces--;
			if ((desc[i] == '&' || desc[i] == '|' || desc[i] == '-'))
			{
				comp->op = desc[i];
				comp->right = (Comp*)ReadCSG(desc + i + 1, size);
				return comp;
			}
		}
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
	std::size_t size;
	s->modelroot = (Comp*)ReadCSG(buffer, size);
	delete buffer;
	std::fclose(csgfile);
	s->numlight = 1;
	s->lights = new Light[s->numlight];
	s->lights[0].position = { 0.0,0.0,150.0};
	s->lights[0].color = { 1.0,0.50, 1.0 };
	s->lights[0].intensity = 10000.0;
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
