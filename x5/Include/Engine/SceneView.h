#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/EngineBaseTypes.h"
#include "RenderCore/RenderResource.h"
#include "RenderCore/UniformBuffer.h"
#include "Engine/World.h"
#include "SceneTypes.h"
#include "ShowFlags.h"
#include "ConvexVolume.h"
#include "Engine/GameViewportClient.h"
#include "SceneInterface.h"
#include "FinalPostProcessSettings.h"
#include "Renderer/GlobalDistanceFieldParameters.h"
#include "DebugViewModeHelpers.h"
#include "RenderCore/RendererInterface.h"

#include "X5.h"

class FForwardLightingViewResources;
class FSceneView;
class FSceneViewFamily;
class FSceneViewStateInterface;
class FViewElementDrawer;
class ISceneViewExtension;
class FSceneViewFamily;
class FVolumetricFogViewResources;

// Projection data for a FSceneView
struct FSceneViewProjectionData
{
	/** The view origin. */
	FVector ViewOrigin;

	/** Rotation matrix transforming from world space to view space. */
	FMatrix ViewRotationMatrix;

	/** UE4 projection matrix projects such that clip space Z=1 is the near plane, and Z=0 is the infinite far plane. */
	FMatrix ProjectionMatrix;

protected:
	//The unconstrained (no aspect ratio bars applied) view rectangle (also unscaled)
	FIntRect ViewRect;

	// The constrained view rectangle (identical to UnconstrainedUnscaledViewRect if aspect ratio is not constrained)
	FIntRect ConstrainedViewRect;

public:
	void SetViewRectangle(const FIntRect& InViewRect)
	{
		ViewRect = InViewRect;
		ConstrainedViewRect = InViewRect;
	}

	void SetConstrainedViewRectangle(const FIntRect& InViewRect)
	{
		ConstrainedViewRect = InViewRect;
	}

	bool IsValidViewRectangle() const
	{
		return (ConstrainedViewRect.Min.X >= 0) &&
			(ConstrainedViewRect.Min.Y >= 0) &&
			(ConstrainedViewRect.Width() > 0) &&
			(ConstrainedViewRect.Height() > 0);
	}

	bool IsPerspectiveProjection() const
	{
		return ProjectionMatrix.M[3][3] < 1.0f;
	}

	const FIntRect& GetViewRect() const { return ViewRect; }
	const FIntRect& GetConstrainedViewRect() const { return ConstrainedViewRect; }

	FMatrix ComputeViewProjectionMatrix() const
	{
		return FTranslationMatrix(-ViewOrigin) * ViewRotationMatrix * ProjectionMatrix;
	}
};

enum EMonoscopicFarFieldMode
{
	// Disabled
	Off = 0,

	// Enabled
	On = 1,

	// Render only the stereo views up to the far field clipping plane
	StereoOnly = 2,

	// Render only the stereo views, but without the far field clipping plane enabled.
	// This is useful for finding meshes that pass the culling test, but aren't 
	// actually visible in the stereo view and should be explicitly set to far field.
	// Like a sky box.
	StereoNoClipping = 3,

	// Render only the far field view behind the far field clipping plane
	MonoOnly = 4,
};

/** Method used for primary screen percentage method. */
enum class EPrimaryScreenPercentageMethod
{
	// Add spatial upscale pass at the end of post processing chain, before the secondary upscale.
	SpatialUpscale,

	// Let temporal AA's do the upscale.
	TemporalUpscale,

	// No upscaling or up sampling, just output the view rect smaller.
	// This is useful for VR's render thread dynamic resolution with MSAA.
	RawOutput,
};

/**
 * Method used for second screen percentage method, that is a second spatial upscale pass at the
 * very end, independent of screen percentage show flag.
 */
enum class ESecondaryScreenPercentageMethod
{
	// Helpful to work on aliasing issue on HighDPI monitors.
	NearestSpatialUpscale,

	// Upscale to simulate smaller pixel density on HighDPI monitors.
	LowerPixelDensitySimulation,

	// TODO: Same config as primary upscale?
};

// Parameters defining monoscopic far field VR rendering
struct FMonoscopicFarFieldParameters
{
	// Culling plane in unreal units between stereo and mono far field
	float CullingDistance;

	// Culling plane distance for stereo views in NDC depth [0:1]
	float StereoDepthClip;

	// Culling plane distance for the mono far field view in NDC depth [0:1]
	// This is the same the stereo depth clip, but with the overlap distance bias applied.
	float MonoDepthClip;

	// Stereo disparity lateral offset between a stereo view and the mono far field view at the culling plane distance for reprojection.
	float LateralOffset;

	// Distance to overlap the mono and stereo views in unreal units to handle precision artifacts
	float OverlapDistance;

	EMonoscopicFarFieldMode Mode;

	bool bEnabled;

	FMonoscopicFarFieldParameters() :
		CullingDistance(0.0f),
		StereoDepthClip(0.0f),
		MonoDepthClip(0.0f),
		LateralOffset(0.0f),
		OverlapDistance(50.0f),
		Mode(EMonoscopicFarFieldMode::Off),
		bEnabled(false)
	{
	}
};

struct FViewMatrices
{
	FViewMatrices()
	{

	}

private:
	FMatrix ProjectionMatrix;
	FMatrix ProjectionNoAAMatrix;
	FMatrix InvProjectionMatrix;
	FMatrix ViewMatrix;
	FMatrix InvViewMatrix;
	FMatrix ViewProjectionMatrix;
	FMatrix InvViewProjectionMatrix;
	FMatrix TranslatedViewMatrix;
	FMatrix InvTranslatedViewMatrix;
	//FMatrix OverriddenTranslatedViewMatrix;
	FVector PreViewTranslation;
	FVector ViewOrigin;
	FVector2D ProjectionScale;
	FVector2D TemporalAAProjectionJitter;
	float ScreenScale;

public:
	void UpdateViewMatrix(const FVector& ViewLocation, const FRotator& ViewRotation);

	inline const FMatrix& GetProjectionMatrix() const
	{
		return ProjectionMatrix;
	}
	inline const FMatrix& GetProjectionNoAAMatrix() const
	{
		return ProjectionNoAAMatrix;
	}
	inline const FMatrix& GetInvProjectionMatrix() const
	{
		return InvProjectionMatrix;
	}
	inline const FMatrix& GetViewMatrix() const
	{
		return ViewMatrix;
	}
	inline const FMatrix& GetInvViewMatrix() const
	{
		return InvViewMatrix;
	}
	inline const FMatrix& GetViewProjectionMatrix() const
	{
		return ViewProjectionMatrix;
	}
	inline const FMatrix& GetInvViewProjectionMatrix() const
	{
		return InvViewProjectionMatrix;
	}
	inline const FMatrix& GetTranslatedViewMatrix() const
	{
		return TranslatedViewMatrix;
	}
	inline const FMatrix& GetInvTranslatedViewMatrix() const
	{
		return InvTranslatedViewMatrix;
	}
	inline const FVector& GetPreViewTranslation() const
	{
		return PreViewTranslation;
	}
	inline const FVector& GetViewOrigin() const
	{
		return ViewOrigin;
	}
	inline float GetScreenScale() const
	{
		return ScreenScale;
	}
	inline const FVector2D& GetProjectionScale() const
	{
		return ProjectionScale;
	}
	inline bool IsPerspectiveProjection() const
	{
		return ProjectionMatrix.M[3][3] < 1.0f;
	}

	void SaveProjectionNoAAMatrix()
	{
		ProjectionNoAAMatrix = ProjectionMatrix;
	}

	inline const FVector2D GetTemporalAAJitter() const
	{
		return TemporalAAProjectionJitter;
	}
};

class FSceneView
{
public:
	FViewMatrices ViewMatrices;
};