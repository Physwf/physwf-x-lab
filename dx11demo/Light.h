#pragma once

#include "Math.h"

struct PointLight
{
	Vector4 PoistionAndRadias;
	Vector4 ColorAndFalloffExponent;
};

struct DirectionalLight
{
	Vector Direction;
	float Intencity;
	Vector Color;
};

struct DeferredLightUniforms
{
	Vector LightPosition;
	float LightInvRadius;
	Vector LightColor;
	float LightFalloffExponent;
	Vector NormalizedLightDirection;
	Vector NormalizedLightTangent;
	Vector2 SpotAngles;
	float SpecularScale;
	float SourceRadius;
	float SoftSourceRadius;
	float SourceLength;
	float ContactShadowLength;
	Vector2 DistanceFadeMAD;
	Vector4 ShadowMapChannelMask;
	unsigned int ShadowedBits;
	unsigned int LightingChannelMask;
	float VolumetricScatteringIntensity;
};
