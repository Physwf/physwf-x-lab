#include "primitives.h"
#include <stdio.h>

PrimProcs SphereProcs = { SphereName, SpherePrint, SphereRead, SphereIntersect, SphereNormal };

void SpherePrint(const Prim* prim)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	printf("sphere : center(%g, % g, % g) radius %g\n", s->center[0], s->center[1], s->center[2], s->rad);
}

Prim* SphereRead(const char* desc, std::size_t size)
{
	Sphere* s = new Sphere();
	Surf* surf = new Surf();

	std::size_t startpos = 1;
	int index = 0;
	for (std::size_t i = startpos; i < size; ++i)
	{
		if (desc[i] == ',')
		{
			if (index < 3)
			{
				s->center[index] = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			else if (index == 3)
			{
				s->rad = std::atof(&desc[startpos]);
				s->rad2 = s->rad*s->rad;
				startpos = i + 1;
			}
			else if (index == 4)//diffuse
			{
				surf->kdiff = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			else if (index == 5)//specular
			{
				surf->kspec = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			else if (index == 6)//transition
			{
				surf->ktran = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			else if (index < 10)//color
			{
				surf->color[index-7] = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			else if (index == 10)
			{
				surf->refrindex = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
			index++;
		}
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
	Subtract(s->center, ray->P, V);
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

void SphereNormal(const Prim* prim, const Point P, Point N)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	Subtract(P, s->center, N);
	Normalize(N, N);
}

