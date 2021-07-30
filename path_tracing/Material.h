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
	MetalMaterial(const LinearColor eta, const LinearColor& k, float alpha) : eta(eta), k(k), alpha(alpha) {}
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena);
private:
	LinearColor eta;//for the metal
	LinearColor k;
	float alpha;
};

class MatteMaterial : public Material
{
public:
	MatteMaterial(LinearColor Kd) : Kd(Kd) {}
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena);
private:
	LinearColor Kd;
};