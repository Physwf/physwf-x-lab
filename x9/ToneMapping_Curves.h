#pragma once

#include <cmath>

inline float Reinhard(float x) {
	return x / (1.0f + x);
}

inline float Reinhard2(float x) {
	const float L_white = 4.0;
	return (x * (1.0f + x / (L_white * L_white))) / (1.0f + x);
}

inline float Tonemap_ACES(float x) {
	// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

float Tonemap_Unreal(float x) {
	// Unreal 3, Documentation: "Color Grading"
	// Adapted to be close to Tonemap_ACES, with similar range
	// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
	return x / (x + 0.155f) * 1.019f;
}

float clamp(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline float smoothstep(float edge0, float edge1, float x)
{
	float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

inline float step(float edge, float x)
{
	return x < edge ? 0.f : 1.0f;
}

inline float Tonemap_Uchimura(float x, float P, float a, float m, float l, float c, float b) {
	// Uchimura 2017, "HDR theory and practice"
	// Math: https://www.desmos.com/calculator/gslcdxvipg
	// Source: https://www.slideshare.net/nikuque/hdr-theory-and-practicce-jp
	float l0 = ((P - m) * l) / a;
	float L0 = m - m / a;
	float L1 = m + (1.0f - m) / a;
	float S0 = m + l0;
	float S1 = m + a * l0;
	float C2 = (a * P) / (P - S1);
	float CP = -C2 / P;

	float w0 = 1.0f -   smoothstep(0.0f, m, x);
	float w2 = step(m + l0, x);
	float w1 = 1.0f - w0 - w2;

	float T = m * pow(x / m, c) + b;
	float S = P - (P - S1) * exp(CP * (x - S0));
	float L = m + a * (x - m);

	return T * w0 + L * w1 + S * w2;
}

inline float Tonemap_Uchimura(float x) {
	const float P = 1.0f;  // max display brightness
	const float a = 1.0f;  // contrast
	const float m = 0.22f; // linear section start
	const float l = 0.4f;  // linear section length
	const float c = 1.33f; // black
	const float b = 0.0f;  // pedestal
	return Tonemap_Uchimura(x, P, a, m, l, c, b);
}

inline float Tonemap_Lottes(float x) {
	// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
	const float a = 1.6f;
	const float d = 0.977f;
	const float hdrMax = 8.0f;
	const float midIn = 0.18f;
	const float midOut = 0.267f;

	// Can be precomputed
	const float b =
		(-std::powf(midIn, a) + std::powf(hdrMax, a) * midOut) /
		((std::powf(hdrMax, a * d) - std::powf(midIn, a * d)) * midOut);
	const float c =
		(std::powf(hdrMax, a * d) * std::powf(midIn, a) - std::powf(hdrMax, a) * std::powf(midIn, a * d) * midOut) /
		((std::powf(hdrMax, a * d) - std::powf(midIn, a * d)) * midOut);

	return std::powf(x, a) / (std::powf(x, a * d) * b + c);
}
