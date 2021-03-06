#include "Scene.h"
#include "log.h"
#include "D3D11RHI.h"
#include "RenderTargets.h"
#include "AtmosphereRendering.h"

ViewInfo::ViewInfo(const ViewInitOptions& InitOptions)
	: SceneView(InitOptions),
	CachedViewUniformShaderParameters(NULL)
{

}

ViewInfo::ViewInfo(const SceneView* InView)
	: SceneView(*InView),
	CachedViewUniformShaderParameters(NULL)
{

}

ViewInfo::~ViewInfo()
{
	if(CachedViewUniformShaderParameters) 
		delete CachedViewUniformShaderParameters;
}

void ViewInfo::SetupUniformBufferParameters(
	RenderTargets& SceneContext, 
	const ViewMatrices& InViewMatrices, 
	const ViewMatrices& InPrevViewMatrices, 
	Box* OutTranslucentCascadeBoundsArray, 
	int32 NumTranslucentCascades, 
	ViewUniformShaderParameters& ViewUniformParameters) const
{
	//check(Family);

	// Create the view's uniform buffer.
	bool bIsMobileMultiViewEnabled = false;
	// Mobile multi-view is not side by side
	const IntRect EffectiveViewRect = (bIsMobileMultiViewEnabled) ? IntRect(0, 0, ViewRect.Width(), ViewRect.Height()) : ViewRect;

	// TODO: We should use a view and previous view uniform buffer to avoid code duplication and keep consistency
	SetupCommonViewUniformBufferParameters(
		ViewUniformParameters,
		SceneContext.GetBufferSizeXY(),
		SceneContext.GetMSAACount(),
		EffectiveViewRect,
		InViewMatrices,
		InPrevViewMatrices
	);

	//const bool bCheckerboardSubsurfaceRendering = FRCPassPostProcessSubsurface::RequiresCheckerboardSubsurfaceRendering(SceneContext.GetSceneColorFormat());
	//ViewUniformParameters.bCheckerboardSubsurfaceProfileRendering = bCheckerboardSubsurfaceRendering ? 1.0f : 0.0f;

	extern Scene* GScene;
	if (GScene)
	{
// 		if (Scene->SimpleDirectionalLight)
// 		{
// 			ViewUniformParameters.DirectionalLightColor = Scene->SimpleDirectionalLight->Proxy->GetColor() / PI;
// 			ViewUniformParameters.DirectionalLightDirection = -Scene->SimpleDirectionalLight->Proxy->GetDirection();
// 		}
// 		else
// 		{
// 			ViewUniformParameters.DirectionalLightColor = FLinearColor::Black;
// 			ViewUniformParameters.DirectionalLightDirection = FVector::ZeroVector;
// 		}

		// Atmospheric fog parameters
		if (/*ShouldRenderAtmosphere(*Family) &&*/ GScene->AtmosphericFog)
		{
			ViewUniformParameters.AtmosphericFogSunPower = GScene->AtmosphericFog->SunMultiplier;
			ViewUniformParameters.AtmosphericFogPower = GScene->AtmosphericFog->FogMultiplier;
			ViewUniformParameters.AtmosphericFogDensityScale = GScene->AtmosphericFog->InvDensityMultiplier;
			ViewUniformParameters.AtmosphericFogDensityOffset = GScene->AtmosphericFog->DensityOffset;
			ViewUniformParameters.AtmosphericFogGroundOffset = GScene->AtmosphericFog->GroundOffset;
			ViewUniformParameters.AtmosphericFogDistanceScale = GScene->AtmosphericFog->DistanceScale;
			ViewUniformParameters.AtmosphericFogAltitudeScale = GScene->AtmosphericFog->AltitudeScale;
			ViewUniformParameters.AtmosphericFogHeightScaleRayleigh = GScene->AtmosphericFog->RHeight;
			ViewUniformParameters.AtmosphericFogStartDistance = GScene->AtmosphericFog->StartDistance;
			ViewUniformParameters.AtmosphericFogDistanceOffset = GScene->AtmosphericFog->DistanceOffset;
			ViewUniformParameters.AtmosphericFogSunDiscScale = GScene->AtmosphericFog->SunDiscScale;
			ViewUniformParameters.AtmosphericFogSunColor = /*GScene->SunLight ? GScene->SunLight->Proxy->GetColor() :*/ GScene->AtmosphericFog->DefaultSunColor;
			ViewUniformParameters.AtmosphericFogSunDirection = /*GScene->SunLight ? -GScene->SunLight->Proxy->GetDirection() :*/ -GScene->AtmosphericFog->DefaultSunDirection;
			ViewUniformParameters.AtmosphericFogRenderMask = GScene->AtmosphericFog->RenderFlag /*& (EAtmosphereRenderFlag::E_DisableGroundScattering | EAtmosphereRenderFlag::E_DisableSunDisk)*/;
			ViewUniformParameters.AtmosphericFogInscatterAltitudeSampleNum = GScene->AtmosphericFog->InscatterAltitudeSampleNum;
		}
		else
		{
			ViewUniformParameters.AtmosphericFogSunPower = 0.f;
			ViewUniformParameters.AtmosphericFogPower = 0.f;
			ViewUniformParameters.AtmosphericFogDensityScale = 0.f;
			ViewUniformParameters.AtmosphericFogDensityOffset = 0.f;
			ViewUniformParameters.AtmosphericFogGroundOffset = 0.f;
			ViewUniformParameters.AtmosphericFogDistanceScale = 0.f;
			ViewUniformParameters.AtmosphericFogAltitudeScale = 0.f;
			ViewUniformParameters.AtmosphericFogHeightScaleRayleigh = 0.f;
			ViewUniformParameters.AtmosphericFogStartDistance = 0.f;
			ViewUniformParameters.AtmosphericFogDistanceOffset = 0.f;
			ViewUniformParameters.AtmosphericFogSunDiscScale = 1.f;
			//Added check so atmospheric light color and vector can use a directional light without needing an atmospheric fog actor in the scene
// 			ViewUniformParameters.AtmosphericFogSunColor = GScene->SunLight ? GScene->SunLight->Proxy->GetColor() : FLinearColor::Black;
// 			ViewUniformParameters.AtmosphericFogSunDirection = GScene->SunLight ? -GScene->SunLight->Proxy->GetDirection() : FVector::ZeroVector;
// 			ViewUniformParameters.AtmosphericFogRenderMask = EAtmosphereRenderFlag::E_EnableAll;
// 			ViewUniformParameters.AtmosphericFogInscatterAltitudeSampleNum = 0;
		}
	}
	else
	{
		// Atmospheric fog parameters
		ViewUniformParameters.AtmosphericFogSunPower = 0.f;
		ViewUniformParameters.AtmosphericFogPower = 0.f;
		ViewUniformParameters.AtmosphericFogDensityScale = 0.f;
		ViewUniformParameters.AtmosphericFogDensityOffset = 0.f;
		ViewUniformParameters.AtmosphericFogGroundOffset = 0.f;
		ViewUniformParameters.AtmosphericFogDistanceScale = 0.f;
		ViewUniformParameters.AtmosphericFogAltitudeScale = 0.f;
		ViewUniformParameters.AtmosphericFogHeightScaleRayleigh = 0.f;
		ViewUniformParameters.AtmosphericFogStartDistance = 0.f;
		ViewUniformParameters.AtmosphericFogDistanceOffset = 0.f;
		ViewUniformParameters.AtmosphericFogSunDiscScale = 1.f;
// 		ViewUniformParameters.AtmosphericFogSunColor = LinearColor::Black;
// 		ViewUniformParameters.AtmosphericFogSunDirection = Vector::ZeroVector;
// 		ViewUniformParameters.AtmosphericFogRenderMask = EAtmosphereRenderFlag::E_EnableAll;
// 		ViewUniformParameters.AtmosphericFogInscatterAltitudeSampleNum = 0;
	}

	//ViewUniformParameters.AtmosphereTransmittanceTexture = OrBlack2DIfNull(AtmosphereTransmittanceTexture);
	//ViewUniformParameters.AtmosphereIrradianceTexture = OrBlack2DIfNull(AtmosphereIrradianceTexture);
	//ViewUniformParameters.AtmosphereInscatterTexture = OrBlack3DIfNull(AtmosphereInscatterTexture);

	//ViewUniformParameters.AtmosphereTransmittanceTextureSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	//ViewUniformParameters.AtmosphereIrradianceTextureSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();
	//ViewUniformParameters.AtmosphereInscatterTextureSampler = TStaticSamplerState<SF_Bilinear>::GetRHI();

	// This should probably be in SetupCommonViewUniformBufferParameters, but drags in too many dependencies
	//UpdateNoiseTextureParameters(ViewUniformParameters);

	//SetupDefaultGlobalDistanceFieldUniformBufferParameters(ViewUniformParameters);

	//SetupVolumetricFogUniformBufferParameters(ViewUniformParameters);

	//SetupPrecomputedVolumetricLightmapUniformBufferParameters(Scene, ViewUniformParameters);

	// Setup view's shared sampler for material texture sampling.
// 	{
// 		const float GlobalMipBias = UTexture2D::GetGlobalMipMapLODBias();
// 
// 		float FinalMaterialTextureMipBias = GlobalMipBias;
// 
// 		if (bIsValidWorldTextureGroupSamplerFilter && PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale)
// 		{
// 			ViewUniformParameters.MaterialTextureMipBias = MaterialTextureMipBias;
// 			ViewUniformParameters.MaterialTextureDerivativeMultiply = FMath::Pow(2.0f, MaterialTextureMipBias);
// 
// 			FinalMaterialTextureMipBias += MaterialTextureMipBias;
// 		}
// 
// 		FSamplerStateRHIRef WrappedSampler = nullptr;
// 		FSamplerStateRHIRef ClampedSampler = nullptr;
// 
// 		if (FMath::Abs(FinalMaterialTextureMipBias - GlobalMipBias) < KINDA_SMALL_NUMBER)
// 		{
// 			WrappedSampler = Wrap_WorldGroupSettings->SamplerStateRHI;
// 			ClampedSampler = Clamp_WorldGroupSettings->SamplerStateRHI;
// 		}
// 		else if (ViewState && FMath::Abs(ViewState->MaterialTextureCachedMipBias - FinalMaterialTextureMipBias) < KINDA_SMALL_NUMBER)
// 		{
// 			WrappedSampler = ViewState->MaterialTextureBilinearWrapedSamplerCache;
// 			ClampedSampler = ViewState->MaterialTextureBilinearClampedSamplerCache;
// 		}
// 		else
// 		{
// 			check(bIsValidWorldTextureGroupSamplerFilter);
// 
// 			WrappedSampler = RHICreateSamplerState(FSamplerStateInitializerRHI(WorldTextureGroupSamplerFilter, AM_Wrap, AM_Wrap, AM_Wrap, FinalMaterialTextureMipBias));
// 			ClampedSampler = RHICreateSamplerState(FSamplerStateInitializerRHI(WorldTextureGroupSamplerFilter, AM_Clamp, AM_Clamp, AM_Clamp, FinalMaterialTextureMipBias));
// 		}
// 
// 		// At this point, a sampler must be set.
// 		check(WrappedSampler.IsValid());
// 		check(ClampedSampler.IsValid());
// 
// 		ViewUniformParameters.MaterialTextureBilinearWrapedSampler = WrappedSampler;
// 		ViewUniformParameters.MaterialTextureBilinearClampedSampler = ClampedSampler;
// 
// 		// Update view state's cached sampler.
// 		if (ViewState && ViewState->MaterialTextureBilinearWrapedSamplerCache != WrappedSampler)
// 		{
// 			ViewState->MaterialTextureCachedMipBias = FinalMaterialTextureMipBias;
// 			ViewState->MaterialTextureBilinearWrapedSamplerCache = WrappedSampler;
// 			ViewState->MaterialTextureBilinearClampedSamplerCache = ClampedSampler;
// 		}
// 	}

	uint32 StateFrameIndexMod8 = 0;

// 	if (State)
// 	{
// 		ViewUniformParameters.TemporalAAParams = FVector4(
// 			ViewState->GetCurrentTemporalAASampleIndex(),
// 			ViewState->GetCurrentTemporalAASampleCount(),
// 			TemporalJitterPixels.X,
// 			TemporalJitterPixels.Y);
// 
// 		StateFrameIndexMod8 = ViewState->GetFrameIndexMod8();
// 	}
// 	else
// 	{
// 		ViewUniformParameters.TemporalAAParams = FVector4(0, 1, 0, 0);
// 	}
// 
// 	ViewUniformParameters.StateFrameIndexMod8 = StateFrameIndexMod8;
// 
// 	{
// 		// If rendering in stereo, the other stereo passes uses the left eye's translucency lighting volume.
// 		const FViewInfo* PrimaryView = this;
// 		if (IStereoRendering::IsASecondaryView(StereoPass))
// 		{
// 			if (Family->Views.IsValidIndex(0))
// 			{
// 				const FSceneView* LeftEyeView = Family->Views[0];
// 				if (LeftEyeView->bIsViewInfo && LeftEyeView->StereoPass == eSSP_LEFT_EYE)
// 				{
// 					PrimaryView = static_cast<const FViewInfo*>(LeftEyeView);
// 				}
// 			}
// 		}
// 		PrimaryView->CalcTranslucencyLightingVolumeBounds(OutTranslucentCascadeBoundsArray, NumTranslucentCascades);
// 	}
// 
// 	for (int32 CascadeIndex = 0; CascadeIndex < NumTranslucentCascades; CascadeIndex++)
// 	{
// 		const float VolumeVoxelSize = (OutTranslucentCascadeBoundsArray[CascadeIndex].Max.X - OutTranslucentCascadeBoundsArray[CascadeIndex].Min.X) / GTranslucencyLightingVolumeDim;
// 		const FVector VolumeSize = OutTranslucentCascadeBoundsArray[CascadeIndex].Max - OutTranslucentCascadeBoundsArray[CascadeIndex].Min;
// 		ViewUniformParameters.TranslucencyLightingVolumeMin[CascadeIndex] = FVector4(OutTranslucentCascadeBoundsArray[CascadeIndex].Min, 1.0f / GTranslucencyLightingVolumeDim);
// 		ViewUniformParameters.TranslucencyLightingVolumeInvSize[CascadeIndex] = FVector4(FVector(1.0f) / VolumeSize, VolumeVoxelSize);
// 	}
// 
// 	ViewUniformParameters.PreExposure = PreExposure;
// 	ViewUniformParameters.OneOverPreExposure = 1.f / PreExposure;
// 
// 	ViewUniformParameters.DepthOfFieldFocalDistance = FinalPostProcessSettings.DepthOfFieldFocalDistance;
// 	ViewUniformParameters.DepthOfFieldSensorWidth = FinalPostProcessSettings.DepthOfFieldSensorWidth;
// 	ViewUniformParameters.DepthOfFieldFocalRegion = FinalPostProcessSettings.DepthOfFieldFocalRegion;
// 	// clamped to avoid div by 0 in shader
// 	ViewUniformParameters.DepthOfFieldNearTransitionRegion = FMath::Max(0.01f, FinalPostProcessSettings.DepthOfFieldNearTransitionRegion);
// 	// clamped to avoid div by 0 in shader
// 	ViewUniformParameters.DepthOfFieldFarTransitionRegion = FMath::Max(0.01f, FinalPostProcessSettings.DepthOfFieldFarTransitionRegion);
// 	ViewUniformParameters.DepthOfFieldScale = FinalPostProcessSettings.DepthOfFieldScale;
// 	ViewUniformParameters.DepthOfFieldFocalLength = 50.0f;
// 
// 	ViewUniformParameters.bSubsurfacePostprocessEnabled = GCompositionLighting.IsSubsurfacePostprocessRequired() ? 1.0f : 0.0f;
// 
// 	{
// 		// This is the CVar default
// 		float Value = 1.0f;
// 
// 		// Compiled out in SHIPPING to make cheating a bit harder.
// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// 		Value = CVarGeneralPurposeTweak.GetValueOnRenderThread();
// #endif
// 
// 		ViewUniformParameters.GeneralPurposeTweak = Value;
// 	}
// 
// 	ViewUniformParameters.DemosaicVposOffset = 0.0f;
// 	{
// 		ViewUniformParameters.DemosaicVposOffset = CVarDemosaicVposOffset.GetValueOnRenderThread();
// 	}
// 
// 	ViewUniformParameters.IndirectLightingColorScale = FVector(FinalPostProcessSettings.IndirectLightingColor.R * FinalPostProcessSettings.IndirectLightingIntensity,
// 		FinalPostProcessSettings.IndirectLightingColor.G * FinalPostProcessSettings.IndirectLightingIntensity,
// 		FinalPostProcessSettings.IndirectLightingColor.B * FinalPostProcessSettings.IndirectLightingIntensity);
// 
// 	ViewUniformParameters.NormalCurvatureToRoughnessScaleBias.X = FMath::Clamp(CVarNormalCurvatureToRoughnessScale.GetValueOnAnyThread(), 0.0f, 2.0f);
// 	ViewUniformParameters.NormalCurvatureToRoughnessScaleBias.Y = FMath::Clamp(CVarNormalCurvatureToRoughnessBias.GetValueOnAnyThread(), -1.0f, 1.0f);
// 	ViewUniformParameters.NormalCurvatureToRoughnessScaleBias.Z = FMath::Clamp(CVarNormalCurvatureToRoughnessExponent.GetValueOnAnyThread(), .05f, 20.0f);
// 
// 	ViewUniformParameters.RenderingReflectionCaptureMask = bIsReflectionCapture ? 1.0f : 0.0f;
// 
// 	ViewUniformParameters.AmbientCubemapTint = FinalPostProcessSettings.AmbientCubemapTint;
// 	ViewUniformParameters.AmbientCubemapIntensity = FinalPostProcessSettings.AmbientCubemapIntensity;
// 
// 	{
// 		// Enables HDR encoding mode selection without recompile of all PC shaders during ES2 emulation.
// 		ViewUniformParameters.HDR32bppEncodingMode = 0.0f;
// 		if (IsMobileHDR32bpp())
// 		{
// 			EMobileHDRMode MobileHDRMode = GetMobileHDRMode();
// 			switch (MobileHDRMode)
// 			{
// 			case EMobileHDRMode::EnabledMosaic:
// 				ViewUniformParameters.HDR32bppEncodingMode = 1.0f;
// 				break;
// 			case EMobileHDRMode::EnabledRGBE:
// 				ViewUniformParameters.HDR32bppEncodingMode = 2.0f;
// 				break;
// 			case EMobileHDRMode::EnabledRGBA8:
// 				ViewUniformParameters.HDR32bppEncodingMode = 3.0f;
// 				break;
// 			default:
// 				checkNoEntry();
// 				break;
// 			}
// 		}
// 	}
// 
// 	ViewUniformParameters.CircleDOFParams = CircleDofHalfCoc(*this);
// 
// 	if (Family->Scene)
// 	{
// 		Scene = Family->Scene->GetRenderScene();
// 	}
// 
// 
// 
// 	ERHIFeatureLevel::Type RHIFeatureLevel = Scene == nullptr ? GMaxRHIFeatureLevel : Scene->GetFeatureLevel();

	//if (GScene && GScene->SkyLight)
	{
// 		FSkyLightSceneProxy* SkyLight = Scene->SkyLight;
// 
// 		ViewUniformParameters.SkyLightColor = SkyLight->LightColor;
// 
// 		bool bApplyPrecomputedBentNormalShadowing =
// 			SkyLight->bCastShadows
// 			&& SkyLight->bWantsStaticShadowing;
// 
// 		ViewUniformParameters.SkyLightParameters = bApplyPrecomputedBentNormalShadowing ? 1 : 0;
		ViewUniformParameters.SkyLightColor = { 1.0f,1.0f, 1.0f };
		ViewUniformParameters.SkyLightParameters = 1.f;
	}
	//else
	{
		//ViewUniformParameters.SkyLightColor = FLinearColor::Black;
		//ViewUniformParameters.SkyLightParameters = 0;
	}

	// Make sure there's no padding since we're going to cast to FVector4*
	//checkSlow(sizeof(ViewUniformParameters.SkyIrradianceEnvironmentMap) == sizeof(FVector4) * 7);
	SetupSkyIrradianceEnvironmentMapConstants((Vector4*)&ViewUniformParameters.SkyIrradianceEnvironmentMap);

// 	ViewUniformParameters.MobilePreviewMode =
// 		(GIsEditor &&
// 		(RHIFeatureLevel == ERHIFeatureLevel::ES2 || RHIFeatureLevel == ERHIFeatureLevel::ES3_1) &&
// 			GMaxRHIFeatureLevel > ERHIFeatureLevel::ES3_1) ? 1.0f : 0.0f;

	// Padding between the left and right eye may be introduced by an HMD, which instanced stereo needs to account for.
// 	if ((Family != nullptr) && (StereoPass != eSSP_FULL) && (Family->Views.Num() > 1))
// 	{
// 		check(Family->Views.Num() >= 2);
// 
// 		// The static_cast<const FViewInfo*> is fine because when executing this method, we know that
// 		// Family::Views point to multiple FViewInfo, since of them is <this>.
// 		const float StereoViewportWidth = float(
// 			static_cast<const FViewInfo*>(Family->Views[1])->ViewRect.Max.X -
// 			static_cast<const FViewInfo*>(Family->Views[0])->ViewRect.Min.X);
// 		const float EyePaddingSize = float(
// 			static_cast<const FViewInfo*>(Family->Views[1])->ViewRect.Min.X -
// 			static_cast<const FViewInfo*>(Family->Views[0])->ViewRect.Max.X);
// 
// 		ViewUniformParameters.HMDEyePaddingOffset = (StereoViewportWidth - EyePaddingSize) / StereoViewportWidth;
// 	}
// 	else
// 	{
// 		ViewUniformParameters.HMDEyePaddingOffset = 1.0f;
// 	}
// 
// 	ViewUniformParameters.ReflectionCubemapMaxMip = FMath::FloorLog2(UReflectionCaptureComponent::GetReflectionCaptureSize());
// 
// 	ViewUniformParameters.ShowDecalsMask = Family->EngineShowFlags.Decals ? 1.0f : 0.0f;
// 
// 	extern int32 GDistanceFieldAOSpecularOcclusionMode;
// 	ViewUniformParameters.DistanceFieldAOSpecularOcclusionMode = GDistanceFieldAOSpecularOcclusionMode;
// 
// 	ViewUniformParameters.IndirectCapsuleSelfShadowingIntensity = Scene ? Scene->DynamicIndirectShadowsSelfShadowingIntensity : 1.0f;
// 
// 	extern FVector GetReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight();
// 	ViewUniformParameters.ReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight = GetReflectionEnvironmentRoughnessMixingScaleBiasAndLargestWeight();
// 
// 	ViewUniformParameters.StereoPassIndex = (StereoPass <= eSSP_LEFT_EYE) ? 0 : (StereoPass == eSSP_RIGHT_EYE) ? 1 : StereoPass - eSSP_MONOSCOPIC_EYE + 1;
// 	ViewUniformParameters.StereoIPD = StereoIPD;
// 
// 	ViewUniformParameters.PreIntegratedBRDF = GEngine->PreIntegratedSkinBRDFTexture->Resource->TextureRHI;
}
enum ETranslucencyVolumeCascade
{
	TVC_Inner,
	TVC_Outer,

	TVC_MAX,
};

void ViewInfo::InitRHIResources()
{
	Box VolumeBounds[TVC_MAX];

	CachedViewUniformShaderParameters = new ViewUniformShaderParameters();

	RenderTargets& SceneContext = RenderTargets::Get();

	SetupUniformBufferParameters(
		SceneContext,
		VolumeBounds,
		TVC_MAX,
		*CachedViewUniformShaderParameters);

	ViewUniformBuffer = CreateConstantBuffer(false, sizeof(ViewUniformShaderParameters), CachedViewUniformShaderParameters); //TUniformBufferRef<FViewUniformShaderParameters>::CreateUniformBufferImmediate(*CachedViewUniformShaderParameters, UniformBuffer_SingleFrame);
}

void ViewInfo::SetupSkyIrradianceEnvironmentMapConstants(Vector4* OutSkyIrradianceEnvironmentMap) const
{
	OutSkyIrradianceEnvironmentMap[0] = { 0.00155f, -0.0033f,  0.06505f,  0.14057f };
	OutSkyIrradianceEnvironmentMap[1] = { 0.00003f, -0.00304f,  0.09185f, 0.17386f };
	OutSkyIrradianceEnvironmentMap[2] = { -0.00289f, -0.00268f, 0.1612f,  0.25372f };
	OutSkyIrradianceEnvironmentMap[3] = { 0.00864f, -0.0024f,  -0.05994f, -0.00767f };
	OutSkyIrradianceEnvironmentMap[4] = { 0.00792f, -0.00237f, -0.06537f, -0.00857f };
	OutSkyIrradianceEnvironmentMap[5] = { 0.00666f, -0.00231f, -0.07398f, -0.01038f };
	OutSkyIrradianceEnvironmentMap[6] = { -0.00346f, -0.00294f, -0.00201f, 1.00f };
}
void Scene::InitScene()
{
	/*
	//Matrix::DXFromPerspectiveFovLH(3.1415926f / 2, 1.0, 1.0f, 10000.f);
	Matrix ProjectionMatrix = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 3.f, (float)WindowWidth / WindowHeight, ViewZNear, ViewZFar);
	//Matrix ProjectionMatrix = ReversedZPerspectiveMatrix(3.1415926f / 2.f, 3.1415926f / 2.f, 1.0f, 1.0f, 1.0,1.0f);
	Matrix ViewRotationMatrix = Matrix::DXLookAtLH(Vector(0, 0, 0), MainCamera.FaceDir, MainCamera.Up);
	VMs = ViewMatrices(MainCamera.Eye, ViewRotationMatrix, ProjectionMatrix);
	VU.ViewToTranslatedWorld = VMs.GetOverriddenInvTranslatedViewMatrix();
	VU.TranslatedWorldToClip = ProjectionMatrix;// VMs.GetTranslatedViewProjectionMatrix();
	VU.WorldToClip = VMs.GetViewProjectionMatrix();
	VU.TranslatedWorldToView = VMs.GetOverriddenTranslatedViewMatrix();
	//VU.TranslatedWorldToView = 
	VU.TranslatedWorldToCameraView = VMs.GetTranslatedViewMatrix();
	VU.CameraViewToTranslatedWorld = VMs.GetInvTranslatedViewMatrix();
	VU.ViewToClip = VMs.GetProjectionMatrix();
	VU.ClipToView = VMs.GetInvProjectionMatrix();
	VU.ClipToTranslatedWorld = VMs.GetInvTranslatedViewProjectionMatrix();
	VU.ScreenToTranslatedWorld = Matrix(
		Plane(1, 0, 0, 0),
		Plane(0, 1, 0, 0),
		Plane(0, 0, VMs.GetProjectionMatrix().M[2][2], 1),
		Plane(0, 0, VMs.GetProjectionMatrix().M[3][2], 0))
		* VMs.GetInvTranslatedViewProjectionMatrix();
	VU.PreViewTranslation = VMs.GetPreViewTranslation();

	VU.WorldCameraOrigin = VMs.GetViewOrigin();
	VU.InvDeviceZToWorldZTransform = CreateInvDeviceZToWorldZTransform(VMs.GetProjectionMatrix());

	ViewUniformBuffer = CreateConstantBuffer(false, sizeof(VU), &VU);

	const float InvBufferSizeX = 1.0f / WindowWidth;
	const float InvBufferSizeY = 1.0f / WindowHeight;

	//Vector4 EffectiveViewRect(0,0,WindowWidth,WindowHeight);

	float GProjectionSignY = 1.0f;
	VU.ScreenPositionScaleBias = Vector4(
		WindowWidth * InvBufferSizeX / +2.0f,
		WindowHeight * InvBufferSizeY / (-2.0f * GProjectionSignY),
		(WindowHeight / 2.0f + 0.f) * InvBufferSizeY,
		(WindowWidth / 2.0f + 0.f) * InvBufferSizeX
	);
	VU.ViewRectMin = Vector4();
	VU.ViewSizeAndInvSize = Vector4((float)WindowWidth, (float)WindowHeight, 1.0f / WindowWidth, 1.0f / WindowHeight);
	VU.BufferSizeAndInvSize = Vector4((float)WindowWidth, (float)WindowHeight, 1.0f / WindowWidth, 1.0f / WindowHeight);
	VU.ScreenToWorld = VMs.GetInvViewProjectionMatrix();
	VU.IndirectLightingColorScale = Vector(1.0f, 1.0f, 1.0f); //SceneRendering.cpp void FViewInfo::SetupUniformBufferParameters(

	VU.TranslatedWorldToClip.Transpose();
	VU.ViewToTranslatedWorld.Transpose();
	VU.WorldToClip.Transpose();
	VU.TranslatedWorldToView.Transpose();
	VU.ViewToTranslatedWorld.Transpose();
	VU.TranslatedWorldToCameraView.Transpose();
	VU.CameraViewToTranslatedWorld.Transpose();
	VU.ViewToClip.Transpose();
	VU.ViewToClipNoAA.Transpose();
	VU.ClipToView.Transpose();
	VU.ClipToTranslatedWorld.Transpose();
	VU.SVPositionToTranslatedWorld.Transpose();
	VU.ScreenToWorld.Transpose();
	VU.ScreenToTranslatedWorld.Transpose();
	*/
	AtmosphericFog = new AtmosphericFogSceneInfo(this);

}

void Scene::AddMesh(Mesh* InMesh)
{
	InMesh->DrawStaticElement(this);
}

void UpdateView()
{
	/*
	Matrix ProjectionMatrix = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 3.f, (float)WindowWidth / WindowHeight, ViewZNear, ViewZFar);
	Matrix ViewRotationMatrix = Matrix::DXLookAtLH(Vector(0, 0, 0), MainCamera.FaceDir, MainCamera.Up);

	ViewMatrices VMs(MainCamera.Eye, ViewRotationMatrix, ProjectionMatrix);
	VU.ViewToTranslatedWorld = VMs.GetOverriddenInvTranslatedViewMatrix();
	VU.TranslatedWorldToClip = VMs.GetTranslatedViewProjectionMatrix();
	VU.WorldToClip = VMs.GetViewProjectionMatrix();
	VU.TranslatedWorldToView = VMs.GetOverriddenTranslatedViewMatrix();
	//VU.TranslatedWorldToView = 
	VU.TranslatedWorldToCameraView = VMs.GetTranslatedViewMatrix();
	VU.CameraViewToTranslatedWorld = VMs.GetInvTranslatedViewMatrix();
	VU.ViewToClip = VMs.GetProjectionMatrix();
	VU.ClipToView = VMs.GetInvProjectionMatrix();
	VU.ClipToTranslatedWorld = VMs.GetInvTranslatedViewProjectionMatrix();
	VU.ScreenToTranslatedWorld = Matrix(
		Plane(1, 0, 0, 0),
		Plane(0, 1, 0, 0),
		Plane(0, 0, VMs.GetProjectionMatrix().M[2][2], 1),
		Plane(0, 0, VMs.GetProjectionMatrix().M[3][2], 0))
		* VMs.GetInvTranslatedViewProjectionMatrix();
	VU.PreViewTranslation = VMs.GetPreViewTranslation();

	VU.WorldCameraOrigin = VMs.GetViewOrigin();
	VU.InvDeviceZToWorldZTransform = CreateInvDeviceZToWorldZTransform(VMs.GetProjectionMatrix());

	const float InvBufferSizeX = 1.0f / WindowWidth;
	const float InvBufferSizeY = 1.0f / WindowHeight;

	//Vector4 EffectiveViewRect(0,0,WindowWidth,WindowHeight);

	float GProjectionSignY = 1.0f;
	VU.ScreenPositionScaleBias = Vector4(
		WindowWidth * InvBufferSizeX / +2.0f,
		WindowHeight * InvBufferSizeY / (-2.0f * GProjectionSignY),
		(WindowHeight / 2.0f + 0.f) * InvBufferSizeY,
		(WindowWidth / 2.0f + 0.f) * InvBufferSizeX
	);
	VU.ViewRectMin = Vector4();
	VU.ViewSizeAndInvSize = Vector4((float)WindowWidth, (float)WindowHeight, 1.0f / WindowWidth, 1.0f / WindowHeight);
	VU.BufferSizeAndInvSize = Vector4((float)WindowWidth, (float)WindowHeight, 1.0f / WindowWidth, 1.0f / WindowHeight);
	VU.ScreenToWorld = VMs.GetInvViewProjectionMatrix();

	VU.TranslatedWorldToClip.Transpose();
	VU.ViewToTranslatedWorld.Transpose();
	VU.WorldToClip.Transpose();
	VU.TranslatedWorldToView.Transpose();
	VU.ViewToTranslatedWorld.Transpose();
	VU.TranslatedWorldToCameraView.Transpose();
	VU.CameraViewToTranslatedWorld.Transpose();
	VU.ViewToClip.Transpose();
	VU.ViewToClipNoAA.Transpose();
	VU.ClipToView.Transpose();
	VU.ClipToTranslatedWorld.Transpose();
	VU.SVPositionToTranslatedWorld.Transpose();
	VU.ScreenToWorld.Transpose();
	VU.ScreenToTranslatedWorld.Transpose();


	D3D11DeviceContext->UpdateSubresource(ViewUniformBuffer, 0, NULL, &VU, 0, 0);
	*/
}

