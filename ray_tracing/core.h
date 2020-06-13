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
	struct Prim* (*read)(const char* desc, std::size_t &size);
	int(*intersect)(const Ray* ray, const Prim* prim,struct Isect* hit);
	void(*normal)(const Prim* prim, const Point& P, Point& N);
	bool(*inside)(const Prim* prim, const Point& P);
};

bool Inside(const Comp* comp,const Point& P);

struct Surf
{
	Color kdiff;//漫反射系数
	Color kspec;//镜面反射系数
	Color ktran;//透射系数
	Float n;//折射率
};

struct Isect
{
	Float t;
	Point P;
	const Prim *prim;
	int enter;//enter or out
	const Surf *medium;
};

void IsectAdd(Isect* hit, Float t, const Point& P, const Prim* prim, int enter, const Surf* medium);
// extern int maxlevel;
// extern Float minweight;
extern Float rayeps;

Color Trace(int level, Float Weight, Ray* ray);
int Intersect(Ray* ray, Comp* solid, Isect* hit);
int IntersectMerge(Comp* solid, int nl, Isect* lhits, int nr, Isect* rhits, Isect* hit);
Color Shade(int level, Float weight, Point P, Point N, Point I, Isect* hit);
Float Shadow(Ray* ray, Float tmax);

Float ReadFloat(const char* desc, std::size_t& endpos);
Vec ReadVector(const char* desc, std::size_t& endpos);

#include <stdio.h>
#include <stdarg.h>
#define X_LOG(Format,...) XLOG(Format, __VA_ARGS__)

inline void XLOG(const char* format, ...)
{
	char buffer[1024] = { 0 };
	va_list v_list;
	va_start(v_list, format);
	vsprintf_s(buffer, format, v_list);
	va_end(v_list);
	extern void OutputDebug(const char* Format);
	OutputDebug(buffer);
}
