#include "Atmosphere.h"

#include <cmath>

float CIE_SkyYc(float fTheta, float fGamma, float fThetaSun, float Yz)
{
	float fCos_Gamma = std::cosf(fGamma);
	float fCos_Theta = std::cosf(fTheta);
	float fCos_ThetaSun = std::cosf(fThetaSun);
	float fNumerator =	(0.91f + 10.f * std::expf(-3.f * fGamma) + 0.45f * fCos_Gamma * fCos_Gamma)* (1.f - std::expf(-0.32f / fCos_Theta));
	float fDenomerator = (0.91f + 10.f * std::expf(-3.f * fTheta) + 0.45f * fCos_ThetaSun * fCos_ThetaSun) * (1.f - std::expf(-0.32f));
	return Yz * fNumerator / fDenomerator;
}

