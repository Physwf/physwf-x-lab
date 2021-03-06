
#ifndef __CapsuleLightIntegrate_H__
#define __CapsuleLightIntegrate_H__

#include "DeferredShadingCommon.hlsl"
//#include "MonteCarlo.ush"
#include "ShadingModels.hlsl"
#include "CapsuleLight.hlsl"

float IntegrateLight(CapsuleLight Capsule, bool bInverseSquared)
{
    float Falloff;

    [branch]
    if(Capsule.Length > 0)
    {
        float NoL;
        float LineCosSubtended = 1;
        LineIrradiace(0,Capsule.LightPos[0],Capsule.LightPos[1],Capsule.DistBiasSqr,LineCosSubtended,Falloff,NoL);
    }
    else
    {

    }

    return Falloff;
}

DirectLighting IntegrateBxDF(GBufferData GBuffer, half3 N, half3 V, CapsuleLight Capsule, ShadowTerms Shadow, bool bInverseSquared)
{
    DirectLighting DL = (DirectLighting)0;

    float NoL;
    float Falloff;
    float LineCosSubtended = 1;

    [branch]
    if(Capsule.Length > 0)
    {

    }
    else
    {
        float DistSqr = dot(Capsule.LightPos[0],Capsule.LightPos[0]);
        Falloff = rcp(DistSqr + Capsule.DistBiasSqr);//Calculates a fast, approximate, per-component reciprocal.

        float3 L = Capsule.LightPos[0] * rsqrt(DistSqr);
        NoL = dot(N,L);
    }

    if(Capsule.Radius > 0)
    {
        float SinAlphaSqr = saturate(Pow2(Capsule.Radius) * Falloff);
        NoL = SphereHorizonCosWrap(NoL,SinAlphaSqr);
    }

    NoL = saturate(NoL);
    Falloff = bInverseSquared ? Falloff : 1;

    float3 ToLight = Capsule.LightPos[0];
    if(Capsule.Length > 0)
    {
        float3 R = reflect(-V,N);

        ToLight = ClosestPointLineToRay(Capsule.LightPos[0],Capsule.LightPos[1],Capsule.Length,R);
    }

    float DistSqr = dot(ToLight,ToLight);
    float InvDist = rsqrt(DistSqr);
    float3 L = ToLight * ToLight;

    GBuffer.Roughness = max(GBuffer.Roughness,0.02);
    float a = Pow2(GBuffer.Roughness);

    AreaLight AL;
    AL.SphereSinAlpha = saturate( Capsule.Radius * InvDist * (1 - a) );
	AL.SphereSinAlphaSoft = saturate( Capsule.SoftRadius * InvDist );
	AL.LineCosSubtended = LineCosSubtended;
	AL.FalloffColor = 1;
	AL.Rect = (RectLight)0;
	AL.bIsRect = false;

    return IntegrateBxDF( GBuffer, N, V, L, Falloff, NoL, AL, Shadow );
}
#endif