//Primitive
#ifndef __UniformBuffer_Primitive_Definition__
#define __UniformBuffer_Primitive_Definition__
cbuffer	Primitive : register(b1)
{
    float4x4 Primitive_LocalToWorld;
	float4x4 Primitive_WorldToLocal;
	// float4 Primitive_ObjectWorldPositionAndRadius;
	// float3 Primitive_ObjectBounds;
	// half Primitive_LocalToWorldDeterminantSign;
	// float3 Primitive_ActorWorldPosition;
	// half Primitive_DecalReceiverMask;
	float Primitive_PerObjectGBufferData;
	float Primitive_UseSingleSampleShadowFromStationaryLights;
	float Primitive_UseVolumetricLightmapShadowFromStationaryLights;
	float Primitive_UseEditorDepthTest;
	// half4 Primitive_ObjectOrientation;
	// half4 Primitive_NonUniformScale;
	float4 Primitive_InvNonUniformScale;
	// float3 Primitive_LocalObjectBoundsMin;
	// float PrePadding_Primitive_252;
	// float3 Primitive_LocalObjectBoundsMax;
	// uint Primitive_LightingChannelMask;
	// float Primitive_LpvBiasMultiplier;
};
static const struct
{
    float4x4 LocalToWorld;
	float4x4 WorldToLocal;
    float PerObjectGBufferData;
	float UseSingleSampleShadowFromStationaryLights;
	float UseVolumetricLightmapShadowFromStationaryLights;
    float4 InvNonUniformScale;
} Primitive = { Primitive_LocalToWorld, Primitive_WorldToLocal, Primitive_PerObjectGBufferData, Primitive_UseSingleSampleShadowFromStationaryLights,Primitive_UseVolumetricLightmapShadowFromStationaryLights, Primitive_InvNonUniformScale };
#endif