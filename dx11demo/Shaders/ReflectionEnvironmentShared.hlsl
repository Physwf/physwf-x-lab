/** 
 * Computes sky diffuse lighting from the SH irradiance map.  
 * This has the SH basis evaluation and diffuse convolution weights combined for minimal ALU's - see "Stupid Spherical Harmonics (SH) Tricks" 
 */
float3 GetSkySHDiffuse(float3 Normal)
{
	float4 NormalVector = float4(Normal, 1);

	float3 Intermediate0, Intermediate1, Intermediate2;
	Intermediate0.x = dot(View.SkyIrradianceEnvironmentMap[0], NormalVector);
	Intermediate0.y = dot(View.SkyIrradianceEnvironmentMap[1], NormalVector);
	Intermediate0.z = dot(View.SkyIrradianceEnvironmentMap[2], NormalVector);

	float4 vB = NormalVector.xyzz * NormalVector.yzzx;
	Intermediate1.x = dot(View.SkyIrradianceEnvironmentMap[3], vB);
	Intermediate1.y = dot(View.SkyIrradianceEnvironmentMap[4], vB);
	Intermediate1.z = dot(View.SkyIrradianceEnvironmentMap[5], vB);

	float vC = NormalVector.x * NormalVector.x - NormalVector.y * NormalVector.y;
	Intermediate2 = View.SkyIrradianceEnvironmentMap[6].xyz * vC;

	// max to not get negative colors
	return max(0, Intermediate0 + Intermediate1 + Intermediate2);
}