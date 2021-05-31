#include "Fresnel.h"
#include "Mathematics.h"
#include <algorithm>

float FrDielectirc(float cosThetaI, float etaI, float etaT)
{
	cosThetaI = Math::Clamp(cosThetaI, -1.f, 1.f);
	bool entering = cosThetaI > 0.f;
	if (!entering)
	{
		std::swap(etaI, etaT);
		cosThetaI = std::abs(cosThetaI);
	}

	float sinThetaI = std::sqrtf(std::max(0.f, 1 - cosThetaI * cosThetaI));
	float sinTehtaT = etaI / etaT * sinThetaI;

	if (sinTehtaT >= 1.f) return 1.f;//total reflection

	float cosThetaT = std::sqrt(std::max(0.f, 1 - sinTehtaT * sinTehtaT));

	float Rparallel = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperpendi = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
	return (Rparallel * Rparallel + Rperpendi * Rperpendi) * 0.5f;
}

LinearColor FrConductor(float cosThetaI, const LinearColor &etai, const LinearColor &etat, const LinearColor &k)
{
	cosThetaI = Math::Clamp(cosThetaI, -1.f, 1.f);
	LinearColor eta = etat / etai;
	LinearColor etak = k / etai;

	float cosThetaI2 = cosThetaI * cosThetaI;
	float sinThetaI2 = 1. - cosThetaI2;
	LinearColor eta2 = eta * eta;
	LinearColor etak2 = etak * etak;

	LinearColor t0 = eta2 - etak2 - sinThetaI2;
	LinearColor a2plusb2 = Sqrt(t0 * t0 + 4 * eta2 * etak2);
	LinearColor t1 = a2plusb2 + cosThetaI2;
	LinearColor a = Sqrt(0.5f * (a2plusb2 + t0));
	LinearColor t2 = (float)2 * cosThetaI * a;
	LinearColor Rs = (t1 - t2) / (t1 + t2);

	LinearColor t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	LinearColor t4 = t2 * sinThetaI2;
	LinearColor Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}

LinearColor FresnelConductor::Evaluate(float cosI)
{
	return FrConductor(std::abs(cosI), etaI, etaT, k);
}

LinearColor FresnelDielectric::Evaluate(float cosI)
{
	return FrDielectirc(cosI,etaI,etaT);
}
