#pragma once

#include "Vector.h"
#include "Colorimetry.h"

class BxDF
{
public:
	virtual ~BxDF() {}

	virtual LinearColor f(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual LinearColor Sample_f(const Vector3f& wo, Vector3f* wi, const Vector2f& sample, float* pdf) const;
};