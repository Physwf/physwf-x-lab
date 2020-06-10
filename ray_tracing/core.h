#pragma once

#include "math.h"

struct Ray
{
	Point P;//origin
	Point D;//direction
};

//calculate a "point" on a "ray" with parameter "t"
#define RayPoint(ray,t,point) MultiplyAdd(t,ray->D,ray->P,point);

struct Comp
{
	int flag;//=1
	int op;
	Comp* left;
	Comp* right;
};

struct Prim
{
	int flag;//=0
	void* info;
	struct PrimProcs* procs;
	//Matrix mat;
	struct Surf *surf;
};

struct PrimProcs
{
	const char*(*name)(const Prim* prim);
	void(*print)(const Prim* prim);
	struct Prim* (*read)(const char* desc, std::size_t size);
	int(*intersect)(const Ray* ray, const Prim* prim,struct Isect* hit);
	void(*normal)(const Prim* prim, const Point P, Point N);
};

struct Surf
{
	Float kdiff;//漫反射系数
	Float kspec;//镜面反射系数
	Float ktran;//透射系数
	Color color;//
	Float refrindex;//折射率
};

struct Isect
{
	Float t;
	const Prim *prim;
	int enter;//enter or out
	const Surf *medium;
};

void IsectAdd(Isect* hit, Float t, const Prim* prim, int enter, const Surf* medium);
// extern int maxlevel;
// extern Float minweight;
extern Float rayeps;

void Trace(int level, Float Weight, Ray* ray, Color color);
int Intersect(Ray* ray, Comp* solid, Isect* hit);
int IntersectMerge(int op, int nl, Isect* lhits, int nr, Isect* rhits, Isect* hit);
void Shade(int level, Float weight, Point P, Point N, Point I, Isect* hit, Color color);
Float Shadow(Ray* ray, Float tmax);
