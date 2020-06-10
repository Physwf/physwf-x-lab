#include "core.h"
#include "scene.h"

#include <assert.h>
#include <algorithm>

int maxlevel = 5;
Float minweight = .01;

void IsectAdd(Isect* hit, Float t, const Prim* prim, int enter, const Surf* medium)
{
	Isect i;
	i.t = t;
	i.prim = prim;
	i.enter = enter;
	i.medium = medium;
}

Float rayeps = 1e-7;

#define ISECTMAX 10

void Trace(int level, Float Weight, Ray* ray, Color color)
{
	const Prim* prim;
	Point P, N;
	Isect hit[ISECTMAX];

	if (Intersect(ray, scene->modelroot, hit))
	{
		prim = hit[0].prim;
		RayPoint(ray, hit[0].t, P);//计算相交点
		(*prim->procs->normal)(hit[0].prim, P, N);//计算法线
		if (Dot(ray->D, N) > 0.0)
		{
			Negate(N, N);
		}

		Shade(level, Weight, P, N, ray->D, hit, color);
	}
	else
	{
		color[0]  = color[1]= color[2] = 0.0;
	}
}

int Intersect(Ray* ray, Comp* solid, Isect* hit)
{
	int nl, nr;
	Isect lhit[ISECTMAX], rhit[ISECTMAX];

	if (solid->flag)//composite solid
	{
		nl = Intersect(ray, solid->left, lhit);
		if (nl == 0 && solid->op != '|')
		{
			return 0;
		}
		else
		{
			nr = Intersect(ray, solid->right, rhit);

			return IntersectMerge(solid->op, nl, lhit, nr, rhit, hit);
		}
	}
	else
	{
		return (*((Prim*)solid)->procs->intersect)(ray, (Prim*)solid,hit);
	}
}

int IntersectMerge(int op, int nl, Isect* lhits, int nr, Isect* rhits, Isect* hit)
{
	//make sure we have even number of intersection, when the intersection is tangent, duplicate it in the results.
	assert(nl % 2 == 0);
	assert(nr % 2 == 0);
	int numhit = 0;

	//先计算一方在另一方内部的相交次数
	int numlinternals = 0;
	int numrinternals = 0;
	Isect* linternals[ISECTMAX];
	Isect* rinternals[ISECTMAX];
	for (int i = 0; i < nl; i += 2)
	{
		Isect* lhit1 = lhits + i;
		Isect* lhit2 = lhits + i + 1;
		//make sure the hit list is sort properly
		assert(lhit1->t < lhit2->t);
		for (int j = 0; j < nr; ++j)
		{
			Isect* rhit = rhits + j;
			if ((rhit->t >= lhit1->t && rhit->t <= lhit2->t))
			{
				assert(numlinternals < ISECTMAX);
				linternals[numlinternals++] = rhit;
			}
		}
	}
	for (int i = 0; i < nr; i += 2)
	{
		Isect* rhit1 = lhits + i;
		Isect* rhit2 = lhits + i + 1;
		//make sure the hit list is sort properly
		assert(rhit1->t < rhit2->t);
		for (int j = 0; j < nl; ++j)
		{
			Isect* lhit = lhits + j;
			if ((lhit->t >= rhit1->t && lhit->t <= rhit2->t))
			{
				assert(numrinternals < ISECTMAX);
				rinternals[numrinternals++] = lhit;
			}
		}
	}

	//再根据合成关系剔除多余的点
	for (int i = 0; i < nl; ++i)
	{
		Isect* lhit = lhits + i;
		bool isinternal = false;
		for (int j = 0; j < numrinternals; ++j)
		{
			Isect* temp = rinternals[j];
			if (lhit->t == temp->t)
			{
				isinternal = true;
			}
		}
		if (op == '&' && isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *lhit;
		}
		else if (op == '|' && !isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *lhit;
		}
		else if (op == '-' && !isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *lhit;
		}
	}
	for (int i = 0; i < nr; ++i)
	{
		Isect* rhit = rhits + i;
		bool isinternal = false;
		for (int j = 0; j < numlinternals; ++j)
		{
			Isect* temp = linternals[j];
			if (rhit->t == temp->t)
			{
				isinternal = true;
			}
		}
		if (op == '&' && isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *rhit;
		}
		else if (op == '|' && !isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *rhit;
		}
		else if (op == '-' && isinternal)
		{
			assert(numhit < ISECTMAX);
			hit[numhit++] = *rhit;
		}
	}
	std::sort(hit, hit + numhit, [](const Isect l, const Isect r) { return l.t < r.t; });
	return numhit;
}

void Shade(int level, Float weight, Point P, Point N, Point I, Isect* hit, Color color)
{
	Ray tray;
	Color tcolor;
	Surf* surf;

	color[0] = color[1] = color[2] =  0.0 ;

	for (int i = 0; i < scene->numlight; ++i)
	{
		Light light = scene->lights[i];
		Vec L;
		Subtract(light.position, P, L);
		Normalize(L, L);
		Ray RL;
		Copy(RL.P, P);
		Copy(RL.D, L);
		Float dot = Dot(N, L);
		if (dot > 0.0 && Shadow(&RL, 100000.0))
		{
			Color c;
			Multiply(light.color, dot, c);
			Add(color, c, color);
		}
	}

	if (level + 1 < maxlevel)
	{
		Copy(P, tray.P);

		surf = hit[0].prim->surf;
		if (surf->kspec* weight > minweight)
		{
			SpecularDirection(I, N, tray.D);
			Trace(level + 1, surf->kspec*weight, &tray, tcolor);
			MultiplyAdd(surf->kspec, tcolor, color, color);
		}

		if (surf->ktran*weight > minweight)
		{
			if (TransmissionDirection(hit[0].medium, hit[1].medium, I, N, tray.D))
			{
				Trace(level + 1, surf->ktran*weight, &tray, tcolor);
				MultiplyAdd(surf->ktran, tcolor, color, color);
			}
		}
	}
}

Float Shadow(Ray* ray, Float tmax)
{
	int nhit;
	Isect hit[ISECTMAX];
	nhit = Intersect(ray, scene->modelroot, hit);
	if (nhit == 0 || hit[0].t > tmax - rayeps) return 1.0;
	else return 0.0;
}

