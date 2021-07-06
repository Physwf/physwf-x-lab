#pragma once
#include "Colorimetry.h"

class Fresnel
{
public:
	virtual LinearColor Evaluate(float cosI) const = 0;
};

class FresnelConductor : public Fresnel
{
public:
	FresnelConductor(const LinearColor& InEtaI, const LinearColor& InEtaT, const LinearColor& InK):etaI(InEtaI),etaT(InEtaT),k(InK) {}
	LinearColor Evaluate(float cosI) const;
private:
	LinearColor etaI, etaT, k;
};

class FresnelDielectric : public Fresnel
{
public:
	FresnelDielectric(float InEtaI, float InEtaT) : etaI(InEtaI), etaT(InEtaT) {}
	LinearColor Evaluate(float cosI) const;
private:
	float etaI, etaT;
};