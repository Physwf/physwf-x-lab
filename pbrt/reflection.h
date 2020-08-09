#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "spectrum.h"


inline Float CosTheta(const Vector3f & w) { return w.z; }
inline Float Cos2Theta(const Vector3f &w) { return w.z * w.z; }
inline Float AbsCosTheta(const Vector3f &w) { return std::abs(w.z); }
inline Float Sin2Theta(const Vector3f &w) { return std::max((Float)0,(Float)1-Cos2Theta(w)); }
inline Float SinTheta(const Vector3f &w) { return std::sqrt(Sin2Theta(w)); }
inline Float TanTheta(const Vector3f &w) { return SinTheta(w) / CosTheta(w); }
inline Float Tan2Theta(const Vector3f &w) { return Sin2Theta(w) / Cos2Theta(w); }

inline Float CosPhi(const Vector3f &w) 
{ 
	Float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.y / sinTheta, -1, 1);
}
inline Float SinPhi(const Vector3f &w)
{
	Float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}
inline Float Cos2Phi(const Vector3f& w)
{
	Float cosPhi = Cos2Phi(w);
	return cosPhi * cosPhi;
}
inline Float Sin2Phi(const Vector3f &w)
{
	Float sinPhi = SinPhi(w);
	return sinPhi * sinPhi;
}
//delta phi
inline Float CosDPhi(const Vector3f &wa, const Vector3f& wb)
{
	return Clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt((wa.x*wa.x + wa.y*wa.y)*(wb.x*wb.x + wb.y*wb.y)),-1,1);
}

enum BxDFType
{
	BSDF_REFLECTION			= 1 << 0,
	BSDF_TRANSMISSION		= 1 << 1,
	BSDF_DIFFUSE			= 1 << 2,
	BSDF_GLOSSY				= 1 << 3,
	BSDF_SPECULAR			= 1 << 4,
	BSDF_ALL				= BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_REFLECTION,
};

class BxDF
{
public:
	BxDF(BxDFType type) :type(type) { }
	virtual ~BxDF() { }

	bool MatchFlags(BxDFType t)
	{
		return (type & t) == type;
	}

	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const = 0;
	virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType = nullptr) const;
	virtual Spectrum rho(const Vector3f &wo, int nSamples, const Point2f *samples) const;
	virtual Spectrum rho(int nSamples, const Point2f *smaples1, const Point2f *samples2) const;
	
	BxDFType type;
};


class ScaledBxDF : public BxDF
{
public:
	ScaledBxDF(BxDF* bxdf,const Spectrum& scale) : BxDF(BxDFType(bxdf->type)),bxdf(bxdf),scale(scale) { }
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override
	{
		return scale * bxdf->f(wo, wi);
	}
private:
	BxDF * bxdf;
	Spectrum scale;
};