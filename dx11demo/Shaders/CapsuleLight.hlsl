
#ifndef __CapsuleLight_H__
#define __CapsuleLight_H__

struct CapsuleLight
{
	float3	LightPos[2];
	float	Length;
	float	Radius;
	float	SoftRadius;
	float	DistBiasSqr;
};


float3 LineIrradiace(float3 N, float3 Line0, float3 Line1, float DistanceBiasSqr, out float CosSubtended, out float BaseIrradiance, out float NoL)
{
    return float3(0,0,0);
}

#endif