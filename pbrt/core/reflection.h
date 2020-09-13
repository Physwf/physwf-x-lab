#pragma once

#include "pbrt.h"
#include "geometry.h"
#include "microfacet.h"
#include "shape.h"
#include "spectrum.h"

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT);
Spectrum FrConductor(Float cosThetaI, const Spectrum &etaI, const Spectrum& etaT, const Spectrum &k);

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

inline Vector3f Reflect(const Vector3f& wo, const Vector3f &n)
{
	return -wo + 2 * Dot(wo, n) * n;
}

inline bool Refract(const Vector3f &wi, const Normal3f &n, Float eta, Vector3f *wt)
{
	float cosThetaI = Dot(n, wi);
	Float sin2ThetaI = std::max(Float(0), Float(1 - cosThetaI * cosThetaI));
	Float sin2ThetaT = eta * eta * sin2ThetaI;

	if (sin2ThetaT >= 1) return false;
	Float cosThetaT = std::sqrt(1 - sin2ThetaI);
	*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
	return true;
}

inline bool SameHemisphere(const Vector3f &w, const Vector3f &wp) {
	return w.z * wp.z > 0;
}

inline bool SameHemisphere(const Vector3f &w, const Normal3f &wp) {
	return w.z * wp.z > 0;
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

class BSDF
{
public:
	BSDF(const SurfaceInteraction& si, Float eat = 1)
		:eta(eta),
		ns(si.shading.n), ng(si.n), ss(Normalize(si.shading.dpdu)), ts(Cross(ns, ss)) {}

	void Add(BxDF* b)
	{
		DOCHECK(nBxDFs < MaxBxDFs);
		bxdfs[nBxDFs++] = b;
	}
	int NumComponents(BxDFType flags = BSDF_ALL) const;
	Vector3f WorldToLocal(const Vector3f &v) const
	{
		return Vector3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
	}
	Vector3f LocalToWorld(const Vector3f& v) const
	{
		return Vector3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
						ss.y * v.x + ts.y * v.y + ns.y * v.z,
						ss.z *v.x + ts.z * v.y * ns.z * v.z);
	}
	Spectrum f(const Vector3f& woW, const Vector3f& wiW, BxDFType flags = BSDF_ALL) const;
	Spectrum rho(int nSamples, const Point2f* smaple1, const Point2f* smaple2, BxDFType flags = BSDF_ALL) const;
	Spectrum rho(const Vector3f& wo, int nSamples, const Point2f* samples, BxDFType flags = BSDF_ALL) const;
	Spectrum Sample_f(const Vector3f& woW, Vector3f* wiW, const Point2f& u, Float* pdf, BxDFType type, BxDFType *sampledType = nullptr) const;
	Float Pdf(const Vector3f& wo, const Vector3f& wi, BxDFType flags = BSDF_ALL) const;

	const Float eta;//相对于boundary的折射率
private:
	~BSDF() {}

	//shading normal ns, gometric normal ng;
	const Normal3f ns, ng;
	const Vector3f ss, ts;
	int nBxDFs = 0;
	static constexpr int MaxBxDFs = 8;
	BxDF* bxdfs[MaxBxDFs];
	friend class MixMaterial;
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
	//半球-方向反射率(hemispherical-directional reflectance),ρhd
	virtual Spectrum rho(const Vector3f &wo, int nSamples, const Point2f *samples) const;
	//半球-半球反射率(hemispherical-hemispherical reflectance),ρhh
	virtual Spectrum rho(int nSamples, const Point2f *smaples1, const Point2f *samples2) const;
	virtual Float Pdf(const Vector3f &wo, const Vector3f& wi) const;
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

class Fresnel
{
public:
	virtual Spectrum Evaluate(Float cosThetaI) const = 0;
};

class FresnelConductor : public Fresnel
{
public:
	FresnelConductor(const Spectrum &etaI, const Spectrum & etaT, const Spectrum& k) : etaI(etaI), etaT(etaT), k(k) {}
	virtual Spectrum Evaluate(Float cosI) const override;
private:
	Spectrum etaI, etaT, k;
};

class FresnelDielectric : public Fresnel
{
public:
	FresnelDielectric(Float etaI, Float etaT) : etaI(etaI),etaT(etaT) { }
	virtual Spectrum Evaluate(Float cosThetaI) const override;
private:
	Float etaI, etaT;
};

class FresnelNoOp : public Fresnel
{
public:
	virtual Spectrum Evaluate(Float cosThetaI) const override { return Spectrum(1.0); }
};
//纯镜面反射
class SpecularReflection : public BxDF
{
public:
	SpecularReflection(const Spectrum& R, Fresnel *fresnel) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), R(R), fresnel(fresnel) {}
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override { return Spectrum(0.f); }
	virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const override;
	virtual Float Pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0; }
private:
	const Spectrum R;
	const Fresnel *fresnel;
};
//纯镜面透射
class SpecularTransmission : public BxDF
{
public:
	SpecularTransmission(const Spectrum &T, Float etaA, Float etaB, TransportMode mode) : BxDF(BxDFType(BSDF_TRANSMISSION|BSDF_SPECULAR)),T(T),etaA(etaA),etaB(etaB),fresnel(etaA,etaB),mode(mode) {}
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override { return Spectrum(0.0f); }
	virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const override;
	virtual Float Pdf(const Vector3f& wo, const Vector3f& wi) const override { return 0; }
private:
	const Spectrum T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};
//FRESNEL-MODULATED SPECULAR REFLECTION AND TRANSMISSION
// single BxDF that represents both specular reflection and specular transmission together, 
// where the relative weightings of the types of scattering are modulated by the dielectric 
// Fresnel equations.
class FresnelSpecular : public BxDF
{
public:
	FresnelSpecular(const Spectrum& R, const Spectrum &T, Float etaA, Float etaB, TransportMode mode) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)), R(R), T(T), etaA(etaA), etaB(etaB), fresnel(fresnel), mode(mode) {}
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override { return Spectrum(0.f); }
	virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const override;
private:
	const Spectrum R, T;
	const Float etaA, etaB;
	const FresnelDielectric fresnel;
	const TransportMode mode;
};

class LambertianReflection : public BxDF
{
public:
	LambertianReflection(const Spectrum& R) :BxDF(BxDFType(BSDF_REFLECTION|BSDF_DIFFUSE)),R(R) {}
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override { return R * InvPi; }
	virtual Spectrum rho(const Vector3f &wo, int nSamples, const Point2f *samples) const { return R; }
	virtual Spectrum rho(int nSamples, const Point2f *smaples1, const Point2f *samples2) const { return R; }
private:
	const Spectrum R;
};

class LambertianTransmission : public BxDF
{
public:
	LambertianTransmission(const Spectrum& T) : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), T(T) {}
	virtual Spectrum f(const Vector3f& wo, const Vector3f &wi) const override { return T * InvPi; }
	virtual Spectrum rho(const Vector3f&, int, const Point2f*) const override { return T; }
	virtual Spectrum rho(int nSamples, const Point2f *smaples1, const Point2f *samples2) const override { return T; }
	virtual Spectrum Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const override;
	virtual Float Pdf(const Vector3f &wo, const Vector3f& wi) const override;
private:
	Spectrum T;
};

class OrenNayar : public BxDF
{
public:
	Spectrum f(const Vector3f& wo, const Vector3f& wi) const;
	OrenNayar(const Spectrum& R, Float sigma)
		:BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
	{
		sigma = Radians(sigma);
		Float sigma2 = sigma * sigma;
		A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33)));
		B = 0.045 * sigma2 / (sigma2 + 0.09f);
	}

private:
	const Spectrum R;
	Float A, B;
};