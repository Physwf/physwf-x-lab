#include "DeferredShadingCommon.hlsl"
#include "BSDF.hlsl"
#include "FastMath.hlsl"
#include "CapsuleLight.hlsl"
#include "RectLight.hlsl"
//#include "TransmissionCommon.ush"


struct AreaLight
{
    float       SphereSinAlpha;
    float		SphereSinAlphaSoft;
    float		LineCosSubtended;

	float3		FalloffColor;

    RectLight	Rect;
	Texture2D	Texture;
	bool		bIsRect;
};

struct ShadowTerms
{
    float SurfaceShadow;
    float TransmissionShadow;
    float TransmissionThickness;
};

float New_a2( float a2, float SinAlpha, float VoH )
{
	return a2 + 0.25 * SinAlpha * (3.0 * sqrtFast(a2) + SinAlpha) / ( VoH + 0.001 );
	//return a2 + 0.25 * SinAlpha * ( saturate(12 * a2 + 0.125) + SinAlpha ) / ( VoH + 0.001 );
	//return a2 + 0.25 * SinAlpha * ( a2 * 2 + 1 + SinAlpha ) / ( VoH + 0.001 );
}

float EnergyNormalization( inout float a2, float VoH, AreaLight AL )
{
	if( AL.SphereSinAlphaSoft > 0 )
	{
		// Modify Roughness
		a2 = saturate( a2 + Pow2( AL.SphereSinAlphaSoft ) / ( VoH * 3.6 + 0.4 ) );
	}

	float Sphere_a2 = a2;
	float Energy = 1;
	if( AL.SphereSinAlpha > 0 )
	{
		Sphere_a2 = New_a2( a2, AL.SphereSinAlpha, VoH );
		Energy = a2 / Sphere_a2;
	}

	if( AL.LineCosSubtended < 1 )
	{
#if 1
		float LineCosTwoAlpha = AL.LineCosSubtended;
		float LineTanAlpha = sqrt( ( 1.0001 - LineCosTwoAlpha ) / ( 1 + LineCosTwoAlpha ) );
		float Line_a2 = New_a2( Sphere_a2, LineTanAlpha, VoH );
		Energy *= sqrt( Sphere_a2 / Line_a2 );
#else
		float LineCosTwoAlpha = AL.LineCosSubtended;
		float LineSinAlpha = sqrt( 0.5 - 0.5 * LineCosTwoAlpha );
		float Line_a2 = New_a2( Sphere_a2, LineSinAlpha, VoH );
		Energy *= Sphere_a2 / Line_a2;
#endif
	}

	return Energy;
}


float3 SpecularGGX( float Roughness, float3 SpecularColor, BxDFContext Context, float NoL, AreaLight AL )
{
	float a2 = Pow4( Roughness );
	float Energy = EnergyNormalization( a2, Context.VoH, AL );
	
	// Generalized microfacet specular
	float D = D_GGX( a2, Context.NoH ) * Energy;
	float Vis = Vis_SmithJointApprox( a2, Context.NoV, NoL );
	float3 F = F_Schlick( SpecularColor, Context.VoH );

	return (D * Vis) * F;
}

struct DirectLighting
{
    float3 Diffuse;
    float3 Specular;
    float3 Transmission;
};

DirectLighting DefaultLitBxDF( GBufferData GBuffer, half3 N, half3 V, half3 L, float Falloff, float NoL, AreaLight AL, ShadowTerms Shadow )
{
    BxDFContext Context;
	Init( Context, N, V, L );
	SphereMaxNoH( Context, AL.SphereSinAlpha, true );
	Context.NoV = saturate( abs( Context.NoV ) + 1e-5 );

    DirectLighting Lighting;
	Lighting.Diffuse  = AL.FalloffColor * (Falloff * NoL) * Diffuse_Lambert( GBuffer.DiffuseColor );

    if( AL.bIsRect )
		Lighting.Specular = RectGGXApproxLTC( GBuffer.Roughness, GBuffer.SpecularColor, N, V, AL.Rect, AL.Texture );
	else
		Lighting.Specular = AL.FalloffColor * (Falloff * NoL) * SpecularGGX( GBuffer.Roughness, GBuffer.SpecularColor, Context, NoL, AL );

	Lighting.Transmission = 0;
	return Lighting;
}


DirectLighting IntegrateBxDF(GBufferData GBuffer, half3 N, half3 V,  half3 L, float Falloff, float NoL, AreaLight AL, ShadowTerms Shadow)
{
    switch(GBuffer.ShadingModelID)
    {
        case SHADINGMODELID_DEFAULT_LIT:
            return DefaultLitBxDF(GBuffer,N,V,L,Falloff,NoL,AL,Shadow);
        default:
            return (DirectLighting)0;
    }
}