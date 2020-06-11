#include "primitives.h"
#include <stdio.h>
#include <cctype>

PrimProcs SphereProcs = { SphereName, SpherePrint, SphereRead, SphereIntersect, SphereNormal };

void SpherePrint(const Prim* prim)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	printf("sphere : center(%g, % g, % g) radius %g\n", s->center[0], s->center[1], s->center[2], s->rad);
}

Float ReadValue(const char* desc,std::size_t& endpos)
{
	std::size_t startpos = 0;
	while (!std::isdigit(desc[startpos]) && desc[startpos] != '-')
	{
		++startpos;
	}
	endpos = startpos + 1;

	while (desc[endpos] != ',')
	{
		++endpos;
	}
	endpos++;//skip ','
	return std::atof(&desc[startpos]);
}

Prim* SphereRead(const char* desc, std::size_t size)
{
	Sphere* s = new Sphere();
	Surf* surf = new Surf();

	std::size_t startpos = 2;//skip S and (
	int index = 0;
	for (std::size_t i = startpos; i < size; )
	{
		std::size_t step = 1;
		switch (desc[i])
		{
		case 'x':
		case 'X':
		{
			s->center[0] = ReadValue(&desc[i], step);
		}
			break;
		case 'y':
		case 'Y':
		{
			s->center[1] = ReadValue(&desc[i], step);
		}
			break;
		case 'z':
		case 'Z':
		{
			s->center[2] = ReadValue(&desc[i], step);
		}
			break;
		case 'r':
		case 'R':
		{
			s->rad = ReadValue(&desc[i], step);
			s->rad2 = s->rad*s->rad;
		}
			break;
		case 'd':
		case 'D':
		{
			surf->kdiff = ReadValue(&desc[i], step);
		}
			break;
		case 's':
		case 'S':
		{
			surf->kspec = ReadValue(&desc[i], step);
		}
			break;
		case 't':
		case 'T':
		{
			surf->ktran = ReadValue(&desc[i], step);
		}
			break;
		case 'c':
		case 'C':
		{
			switch (desc[i+1])
			{
			case 'r':
			case 'R':
				surf->color[0] = ReadValue(&desc[i], step);
				break;
			case 'g':
			case 'G':
				surf->color[1] = ReadValue(&desc[i], step);
				break;
			case 'b':
			case 'B':
				surf->color[2] = ReadValue(&desc[i], step);
				break;
			}
		}
			break;
		case 'f':
		case 'F':
		{
			surf->refrindex = ReadValue(&desc[i], step);
		}
			break;
		}
		i += step;
	}

	Prim* Result = new Prim();
	Result->flag = 0;
	Result->procs = &SphereProcs;
	Result->info = s;
	Result->surf = surf;

	return Result;
}

int SphereIntersect(const Ray* ray, const Prim* prim, Isect* hit)
{
	int nroots;
	Float b, disc, t1, t2;
	Point V;
	Sphere* s;

	s = (Sphere*)prim->info;
	V = s->center - ray->P;
	b = Dot(V, ray->D);
	disc = b * b - Dot(V, V) + s->rad2;
	if (disc < .0) return 0;
	disc = sqrtl(disc);
	t2 = b + disc;
	if (t2 < rayeps) return 0;
	t1 = b - disc;
	// 	if (t1 > rayeps)
	// 	{
	// 		IsectAdd(hit, t1, prim, 1, prim->surf);
	// 		hit++;
	// 		nroots = 2;
	// 	}
	// 	else
	// 	{
	// 		nroots = 1;
	// 	}
	// 	IsectAdd(hit, t2, prim, 0, prim->surf);

	IsectAdd(hit, t1, prim, 1, prim->surf);
	hit++;
	IsectAdd(hit, t2, prim, 0, prim->surf);
	nroots = 2;

	return nroots;
}

void SphereNormal(const Prim* prim, const Point& P, Point& N)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	Subtract(P, s->center, N);
	Normalize(N, N);
}

