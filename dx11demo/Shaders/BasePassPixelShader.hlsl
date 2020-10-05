//#include "SHCommon.hlsl"
#include "Material.hlsl"
#include "BasePassCommon.hlsl"
#include "LocalVertexFactory.hlsl"
// #include "LightmapCommon.ush"  
// #include "PlanarReflectionShared.ush"
// #include "BRDF.ush"
#include "Random.hlsl"
// #include "LightAccumulator.ush"
#include "DeferredShadingCommon.hlsl"
// #include "VelocityCommon.ush"
// #include "SphericalGaussian.ush"
// #include "DBufferDecalShared.ush"

#include "ShadingModelsMaterial.hlsl"

#if USES_GBUFFER

// The selective output mask can only depend on defines, since the shadow will not export the data.
uint GetSelectiveOutputMask()
{
	uint Mask = 0;
// #if !WRITES_CUSTOMDATA_TO_GBUFFER
// 	Mask |= SKIP_CUSTOMDATA_MASK;
// #endif
// #if !GBUFFER_HAS_PRECSHADOWFACTOR
// 	Mask |= SKIP_PRECSHADOW_MASK;
// #endif
// #if (GBUFFER_HAS_PRECSHADOWFACTOR && WRITES_PRECSHADOWFACTOR_ZERO)
// 	Mask |= ZERO_PRECSHADOW_MASK;
// #endif
// #if !WRITES_VELOCITY_TO_GBUFFER
// 	Mask |= SKIP_VELOCITY_MASK;
// #endif
	return Mask;
}
#endif // USES_GBUFFER

void FPixelShaderInOut_PS_Main(
	VertexFactoryInterpolantsVSToPS Interpolants,
	BasePassInterpolantsVSToPS BasePassInterpolants,
	in PixelShaderIn In, 
	inout PixelShaderOut Out)
{
    ResolvedView = ResolveView();

	// Velocity
	float4 OutVelocity = 0;
	// CustomData
	float4 OutGBufferD = 0;
	// PreShadowFactor
	float4 OutGBufferE = 0;

    MaterialPixelParameters MaterialParameters = GetMaterialPixelParameters(Interpolants, In.SvPosition);
    PixelMaterialInputs PMInputs;

// #if HQ_TEXTURE_LIGHTMAP && USES_AO_MATERIAL_MASK && !MATERIAL_SHADINGMODEL_UNLIT
// 	float2 LightmapUV0, LightmapUV1;
// 	GetLightMapCoordinates(Interpolants, LightmapUV0, LightmapUV1);
// 	// Must be computed before BaseColor, Normal, etc are evaluated
// 	MaterialParameters.AOMaterialMask = GetAOMaterialMask(LightmapUV0 * float2(1, 2));
// #endif

    //#if USE_WORLD_POSITION_EXCLUDING_SHADER_OFFSETS
		{
			float4 ScreenPosition = SvPositionToResolvedScreenPosition(In.SvPosition);
			float3 TranslatedWorldPosition = SvPositionToResolvedTranslatedWorld(In.SvPosition);
			CalcMaterialParametersEx(
										MaterialParameters, 
										PMInputs, 
										In.SvPosition, 
										ScreenPosition, 
										In.bIsFrontFace, 
										TranslatedWorldPosition, 
										BasePassInterpolants.PixelPositionExcludingWPO);
		}
	// #else
	// 	{
	// 		float4 ScreenPosition = SvPositionToResolvedScreenPosition(In.SvPosition);
	// 		float3 TranslatedWorldPosition = SvPositionToResolvedTranslatedWorld(In.SvPosition);
	// 		CalcMaterialParametersEx(MaterialParameters, PMInputs, In.SvPosition, ScreenPosition, In.bIsFrontFace, TranslatedWorldPosition, TranslatedWorldPosition);
	// 	}
	// #endif


    half3 BaseColor = GetMaterialBaseColor(PMInputs);
	half  Metallic = GetMaterialMetallic(PMInputs);
	half  Specular = GetMaterialSpecular(PMInputs);

	float MaterialAO = GetMaterialAmbientOcclusion(PMInputs);
	float Roughness = GetMaterialRoughness(PMInputs);


    half Opacity = GetMaterialOpacity(PMInputs);

	float SubsurfaceProfile = 0;
	float3 SubsurfaceColor = 0;

    GBufferData GBuffer = (GBufferData)0;

    GBuffer.GBufferAO = MaterialAO;
	GBuffer.PerObjectGBufferData = Primitive.PerObjectGBufferData;
	GBuffer.Depth = MaterialParameters.ScreenPosition.w;
    //see FPrecomputedLightingParameters
	//GBuffer.PrecomputedShadowFactors = GetPrecomputedShadowMasks(Interpolants, MaterialParameters.AbsoluteWorldPosition, VolumetricLightmapBrickTextureUVs);

    SetGBufferForShadingModel(
        GBuffer, 
        MaterialParameters, 
        Opacity, 
        BaseColor, 
        Metallic, 
        Specular, 
        Roughness, 
        SubsurfaceColor,
        SubsurfaceProfile);

#if USES_GBUFFER
	    GBuffer.SelectiveOutputMask = GetSelectiveOutputMask();

    // #if WRITES_VELOCITY_TO_GBUFFER
	// {
	// 	// 2d velocity, includes camera an object motion
    // #if WRITES_VELOCITY_TO_GBUFFER_USE_POS_INTERPOLATOR
	// 	float2 Velocity = Calculate2DVelocity(BasePassInterpolants.VelocityScreenPosition, BasePassInterpolants.VelocityPrevScreenPosition);
	// #else
	// 	float2 Velocity = Calculate2DVelocity(MaterialParameters.ScreenPosition, BasePassInterpolants.VelocityPrevScreenPosition);
	// #endif

	// 	// Make sure not to touch 0,0 which is clear color
	// 	GBuffer.Velocity = float4(EncodeVelocityToTexture(Velocity), 0, 0) * BasePassInterpolants.VelocityPrevScreenPosition.z;
	// }
	// #else
		GBuffer.Velocity = 0;
	//#endif
#endif
    //???
    GBuffer.SpecularColor = lerp(0.08 * Specular.xxx, BaseColor, Metallic.xxx);

// #if MATERIAL_NORMAL_CURVATURE_TO_ROUGHNESS

// #endif

    GBuffer.DiffuseColor = BaseColor - BaseColor * Metallic;

    float3 BentNormal = MaterialParameters.WorldNormal;
	float DiffOcclusion = MaterialAO;
	float SpecOcclusion = MaterialAO;
    //ApplyBentNormal( MaterialParameters, GBuffer.Roughness, BentNormal, DiffOcclusion, SpecOcclusion );
	// FIXME: ALLOW_STATIC_LIGHTING == 0 expects this to be AO
	//GBuffer.GBufferAO = AOMultiBounce( Luminance( GBuffer.SpecularColor ), SpecOcclusion ).g;
    half3 DiffuseColor = 0;
	half3 Color = 0;
	float IndirectIrradiance = 0;
/*
    #if !MATERIAL_SHADINGMODEL_UNLIT
        float3 DiffuseDir = BentNormal;
        float3 DiffuseColorForIndirect = GBuffer.DiffuseColor;

        float3 DiffuseIndirectLighting;
        float3 SubsurfaceIndirectLighting;
        GetPrecomputedIndirectLightingAndSkyLight(MaterialParameters, Interpolants, BasePassInterpolants, DiffuseDir, VolumetricLightmapBrickTextureUVs, DiffuseIndirectLighting, SubsurfaceIndirectLighting, IndirectIrradiance);

        float IndirectOcclusion = 1.0f;
        float2 NearestResolvedDepthScreenUV = 0;

        #if FORWARD_SHADING && (MATERIALBLENDING_SOLID || MATERIALBLENDING_MASKED)
        #endif

        DiffuseColor += (DiffuseIndirectLighting * DiffuseColorForIndirect + SubsurfaceIndirectLighting * SubsurfaceColor) * AOMultiBounce( GBuffer.BaseColor, DiffOcclusion );
*/
    // #if NEEDS_BASEPASS_PIXEL_VOLUMETRIC_FOGGING
    // #endif

    half3 Emissive = GetMaterialEmissive(PMInputs);


    #if USES_GBUFFER
		GBuffer.IndirectIrradiance = IndirectIrradiance;
        float QuantizationBias = PseudoRandom( MaterialParameters.SvPosition.xy ) - 0.5f;
		EncodeGBuffer(GBuffer, Out.MRT[1], Out.MRT[2], Out.MRT[3], OutGBufferD, OutGBufferE, OutVelocity, QuantizationBias);
    #endif 

    #if USES_GBUFFER
	#if GBUFFER_HAS_VELOCITY
		Out.MRT[4] = OutVelocity;
	#endif

	Out.MRT[GBUFFER_HAS_VELOCITY ? 5 : 4] = OutGBufferD;

	#if GBUFFER_HAS_PRECSHADOWFACTOR
		Out.MRT[GBUFFER_HAS_VELOCITY ? 6 : 5] = OutGBufferE;
	#endif


#endif
}

#define PIXELSHADEROUTPUT_MRT0 	!USES_GBUFFER
#define PIXELSHADEROUTPUT_MRT1  USES_GBUFFER
#define PIXELSHADEROUTPUT_MRT2  USES_GBUFFER
#define PIXELSHADEROUTPUT_MRT3  USES_GBUFFER


#ifndef PIXELSHADEROUTPUT_MRT0
	#define PIXELSHADEROUTPUT_MRT0 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT1
	#define PIXELSHADEROUTPUT_MRT1 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT2
	#define PIXELSHADEROUTPUT_MRT2 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT3
	#define PIXELSHADEROUTPUT_MRT3 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT4
	#define PIXELSHADEROUTPUT_MRT4 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT5
	#define PIXELSHADEROUTPUT_MRT5 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT6
	#define PIXELSHADEROUTPUT_MRT6 0
#endif
#ifndef PIXELSHADEROUTPUT_MRT7
	#define PIXELSHADEROUTPUT_MRT7 0
#endif

void PS_Main(
    VertexFactoryInterpolantsVSToPS Interpolants, 
    BasePassInterpolantsVSToPS BasePassInterpolants,
    in INPUT_POSITION_QUALIFIERS float4 SvPosition : SV_Position

#if PIXELSHADEROUTPUT_MRT0
	, out float4 OutTarget0 : SV_Target0
#endif

#if PIXELSHADEROUTPUT_MRT1
    , out float4 OutTarget1 : SV_Target1
#endif

#if PIXELSHADEROUTPUT_MRT2
    , out float4 OutTarget2 : SV_Target2
#endif

#if PIXELSHADEROUTPUT_MRT3
    , out float4 OutTarget3 : SV_Target3
#endif

#if PIXELSHADEROUTPUT_MRT4
    , out float4 OutTarget4 : SV_Target4
#endif

#if PIXELSHADEROUTPUT_MRT5
    , out float4 OutTarget5 : SV_Target5
#endif
    )
{
    PixelShaderIn PSIn = (PixelShaderIn)0;
    PixelShaderOut PSOut = (PixelShaderOut)0;

    FPixelShaderInOut_PS_Main(Interpolants, BasePassInterpolants, PSIn, PSOut);

#if PIXELSHADEROUTPUT_MRT0
	OutTarget0 = PSOut.MRT[0];
#endif

#if PIXELSHADEROUTPUT_MRT1
	OutTarget1 = PSOut.MRT[1];
#endif

#if PIXELSHADEROUTPUT_MRT2
	OutTarget2 = PSOut.MRT[2];
#endif

#if PIXELSHADEROUTPUT_MRT3
	OutTarget3 = PSOut.MRT[3];
#endif

#if PIXELSHADEROUTPUT_MRT4
	OutTarget4 = PSOut.MRT[4];
#endif

#if PIXELSHADEROUTPUT_MRT5
	OutTarget5 = PSOut.MRT[5];
#endif

#if PIXELSHADEROUTPUT_MRT6
	OutTarget6 = PSOut.MRT[6];
#endif

#if PIXELSHADEROUTPUT_MRT7
	OutTarget7 = PSOut.MRT[7];
#endif
}