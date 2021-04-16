#include "X9.h"

float X9_API CIE_SkyYc(float fTheta, float fGamma, float fThetaSun, float Yz);

void X9_API GenerateCIESky(int Width, int Height, float* InColor, float fThetaSun, float Yz, float* OutData);