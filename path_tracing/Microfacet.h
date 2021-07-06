#pragma once

#include "Vector.h"

class MicrofacetDistribution
{
public:
	virtual ~MicrofacetDistribution() {}
	virtual float D(const Vector3f& wh) const = 0;
	virtual float G1(const Vector3f& w, const Vector3f& wh) const = 0;
	float G(const Vector3f& wo, const Vector3f& wi, const Vector3f& wh) const
	{
		return G1(wo, wh) * G1(wi, wh);
	}
	virtual Vector3f Sample_wh(const Vector3f& wo, const Vector2f& u) const = 0;
	float Pdf(const Vector3f& wo, const Vector3f& wh) const;
};


class BeckmannDistribution : public MicrofacetDistribution
{
public:
	BeckmannDistribution(float alpha) :alpha(alpha) {}
	virtual float D(const Vector3f& wh) const;
	virtual float G1(const Vector3f& w, const Vector3f& wh) const;
	virtual Vector3f Sample_wh(const Vector3f& wo, const Vector2f& u) const;
private:
	const float alpha;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution
{
public:
	TrowbridgeReitzDistribution(float alpha) :alpha(alpha) {}
	virtual float D(const Vector3f& wh) const;
	virtual float G1(const Vector3f& w, const Vector3f& wh) const;
	virtual Vector3f Sample_wh(const Vector3f& wo, const Vector2f& u) const;
private:
	const float alpha;
};