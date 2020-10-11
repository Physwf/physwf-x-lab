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
	float LightSourceAngle;		// Defaults to 0.5357 which is the angle for our sun.
	float LightSourceSoftAngle;	//0.0
};
#pragma pack(push)
#pragma pack(1)
struct DeferredLightUniforms
{
	Vector LightPosition;
	float LightInvRadius;
	Vector LightColor;
	float LightFalloffExponent;
	Vector NormalizedLightDirection;
	float DeferredLightUniformsPading01;
	Vector NormalizedLightTangent;
	float DeferredLightUniformsPading02;
	Vector2 SpotAngles;
	float SpecularScale;
	float SourceRadius;
	float SoftSourceRadius;
	float SourceLength;
	float ContactShadowLength;
	float DeferredLightUniformsPading03;
	Vector2 DistanceFadeMAD;
	float DeferredLightUniformsPading04;
	float DeferredLightUniformsPading05;
	Vector4 ShadowMapChannelMask;
	unsigned int ShadowedBits;
	unsigned int LightingChannelMask;
	float VolumetricScatteringIntensity;
	float DeferredLightUniformsPading06;
};
#pragma pack(pop)
