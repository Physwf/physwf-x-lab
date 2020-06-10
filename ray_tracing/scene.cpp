#include "scene.h"
#include "camera.h"
#include "primitives.h"
#include <cstdio>

Scene* scene = nullptr;

void* ReadCSG(const char* desc, std::size_t size)
{
	char startchar = desc[0];
	if (startchar == '{')
	{
		Comp* comp = new Comp();
		std::size_t startpos = 0;
		int numbraces = 0;
		for (std::size_t i = startpos + 1; i < size; ++i)
		{
			if (desc[i] == '{') numbraces++;
			else if(desc[i] == '}') numbraces--;
			else if (numbraces == 0 && (desc[i] == '&' || desc[i] == '|' || desc[i] == '-'))
			{
				comp->flag = 1;
				comp->op = desc[i];
				comp->left = (Comp*)ReadCSG(desc+1,i-1);
				comp->right = (Comp*)ReadCSG(desc + i + 1, size - i - 1);
			}
		}
		return comp;
	}
	else if (startchar == 'S')
	{
		Prim* prim = SphereRead(desc,size);
		return prim;
	}
	return nullptr;
}

void Setup(Scene* s)
{
	FILE* csgfile = std::fopen("scene.txt", "r");
	if (!csgfile)
	{
		return;
	}
	auto filesize = std::ftell(csgfile);
	char* buffer = new char[filesize];
	std::fread(buffer, sizeof(buffer[0]), filesize, csgfile);
	s->modelroot = (Comp*)ReadCSG(buffer, filesize);
	delete buffer;
	s->numlight = 1;
	s->lights = new Light[s->numlight];
}

void Render(const Scene* s,const Camera* c)
{

}
