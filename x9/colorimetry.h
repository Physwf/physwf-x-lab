#pragma once

#include <cmath>

void RGB2XYZ(float R, float G, float B, float& X, float& Y, float& Z)
{
	X = 0.4124564f * R + 0.3575761f * G + 0.1804375f * B;
	Y = 0.2126729f * R + 0.7151522f * G + 0.0721750f * B;
	Z = 0.0193339f * R + 0.1191920f * G + 0.9503041f * B;
}

void XYZ2RGB(float X, float Y, float Z, float& R, float& G, float& B)
{
	R = 3.2404542f * X + -1.5371385f * Y + -0.4985314f * Z;
	G = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
	B = 0.0556434f * X + -0.2040259f * Y + 1.0572252f * Z;
}

void XYZ2Yxy(float X, float Y, float Z, float& Yxyx, float& Yxyy, float& Yxyz)
{
	float inv = 1.0f / (X + Y + Z);
	Yxyx = Y;
	Yxyy = X * inv;
	Yxyz = Y * inv;
}

void Yxy2XYZ(float Yxyx, float Yxyy, float Yxyz, float& X, float& Y, float& Z)
{
	X = Yxyx * Yxyy / Yxyz;
	Y = Yxyx;
	Z = Yxyx * (1.0f - Yxyy - Yxyz) / Yxyz;
}

void RGB2Yxy(float R, float G, float B, float& Yxyx, float& Yxyy, float& Yxyz)
{
	float X, Y, Z;
	RGB2XYZ(R, G, B, X, Y, Z);
	XYZ2Yxy(X, Y, Z, Yxyx, Yxyy, Yxyz);
}

void Yxy2RGB(float Yxyx, float Yxyy, float Yxyz, float& R, float& G, float& B)
{
	float X, Y, Z;
	Yxy2XYZ(Yxyx, Yxyy, Yxyz, X, Y, Z);
	XYZ2RGB(X, Y, Z, R, G, B);
}

void Linear2Gamma(float LinearR, float LinearG, float LinearB, float& GammaR, float& GammaG, float& GammaB)
{
	GammaR = std::powf(LinearR, 1.0f / 2.2f);
	GammaG = std::powf(LinearG, 1.0f / 2.2f);
	GammaB = std::powf(LinearB, 1.0f / 2.2f);
}
