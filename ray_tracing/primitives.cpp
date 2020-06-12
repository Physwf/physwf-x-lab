#include "primitives.h"
#include <stdio.h>

PrimProcs SphereProcs = { SphereName, SpherePrint, SphereRead, SphereIntersect, SphereNormal };
PrimProcs BoxProcs = { BoxName, BoxPrint, BoxRead, BoxIntersect, BoxNormal };

void SpherePrint(const Prim* prim)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	printf("sphere : center(%g, % g, % g) radius %g\n", s->center[0], s->center[1], s->center[2], s->rad);
}


Prim* SphereRead(const char* desc, std::size_t& size)
{
	Sphere* s = new Sphere();
	Surf* surf = new Surf();

	std::size_t i = 2;//skip S and (
	int index = 0;
	while (desc[i] != ')')
	{
		std::size_t step = 1;
		switch (desc[i])
		{
		case 'p':
		case 'P':
		{
			s->center = ReadVector(&desc[i], step);
		}
			break;
		case 'r':
		case 'R':
		{
			s->rad = ReadFloat(&desc[i], step);
			s->rad2 = s->rad*s->rad;
		}
			break;
		case 'd':
		case 'D':
		{
			surf->kdiff = ReadVector(&desc[i], step);
		}
			break;
		case 's':
		case 'S':
		{
			surf->kspec = ReadVector(&desc[i], step);
		}
			break;
		case 't':
		case 'T':
		{
			surf->ktran = ReadVector(&desc[i], step);
		}
			break;
		case 'n':
		case 'N':
		{
			surf->n = ReadFloat(&desc[i], step);
		}
			break;
		}
		i += step;
	}
	size = i;
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

void BoxPrint(const Prim* prim)
{
	const Box* b;
	b = (Box*)prim->info;
	printf("sphere : center(%g, % g, % g) size(%g, % g, % g)\n", b->center[0], b->center[1], b->center[2], b->size[0], b->size[1], b->size[2] );
}

Prim* BoxRead(const char* desc, std::size_t& size)
{
	Box* b = new Box();
	Surf* surf = new Surf();

	std::size_t i = 2;//skip S and (
	int index = 0;
	while(desc[i] != ')')
	{
		std::size_t step = 1;
		switch (desc[i])
		{
		case 'p':
		case 'P':
		{
			b->center = ReadVector(&desc[i], step);
		}
		break;
		case 'z':
		case 'Z':
		{
			b->size = ReadVector(&desc[i], step);
		}
		break;
		case 'd':
		case 'D':
		{
			surf->kdiff = ReadVector(&desc[i], step);
		}
		break;
		case 's':
		case 'S':
		{
			surf->kspec = ReadVector(&desc[i], step);
		}
		break;
		case 't':
		case 'T':
		{
			surf->ktran = ReadVector(&desc[i], step);
		}
		break;
		case 'n':
		case 'N':
		{
			surf->n = ReadFloat(&desc[i], step);
		}
		break;
		}
		i += step;
	}
	size = i;
	Prim* Result = new Prim();
	Result->flag = 0;
	Result->procs = &BoxProcs;
	Result->info = b;
	Result->surf = surf;

	return Result;
}

int BoxIntersect(const Ray* ray, const Prim* prim, Isect* hit)
{
	Box* box = (Box*)prim->info;

	Vec halfsize = box->size / 2.0;
	Float halfdiagnal2 = Dot(halfsize, halfsize);//box的对焦距离一半的平方
	Vec boxdir = box->center - ray->D;//box中心到射线起点的矢量
	Float boxdist2 = Dot(boxdir, boxdir);//box中心到射线起点距离的平方
	Float boxdirproj = Dot(boxdir, ray->D);
	if (boxdist2 - boxdirproj * boxdirproj > halfdiagnal2) return 0;//射线与box中心的距离小于box的对角线距离的一半
	
	Float xmin = box->center[0] - box->size[0] / 2.0;
	Float xmax = box->center[0] + box->size[0] / 2.0;
	Float ymin = box->center[1] - box->size[1] / 2.0;
	Float ymax = box->center[1] + box->size[1] / 2.0;
	Float zmin = box->center[2] - box->size[2] / 2.0;
	Float zmax = box->center[2] + box->size[2] / 2.0;

	Float tx[] = { (xmin - ray->P[0]) * ray->D.X, (xmax - ray->P[0]) * ray->D.X };
	Float ty[] = { (ymin - ray->P[1]) * ray->D.Y, (ymax - ray->P[1]) * ray->D.Y };
	Float tz[] = { (zmin - ray->P[2]) * ray->D.Z, (zmax - ray->P[2]) * ray->D.Z };

	Vec validinsects[2];
	int num = 0;
	for (int x = 0; x < 2; ++x)
		for (int y = 0; y < 2; ++y)
			for (int z = 0; z < 2; ++z)
			{
				Vec insect = ray->P + Vec(tx[x], ty[y], tz[z]);
				if ((insect.X > xmin  && insect.X < xmax)  && (insect.Y > ymin  && insect.Y < ymax) && (insect.Z > zmin && insect.Z < zmax) )
				{
					validinsects[num++] = insect;
				}
				else if ((insect.X == xmin && insect.X == xmax) && (insect.Y > ymin && insect.Y < ymax) && (insect.Z > zmin && insect.Z < zmax))
				{
					validinsects[num++] = insect;
				}
				else if (insect.X > xmin && insect.X < xmax && insect.Y == ymin && insect.Y == ymax && insect.Z > zmin && insect.Z < zmax)
				{
					validinsects[num++] = insect;
				}
				else if (insect.X > xmin && insect.X < xmax && insect.Y == ymin && insect.Y == ymax && insect.Z == zmin && insect.Z == zmax)
				{
					validinsects[num++] = insect;
				}
			}

	if (num > 0)
	{
		Float t1 = Lenght(validinsects[0] - ray->P);
		Float t2 = Lenght(validinsects[1] - ray->P);
		if (t1 > t2)
		{
			Float temp = t1;
			t1 = t2;
			t1 = temp;
		}
		IsectAdd(hit, t1, prim, 1, prim->surf);
		hit++;
		IsectAdd(hit, t2, prim, 0, prim->surf);
	}
	return num;
}

void BoxNormal(const Prim* prim, const Point& P, Point& N)
{
	const Box* b;
	b = (Box*)prim->info;
	N = P - b->center;
	Vec halfsize = b->size / 2.0;
	if (N.X > b->center[0] - halfsize[0] && N.X < b->center[0] + halfsize[0] &&
		N.Y > b->center[1] - halfsize[1] && N.Y < b->center[1] + halfsize[1] )
	{
		N.X = N.Y = 0;
		if (std::abs(N.Z) < halfsize.Z) N.Z = -1.0;
		else N.Z = 1.0;
	}
	else if (N.Y > b->center[1] - halfsize[1] && N.Y < b->center[1] + halfsize[1] &&
			N.Z >= b->center[2] - halfsize[2] && N.Z < b->center[2] + halfsize[2])
	{
		N.Y = N.Z = 0;
		if (std::abs(N.X) < halfsize.X) N.X = -1.0;
		else N.X = 1.0;
	}
	else if (N.X > b->center[1] - halfsize[1] && N.X < b->center[1] + halfsize[1] &&
			N.Z >= b->center[2] - halfsize[2] && N.Z < b->center[2] + halfsize[2])
	{
		N.X = N.Z = 0;
		if (std::abs(N.Y) < halfsize.Y) N.Y = -1.0;
		else N.Y = 1.0;
	}
}

