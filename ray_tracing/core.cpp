#include "core.h"
#include "scene.h"

#include <assert.h>
#include <algorithm>
#include <math.h>
#include <cctype>

int maxlevel = 5;
Float minweight = .01;

bool Inside(const Comp* comp,const Point& P)
{
	if (comp->flag)
	{
		switch (comp->op)
		{
		case '|': return Inside(comp->left, P) || Inside(comp->right, P);
		case '&': return Inside(comp->left, P) && Inside(comp->right, P);
		case '-': return Inside(comp->left, P) && !Inside(comp->right, P);
		}
	}
	else
	{
		return (*((Prim*)comp)->procs->inside)((Prim*)comp, P);
	}
	assert(false);
	return false;
}

void IsectAdd(Isect* hit, Float t, const Point& P, const Prim* prim, int enter, const Surf* medium)
{
	Isect i;
	i.t = t;
	i.P = P;
	i.prim = prim;
	i.enter = enter;
	i.medium = medium;
	*hit = i;
}

Float rayeps = 1e-7;

#define ISECTMAX 10

Color Trace(int level, Float Weight, Ray* ray)
{
	const Prim* prim;
	Point P, N;
	Isect hit[ISECTMAX];
	Color color;

	if (Intersect(ray, scene->modelroot, hit))
	{
		prim = hit[0].prim;
		RayPoint(ray, hit[0].t, P);//计算相交点
		(*prim->procs->normal)(hit[0].prim, P, N);//计算法线
		if (Dot(ray->D, N) > 0.0)
		{
			Negate(N, N);
		}
		//X_LOG("N=(%f,%f,%f)\n", N.X, N.Y, N.Z);
		color = Shade(level, Weight, P, N, ray->D, hit);
	}
	else
	{
		color = {0.0,0.0,0.0};
	}
	return color;
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

			return IntersectMerge(solid, nl, lhit, nr, rhit, hit);
		}
	}
	else
	{
		return (*((Prim*)solid)->procs->intersect)(ray, (Prim*)solid,hit);
	}
}

int IntersectMerge(Comp* solid, int nl, Isect* lhits, int nr, Isect* rhits, Isect* hit)
{
	//make sure we have even number of intersection, when the intersection is tangent, duplicate it in the results.
	assert(nl % 2 == 0);
	assert(nr % 2 == 0);
	int numhit = 0;


	switch (solid->op)
	{
	case '|':
	{
		for (int i = 0; i < nl; ++i)
		{
			if (lhits[i].t > rayeps)
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = lhits[i];
			}
		}
		for (int i = 0; i < nr; ++i)
		{
			if (rhits[i].t > rayeps)
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = rhits[i];
			}
		}
		break;
	}
	case '&':
	{
		for (int i = 0; i < nl; ++i)
		{
			if (lhits[i].t > rayeps && Inside(solid->right, lhits[i].P))
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = lhits[i];
			}
		}
		for (int i = 0; i < nr; ++i)
		{
			if (rhits[i].t > rayeps && Inside(solid->left, rhits[i].P))
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = rhits[i];
			}
		}
		break;
	}
	case '-':
	{
		for (int i = 0; i < nl; ++i)
		{
			if (lhits[i].t > rayeps && !Inside(solid->right, lhits[i].P))
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = lhits[i];
			}
		}
		for (int i = 0; i < nr; ++i)
		{
			if (rhits[i].t > rayeps && Inside(solid->left, rhits[i].P))
			{
				assert(numhit < ISECTMAX);
				hit[numhit++] = rhits[i];
			}
		}
		break;
	}
	}
	std::sort(hit, hit + numhit, [](const Isect l, const Isect r) { return l.t < r.t; });
	return numhit;
}

Color Shade(int level, Float weight, Point P, Point N, Point I, Isect* hit)
{
	Ray tray;
	Color tcolor;
	Surf* surf;
	Color color;


	color = Multiply( Vec(0.2,0.2,0.2), hit[0].medium->kdiff);
	for (int i = 0; i < scene->numlight; ++i)
	{
		const Light& light = scene->lights[i];
		Vec L;
		L = light.position - P;
		Float Dist2 = Dot(L, L);
		Normalize(L);
		Ray RL;
		RL.P = P;
		RL.D = L;
		Float dot = Dot(N, L);
		if (dot > 0.0 && Shadow(&RL, std::sqrtl(Dist2)) > 0.0)
		{
			Float Intencity = light.intensity / Dist2;
			Color diffuse = hit[0].medium->kdiff * (dot * Intencity);
			color = color + diffuse;
			Vec R;
			SpecularDirection(L, N, R);
			Normalize(R);
			Normalize(I);
			Float s = std::max(0.0, Dot(R, I));
			Color specular = hit[0].medium->kspec * (Intencity * std::powl(s,10.0));
			color = color + specular;
		}
	}

	if (level + 1 < maxlevel)
	{
		tray.P = P;

		surf = hit[0].prim->surf;
// 		if (surf->kspec* weight > minweight)
// 		{
// 			SpecularDirection(I, N, tray.D);
// 			tcolor = Trace(level + 1, surf->kspec*weight, &tray);
// 			if (tcolor.R > 0.0 && tcolor.G > 0.0 && tcolor.B > 0.0)
// 			{
// 				//tcolor = { 0.5,0.5,0.5 };
// 			}
// 			MultiplyAdd(surf->kspec, tcolor, color, color);
// 		}

// 		if (surf->ktran*weight > minweight)
// 		{
// 			if (TransmissionDirection(hit[0].medium, hit[1].medium, I, N, tray.D))
// 			{
// 				tcolor = Trace(level + 1, surf->ktran*weight, &tray);
// 				MultiplyAdd(surf->ktran, tcolor, color, color);
// 			}
// 		}
	}
	return color;
}

Float Shadow(Ray* ray, Float tmax)
{
	int nhit;
	Isect hit[ISECTMAX];
	nhit = Intersect(ray, scene->modelroot, hit);
	if (nhit == 0 || hit[0].t > tmax - rayeps)
	{
		//(*(hit[0].prim->procs->print))(hit[0].prim);
		return 1.0;
	}
	else
	{
		//assert(false);
		return 0.0;
	}
}

Float ReadFloat(const char* desc, std::size_t& endpos)
{
	std::size_t startpos = 0;
	while (!std::isdigit(desc[startpos]) && desc[startpos] != '-')
	{
		++startpos;
	}
	endpos = startpos + 1;

	while (std::isdigit(desc[endpos]))
	{
		++endpos;
	}
	return std::atof(&desc[startpos]);
}

Vec ReadVector(const char* desc, std::size_t& endpos)
{
	std::size_t startpos = 0;
	Vec Result;
	int index = 0;
	for (int i = 0; i < 3; ++i)
	{
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
		Result[i] = std::atof(&desc[startpos]);
		startpos = endpos;
	}
	return Result;
}
