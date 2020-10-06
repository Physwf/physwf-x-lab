#ifndef __DEFERRED_LIGHTING_COMMON__
#define __DEFERRED_LIGHTING_COMMON__

#include "DeferredShadingCommon.hlsl"

float4 GetDynamicLighting(float3 WorldPosition, float3 CameraVector, GBufferData GBuffer, float AmbientOcclusion, uint ShadingModelID, DeferredLightData LightData, float4 LightAttenuation, float Dither, uint2 SVPos)
{
    return float4(0,0,0,0);
}

#endif