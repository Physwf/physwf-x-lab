#include "primitives.h"
#include <stdio.h>
#include <assert.h>
#include <algorithm>

PrimProcs SphereProcs = { SphereName, SpherePrint, SphereRead, SphereIntersect, SphereNormal, SphereInside };
PrimProcs BoxProcs = { BoxName, BoxPrint, BoxRead, BoxIntersect, BoxNormal, BoxInside };

void SpherePrint(const Prim* prim)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	X_LOG("sphere : center(%g, % g, % g) radius %g\n", s->center[0], s->center[1], s->center[2], s->rad);
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

	IsectAdd(hit, t1, ray->P + ray->P*t1, prim, 1, prim->surf);
	hit++;
	IsectAdd(hit, t2, ray->P + ray->P*t2, prim, 0, prim->surf);
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

bool SphereInside(const Prim* prim, const Point& P)
{
	const Sphere* s;
	s = (Sphere*)prim->info;
	Vec v = P - s->center;
	return Dot(v, v) < s->rad2;
}

void BoxPrint(const Prim* prim)
{
	const Box* b;
	b = (Box*)prim->info;
	X_LOG("sphere : center(%g, % g, % g) size(%g, % g, % g)\n", b->center[0], b->center[1], b->center[2], b->size[0], b->size[1], b->size[2] );
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
	//X_LOG("ray.P=(%f,%f,%f),ray.D=(%f,%f,%f)\n", ray->P.X, ray->P.Y, ray->P.Z, ray->D.X, ray->D.Y, ray->D.Z);

	Box* box = (Box*)prim->info;

	Vec halfsize = box->size / 2.0;
	Float halfdiagnal2 = Dot(halfsize, halfsize);//box的对焦距离一半的平方
	Vec boxdir = box->center - ray->P;//box中心到射线起点的矢量
	Float boxdist2 = Dot(boxdir, boxdir);//box中心到射线起点距离的平方
	Float boxdirproj = Dot(boxdir, ray->D);
	if (boxdist2 - boxdirproj * boxdirproj > halfdiagnal2) return 0;//射线与box中心的距离小于box的对角线距离的一半

	Float xmin = box->center[0] - box->size[0] / 2.0;
	Float xmax = box->center[0] + box->size[0] / 2.0;
	Float ymin = box->center[1] - box->size[1] / 2.0;
	Float ymax = box->center[1] + box->size[1] / 2.0;
	Float zmin = box->center[2] - box->size[2] / 2.0;
	Float zmax = box->center[2] + box->size[2] / 2.0;
	//X_LOG("xmin=%f,xmax=%f,ymin=%f,ymax=%f,zmin=%f,zmax=%f\n", xmin, xmax, ymin, ymax, zmin, zmax);
	Float tcomponents[] = { (xmin - ray->P[0]), (xmax - ray->P[0]), (ymin - ray->P[1]), (ymax - ray->P[1]) ,(zmin - ray->P[2]), (zmax - ray->P[2]) };
	int numtcandidates = 0;
	Float tcandidates[6];
	if (std::abs(ray->D.X) > rayeps)
	{
		tcandidates[numtcandidates++] = tcomponents[0] / ray->D.X;
		tcandidates[numtcandidates++] = tcomponents[1] / ray->D.X;
	}
	if (std::abs(ray->D.Y) > rayeps)
	{
		tcandidates[numtcandidates++] = tcomponents[2] / ray->D.Y;
		tcandidates[numtcandidates++] = tcomponents[3] / ray->D.Y;
	}
	if (std::abs(ray->D.Z) > rayeps)
	{
		tcandidates[numtcandidates++] = tcomponents[4] / ray->D.Z;
		tcandidates[numtcandidates++] = tcomponents[5] / ray->D.Z;
	}

	int numt = 0;
	Float t[6];
	t[numt++] = tcandidates[0];
	for (int i = 1; i < 6; ++i)
	{
		int j = 0;
		for (; j < numt; ++j)
		{
			if (std::abs( t[j] - tcandidates[i]) < rayeps) break;
		}
		if (j == numt) t[numt++] = tcandidates[i];
	}

	Vec validinsects[6];
	int num = 0;
	for (int k = 0; k < numt; ++k)
	{
		Vec insect = ray->P + ray->D * t[k];
		//X_LOG("insect=(%f,%f,%f)\n", insect.X, insect.Y, insect.Z);
// 		bool inx = insect.X - xmin > rayeps && insect.X - xmax < rayeps;
// 		bool iny = insect.Y - ymin > rayeps && insect.Y - ymax < rayeps;
// 		bool inz = insect.Z - zmin > rayeps && insect.Z - zmax < rayeps;
		bool inx = insect.X > xmin && insect.X < xmax;
		bool iny = insect.Y > ymin && insect.Y < ymax;
		bool inz = insect.Z > zmin && insect.Z < zmax;
		bool marginx = std::abs(insect.X - xmin) < rayeps || std::abs(insect.X - xmax) < rayeps;
		bool marginy = std::abs(insect.Y - ymin) < rayeps || std::abs(insect.Y - ymax) < rayeps;
		bool marginz = std::abs(insect.Z - zmin) < rayeps || std::abs(insect.Z - zmax) < rayeps;
		//X_LOG("inx=%d,iny=%d,inz=%d,marginx=%d,marginy=%d,marginz=%d\n", inx, iny, inz, marginx, marginy, marginz);
		if ((inx && iny && marginz) || (inx && inz && marginy) || (iny && inz && marginx) || (marginx && marginy && inz) || (marginx && marginz && iny) || (marginy && marginz && inx) || (marginx && marginy && marginz))
		{
			int j = 0;
			for (; j < num; ++j)
			{
				if (validinsects[j] == insect) break;
			}
			if (j == num) validinsects[num++] = insect;
		}
	}
	if (num > 1)
	{
		if (num > 2)
		{
			X_LOG("num=%d\n", num);
		}
		assert(num % 2 == 0);
		Vec Dir1 = validinsects[0] - ray->P;
		Vec Dir2 = validinsects[1] - ray->P;
		Float t1 = Lenght(validinsects[0] - ray->P);
		Float t2 = Lenght(validinsects[1] - ray->P);
		if (Dot(Dir1, ray->D) < 0.0)
		{
			t1 = -t1;
		}
		if (Dot(Dir2, ray->D) < 0.0)
		{
			t2 = -t2;
		}

		if (t1 < 0.0 && t2 < 0.0)
		{
			//X_LOG("num=%d\n", 0);
			return 0;
		}
		if (t1 > t2)
		{
			IsectAdd(hit, t2, validinsects[1], prim, 1, prim->surf);
			hit++;
			IsectAdd(hit, t1, validinsects[0], prim, 0, prim->surf);
		}
		else
		{
			IsectAdd(hit, t1, validinsects[0], prim, 1, prim->surf);
			hit++;
			IsectAdd(hit, t2, validinsects[1], prim, 0, prim->surf);
		}
		return num;
	}
	return 0;
}

void BoxNormal(const Prim* prim, const Point& P, Point& N)
{
	const Box* box;
	box = (Box*)prim->info;
	Float xmin = box->center[0] - box->size[0] / 2.0;
	Float xmax = box->center[0] + box->size[0] / 2.0;
	Float ymin = box->center[1] - box->size[1] / 2.0;
	Float ymax = box->center[1] + box->size[1] / 2.0;
	Float zmin = box->center[2] - box->size[2] / 2.0;
	Float zmax = box->center[2] + box->size[2] / 2.0;

	if (std::abs(P.X - xmin) < rayeps) N = { -1.0,0.0,0.0 };
	if (std::abs(P.X - xmax) < rayeps) N = { 1.0,0.0,0.0 };
	if (std::abs(P.Y - ymin) < rayeps) N = { 0.0,-1.0,0.0 };
	if (std::abs(P.Y - ymax) < rayeps) N = { 0.0,1.0,0.0 };
	if (std::abs(P.Z - zmin) < rayeps) N = { 0.0,0.0,-1.0 };
	if (std::abs(P.Z - zmax) < rayeps) N = { 0.0,0.0,1.0 };
}

bool BoxInside(const Prim* prim, const Point& P)
{
	const Box* box = (Box*)prim->info;
	return 
		P[0] > box->center[0] - box->size[0] / 2.0 &&
		P[0] < box->center[0] + box->size[0] / 2.0 &&
		P[1] > box->center[1] - box->size[0] / 2.0 &&
		P[1] < box->center[1] + box->size[0] / 2.0 &&
		P[2] > box->center[2] - box->size[0] / 2.0 &&
		P[2] < box->center[2] + box->size[0] / 2.0;
}

