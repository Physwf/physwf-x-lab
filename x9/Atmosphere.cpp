#include "Atmosphere.h"
#include "colorimetry.h"
#include <cmath>
#include <assert.h>

float CIE_SkyYc(float fTheta, float fCos_Gamma, float fThetaSun, float Yz)
{
	float fGamma = std::acosf(fCos_Gamma);
	float fCos_Theta = std::cosf(fTheta);
	float fCos_ThetaSun = std::cosf(fThetaSun);
	float e1 = std::expf(-3.f * fGamma);
	float e2 = std::expf(-0.32f / fCos_Theta);
	float fNumerator =	(0.91f + 10.f * e1 + 0.45f * fCos_Gamma * fCos_Gamma)* (1.f - e2);
	float fDenomerator = (0.91f + 10.f * std::expf(-3.f * fTheta) + 0.45f * fCos_ThetaSun * fCos_ThetaSun) * (1.f - std::expf(-0.32f));
	return Yz * fNumerator / fDenomerator;
}

#define PI 3.14f

void GenerateCIESky(int Width, int Height, float* InRGBColor, float fThetaSun, float Yz, float* OutData)
{
	float Yxy[3];
	RGB2Yxy(InRGBColor[0], InRGBColor[1], InRGBColor[2], Yxy[0], Yxy[1], Yxy[2]);
	
	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Height; ++y)
		{
			float fPhi = ((float)x + 0.5f) / (float)Width * PI * 2.0f;
			float fTheta = (1.f - (((float)y + 0.5f) / (float)Height)) * PI * 0.5f;
			float fPhiSun = PI * 0.5f;

			float fCos_Gamma = 
				std::sinf(fThetaSun) * std::cosf(fPhiSun) * std::sinf(fTheta)*std::cosf(fPhi)
				+ std::sinf(fThetaSun) * std::sinf(fPhiSun) * std::sinf(fTheta) * std::sinf(fPhi)
				+ std::cosf(fThetaSun) * std::cosf(fTheta);
			Yxy[0] = CIE_SkyYc(fTheta, fCos_Gamma, fThetaSun, Yz);
			int Index = (y * Width + x)*3;
			Yxy2RGB(Yxy[0], Yxy[1], Yxy[2], OutData[Index + 0], OutData[Index + 1], OutData[Index + 2]);
		}
	}
}

