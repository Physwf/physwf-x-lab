#pragma once

#include "Memory.h"
#include "RayTracing.h"

class Material
{
public:
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena) = 0;
};

class MirrorMaterial : public Material
{
public:
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena);
};

class MetalMaterial : public Material
{
public:
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena);
};

class MatteMaterial : public Material
{
public:
	MatteMaterial(LinearColor Kd) : Kd(Kd) {}
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena);
private:
	LinearColor Kd;
};