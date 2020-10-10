
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

DirectLighting IntegrateBxDF(GBufferData GBuffer, half3 N, half3 V, CapsuleLight Capsule, ShadowTerms Shadow, bool InverseSquared)
{
    DirectLighting DL = (DirectLighting)0;
    return DL;
}
#endif