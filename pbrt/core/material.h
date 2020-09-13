#pragma once

#include "pbrt.h"
#include "memory.h"

enum class TransportMode { Radiance, Importance };

class Material
{
public:
	virtual void ComputeScatteringFunctions(SurfaceInteraction* si, MemoryArena& arena, TransportMode mode, bool allowMultipleLobes) const = 0;
	virtual ~Material();
	static void Bump(const std::shared_ptr<Texture<Float>> &d,SurfaceInteraction* si);
};