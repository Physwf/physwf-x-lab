#include "reflection.h"

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT)
{
	cosThetaI = Clamp(cosThetaI, -1, 1);
	//如果入射角同发现不在一侧，则反转对应参数
	bool entering = cosThetaI > 0.f;
	if (!entering)
	{
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}
	Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
	Float sinThetaT = etaI / etaT * sinThetaI;
	if (sinThetaT >= 1) return 1;//全反射
	Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));
	Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	Float Rperp = ((etaI * cosThetaT) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparl*Rparl + Rperp * Rperp) / 2;
}

Spectrum FrConductor(Float cosThetaI, const Spectrum &etaI, const Spectrum& etaT, const Spectrum &k)
{
	cosThetaI = Clamp(cosThetaI, -1, 1);
	Spectrum eta = etaT / etaI;
	Spectrum etak = k / etaI;

	Float cosThetaI2 = cosThetaI * cosThetaI;
	Float sinThetaI2 = 1. - cosThetaI2;
	Spectrum eta2 = eta * eta;
	Spectrum etak2 = etak * etak;
	
	Spectrum t0 = eta2 - etak2 - sinThetaI2;
	Spectrum a2plusb2 = Sqrt(t0*t0 + 4 * eta2*eta2);
	Spectrum t1 = a2plusb2 + cosThetaI2;
	Spectrum a = Sqrt(0.5f * (a2plusb2 + t0));
	Spectrum t2 = (Float)2 * cosThetaI * a;
	Spectrum Rs = (t1 - t2) / (t1 + t2);

	Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	Spectrum t4 = t2 * sinThetaI2;
	Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}



Spectrum FresnelConductor::Evaluate(Float cosThetaI) const
{
	return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
}

Spectrum FresnelDielectric::Evaluate(Float cosThetaI) const
{
	return FrDielectric(cosThetaI, etaI, etaT);
}

Spectrum SpecularReflection::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	*wi = Vector3f(-wo.x, wo.y, wo.z);
	*pdf = 1;
	return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}

Spectrum SpecularTransmission::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	bool entering = CosTheta(wo) > 0;
	Float etaI = entering ? etaA : etaB;
	Float etaT = entering ? etaB : etaA;
	if (!Refract(wo, FaceForward(Normal3f(0, 0, 1), wo), etaI / etaT, wi))
	{
		return 0;
	}
	*pdf = 1;
	Spectrum ft = T * Spectrum(1.) - fresnel.Evaluate(CosTheta(*wi));
	if (mode == TransportMode::Radiance)
	{
		ft *= (etaI * etaI) / (etaT*etaT);
	}
	return ft / AbsCosTheta(*wi);
}

Spectrum FresnelSpecular::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float * pdf, BxDFType *sampleType /* = nullptr */) const
{
	Float F = FrDielectric(CosTheta(wo), etaA, etaB);

}
