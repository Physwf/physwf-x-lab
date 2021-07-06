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