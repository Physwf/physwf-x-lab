#include "SceneView.h"

/** The minimum Z value in clip space for the RHI. */
float GMinClipZ = 0.f;
/** The sign to apply to the Y axis of projection matrices. */
float GProjectionSignY = 1.f;

inline Matrix AdjustProjectionMatrixForRHI(const Matrix& InProjectionMatrix)
{
	ScaleMatrix ClipSpaceFixScale(Vector(1.0f, GProjectionSignY, 1.0f - GMinClipZ));
	TranslationMatrix ClipSpaceFixTranslate(Vector(0.0f, 0.0f, GMinClipZ));
	return InProjectionMatrix * ClipSpaceFixScale * ClipSpaceFixTranslate;
}

ViewMatrices::ViewMatrices(const ViewInitOptions& InitOptions)
{
	Vector LocalViewOrigin = InitOptions.ViewOrigin;
	Matrix ViewRotationMatrix = InitOptions.ViewRotationMatrix;
	if (!ViewRotationMatrix.GetOrigin().IsNearlyZero(0.0f))
	{
		LocalViewOrigin += ViewRotationMatrix.InverseTransformPosition(Vector::ZeroVector);
		ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();
	}

	ViewMatrix = TranslationMatrix(-LocalViewOrigin) * ViewRotationMatrix;

	// Adjust the projection matrix for the current RHI.
	ProjectionMatrix = AdjustProjectionMatrixForRHI(InitOptions.ProjectionMatrix);
	InvProjectionMatrix = InvertProjectionMatrix(ProjectionMatrix);

	// Compute the view projection matrix and its inverse.
	ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

	// For precision reasons the view matrix inverse is calculated independently.
	InvViewMatrix = ViewRotationMatrix.GetTransposed() * TranslationMatrix(LocalViewOrigin);
	InvViewProjectionMatrix = InvProjectionMatrix * InvViewMatrix;

	bool bApplyPreViewTranslation = true;
	bool bViewOriginIsFudged = false;

	// Calculate the view origin from the view/projection matrices.
	if (IsPerspectiveProjection())
	{
		this->ViewOrigin = LocalViewOrigin;
	}
	else
	{
		this->ViewOrigin = Vector4(InvViewMatrix.TransformVector(Vector(0, 0, -1)).GetSafeNormal(), 0);
		// to avoid issues with view dependent effect (e.g. Frensel)
		bApplyPreViewTranslation = false;
	}

	/** The view transform, starting from world-space points translated by -ViewOrigin. */
	Matrix LocalTranslatedViewMatrix = ViewRotationMatrix;
	Matrix LocalInvTranslatedViewMatrix = LocalTranslatedViewMatrix.GetTransposed();

	// Translate world-space so its origin is at ViewOrigin for improved precision.
	// Note that this isn't exactly right for orthogonal projections (See the above special case), but we still use ViewOrigin
	// in that case so the same value may be used in shaders for both the world-space translation and the camera's world position.
	if (bApplyPreViewTranslation)
	{
		PreViewTranslation = -Vector(LocalViewOrigin);
	}
	else
	{
		// If not applying PreViewTranslation then we need to use the view matrix directly.
		LocalTranslatedViewMatrix = ViewMatrix;
		LocalInvTranslatedViewMatrix = InvViewMatrix;
	}

	// When the view origin is fudged for faux ortho view position the translations don't cancel out.
	if (bViewOriginIsFudged)
	{
		LocalTranslatedViewMatrix = TranslationMatrix(-PreViewTranslation)
			* TranslationMatrix(-LocalViewOrigin) * ViewRotationMatrix;
		LocalInvTranslatedViewMatrix = LocalTranslatedViewMatrix.Inverse();
	}

	// Compute a transform from view origin centered world-space to clip space.
	TranslatedViewMatrix = LocalTranslatedViewMatrix;
	InvTranslatedViewMatrix = LocalInvTranslatedViewMatrix;

	OverriddenTranslatedViewMatrix = TranslationMatrix(-GetPreViewTranslation()) * GetViewMatrix();
	OverriddenInvTranslatedViewMatrix = GetInvViewMatrix() * TranslationMatrix(GetPreViewTranslation());

	TranslatedViewProjectionMatrix = LocalTranslatedViewMatrix * ProjectionMatrix;
	InvTranslatedViewProjectionMatrix = InvProjectionMatrix * LocalInvTranslatedViewMatrix;

	// Compute screen scale factors.
	// Stereo renders at half horizontal resolution, but compute shadow resolution based on full resolution.
// 	const float ScreenXScale = bStereo ? 2.0f : 1.0f;
// 	ProjectionScale.X = ScreenXScale * FMath::Abs(ProjectionMatrix.M[0][0]);
// 	ProjectionScale.Y = FMath::Abs(ProjectionMatrix.M[1][1]);
// 	ScreenScale = FMath::Max(
// 		InitOptions.GetConstrainedViewRect().Size().X * 0.5f * ProjectionScale.X,
// 		InitOptions.GetConstrainedViewRect().Size().Y * 0.5f * ProjectionScale.Y
//	);
}

Vector4 CreateInvDeviceZToWorldZTransform(const Matrix& ProjMatrix)
{
	/*
	00, 01, 02, 03
	10, 11, 12, 13
	20, 21, 22, 23
	30, 31, 32, 33
	*/
	float DepthMul = ProjMatrix.M[2][2];
	float DepthAdd = ProjMatrix.M[3][2];

	bool bIsPerspectiveProjection = ProjMatrix.M[3][3] < 1.0f;

	if (bIsPerspectiveProjection)
	{
		float SubtractValue = DepthMul / DepthAdd;

		// Subtract a tiny number to avoid divide by 0 errors in the shader when a very far distance is decided from the depth buffer.
		// This fixes fog not being applied to the black background in the editor.
		SubtractValue -= 0.00000001f;

		return Vector4(
			0.0f,
			0.0f,
			1.0f / DepthAdd,
			SubtractValue
		);
	}
	else
	{
		return Vector4(
			1.0f / ProjMatrix.M[2][2],
			-ProjMatrix.M[3][2] / ProjMatrix.M[2][2] + 1.0f,
			0.0f,
			1.0f
		);
	}
}


SceneView::SceneView(const ViewInitOptions& InitOptions)
: mViewMatrices(InitOptions),
	ViewActor(InitOptions.ViewActor),
	UnscaledViewRect(InitOptions.GetConstrainedViewRect()),
	UnconstrainedViewRect(InitOptions.GetViewRect()),
	ProjectionMatrixUnadjustedForRHI(InitOptions.ProjectionMatrix)

{
	InvDeviceZToWorldZTransform = CreateInvDeviceZToWorldZTransform(ProjectionMatrixUnadjustedForRHI);
}

void SceneView::SetupViewRectUniformBufferParameters(
	ViewUniformShaderParameters& ViewUniformParameters,
	const IntPoint& BufferSize,
	const IntRect& EffectiveViewRect,
	const ViewMatrices& InViewMatrices,
	const ViewMatrices& InPrevViewMatrices) const
{
	//checkfSlow(EffectiveViewRect.Area() > 0, TEXT("Invalid-size EffectiveViewRect passed to CreateUniformBufferParameters [%d * %d]."), EffectiveViewRect.Width(), EffectiveViewRect.Height());

	// Calculate the vector used by shaders to convert clip space coordinates to texture space.
	const float InvBufferSizeX = 1.0f / BufferSize.X;
	const float InvBufferSizeY = 1.0f / BufferSize.Y;
	// to bring NDC (-1..1, 1..-1) into 0..1 UV for BufferSize textures
	const Vector4 ScreenPositionScaleBias(
		EffectiveViewRect.Width() * InvBufferSizeX / +2.0f,
		EffectiveViewRect.Height() * InvBufferSizeY / (-2.0f * GProjectionSignY),
		(EffectiveViewRect.Height() / 2.0f + EffectiveViewRect.Min.Y) * InvBufferSizeY,
		(EffectiveViewRect.Width() / 2.0f + EffectiveViewRect.Min.X) * InvBufferSizeX
	);

	ViewUniformParameters.ScreenPositionScaleBias = ScreenPositionScaleBias;

	ViewUniformParameters.ViewRectMin = Vector4(EffectiveViewRect.Min.X, EffectiveViewRect.Min.Y, 0.0f, 0.0f);
	ViewUniformParameters.ViewSizeAndInvSize = Vector4(EffectiveViewRect.Width(), EffectiveViewRect.Height(), 1.0f / float(EffectiveViewRect.Width()), 1.0f / float(EffectiveViewRect.Height()));
	ViewUniformParameters.BufferSizeAndInvSize = Vector4(BufferSize.X, BufferSize.Y, InvBufferSizeX, InvBufferSizeY);
	ViewUniformParameters.BufferBilinearUVMinMax = Vector4(
		InvBufferSizeX * (EffectiveViewRect.Min.X + 0.5),
		InvBufferSizeY * (EffectiveViewRect.Min.Y + 0.5),
		InvBufferSizeX * (EffectiveViewRect.Max.X - 0.5),
		InvBufferSizeY * (EffectiveViewRect.Max.Y - 0.5));

	//ViewUniformParameters.MotionBlurNormalizedToPixel = FinalPostProcessSettings.MotionBlurMax * EffectiveViewRect.Width() / 100.0f;

	{
		// setup a matrix to transform float4(SvPosition.xyz,1) directly to TranslatedWorld (quality, performance as we don't need to convert or use interpolator)

		//	new_xy = (xy - ViewRectMin.xy) * ViewSizeAndInvSize.zw * float2(2,-2) + float2(-1, 1);

		//  transformed into one MAD:  new_xy = xy * ViewSizeAndInvSize.zw * float2(2,-2)      +       (-ViewRectMin.xy) * ViewSizeAndInvSize.zw * float2(2,-2) + float2(-1, 1);

		float Mx = 2.0f * ViewUniformParameters.ViewSizeAndInvSize.Z;
		float My = -2.0f * ViewUniformParameters.ViewSizeAndInvSize.W;
		float Ax = -1.0f - 2.0f * EffectiveViewRect.Min.X * ViewUniformParameters.ViewSizeAndInvSize.Z;
		float Ay = 1.0f + 2.0f * EffectiveViewRect.Min.Y * ViewUniformParameters.ViewSizeAndInvSize.W;

		// http://stackoverflow.com/questions/9010546/java-transformation-matrix-operations

		ViewUniformParameters.SVPositionToTranslatedWorld =
			Matrix(Plane(Mx, 0, 0, 0),
				Plane(0, My, 0, 0),
				Plane(0, 0, 1, 0),
				Plane(Ax, Ay, 0, 1)) * InViewMatrices.GetInvTranslatedViewProjectionMatrix();
	}

	// is getting clamped in the shader to a value larger than 0 (we don't want the triangles to disappear)
	//ViewUniformParameters.AdaptiveTessellationFactor = 0.0f;

// 	if (Family->EngineShowFlags.Tessellation)
// 	{
// 		// CVar setting is pixels/tri which is nice and intuitive.  But we want pixels/tessellated edge.  So use a heuristic.
// 		float TessellationAdaptivePixelsPerEdge = FMath::Sqrt(2.f * CVarTessellationAdaptivePixelsPerTriangle.GetValueOnRenderThread());
// 
// 		ViewUniformParameters.AdaptiveTessellationFactor = 0.5f * InViewMatrices.GetProjectionMatrix().M[1][1] * float(EffectiveViewRect.Height()) / TessellationAdaptivePixelsPerEdge;
// 	}

}

void SceneView::SetupCommonViewUniformBufferParameters(
	ViewUniformShaderParameters& ViewUniformParameters, 
	const IntPoint& BufferSize, 
	int32 NumMSAASamples, 
	const IntRect& EffectiveViewRect, 
	const ViewMatrices& InViewMatrices, 
	const ViewMatrices& InPrevViewMatrices) const
{
	//ViewUniformParameters.NumSceneColorMSAASamples = NumMSAASamples;
	ViewUniformParameters.ViewToTranslatedWorld = InViewMatrices.GetOverriddenInvTranslatedViewMatrix();
	ViewUniformParameters.TranslatedWorldToClip = InViewMatrices.GetTranslatedViewProjectionMatrix();
	ViewUniformParameters.WorldToClip = InViewMatrices.GetViewProjectionMatrix();
	ViewUniformParameters.TranslatedWorldToView = InViewMatrices.GetOverriddenTranslatedViewMatrix();
	ViewUniformParameters.TranslatedWorldToCameraView = InViewMatrices.GetTranslatedViewMatrix();
	ViewUniformParameters.CameraViewToTranslatedWorld = InViewMatrices.GetInvTranslatedViewMatrix();
	ViewUniformParameters.ViewToClip = InViewMatrices.GetProjectionMatrix();
	//ViewUniformParameters.ViewToClipNoAA = InViewMatrices.GetProjectionNoAAMatrix();
	ViewUniformParameters.ClipToView = InViewMatrices.GetInvProjectionMatrix();
	ViewUniformParameters.ClipToTranslatedWorld = InViewMatrices.GetInvTranslatedViewProjectionMatrix();
	//ViewUniformParameters.ViewForward = InViewMatrices.GetOverriddenTranslatedViewMatrix().GetColumn(2);
	//ViewUniformParameters.ViewUp = InViewMatrices.GetOverriddenTranslatedViewMatrix().GetColumn(1);
	//ViewUniformParameters.ViewRight = InViewMatrices.GetOverriddenTranslatedViewMatrix().GetColumn(0);
	//ViewUniformParameters.HMDViewNoRollUp = InViewMatrices.GetHMDViewMatrixNoRoll().GetColumn(1);
	//ViewUniformParameters.HMDViewNoRollRight = InViewMatrices.GetHMDViewMatrixNoRoll().GetColumn(0);
	ViewUniformParameters.InvDeviceZToWorldZTransform = InvDeviceZToWorldZTransform;
	ViewUniformParameters.WorldViewOrigin = InViewMatrices.GetOverriddenInvTranslatedViewMatrix().TransformPosition(Vector(0)) - InViewMatrices.GetPreViewTranslation();
	ViewUniformParameters.WorldCameraOrigin = InViewMatrices.GetViewOrigin();
	ViewUniformParameters.TranslatedWorldCameraOrigin = InViewMatrices.GetViewOrigin() + InViewMatrices.GetPreViewTranslation();
	ViewUniformParameters.PreViewTranslation = InViewMatrices.GetPreViewTranslation();
	//ViewUniformParameters.PrevProjection = InPrevViewMatrices.GetProjectionMatrix();
	//ViewUniformParameters.PrevViewProj = InPrevViewMatrices.GetViewProjectionMatrix();
	//ViewUniformParameters.PrevViewRotationProj = InPrevViewMatrices.ComputeViewRotationProjectionMatrix();
	//ViewUniformParameters.PrevViewToClip = InPrevViewMatrices.GetProjectionMatrix();
	//ViewUniformParameters.PrevClipToView = InPrevViewMatrices.GetInvProjectionMatrix();
	//ViewUniformParameters.PrevTranslatedWorldToClip = InPrevViewMatrices.GetTranslatedViewProjectionMatrix();
	// EffectiveTranslatedViewMatrix != InViewMatrices.TranslatedViewMatrix in the shadow pass
	// and we don't have EffectiveTranslatedViewMatrix for the previous frame to set up PrevTranslatedWorldToView
	// but that is fine to set up PrevTranslatedWorldToView as same as PrevTranslatedWorldToCameraView
	// since the shadow pass doesn't require previous frame computation.
	//ViewUniformParameters.PrevTranslatedWorldToView = InPrevViewMatrices.GetTranslatedViewMatrix();
	//ViewUniformParameters.PrevViewToTranslatedWorld = InPrevViewMatrices.GetInvTranslatedViewMatrix();
	//ViewUniformParameters.PrevTranslatedWorldToCameraView = InPrevViewMatrices.GetTranslatedViewMatrix();
	//ViewUniformParameters.PrevCameraViewToTranslatedWorld = InPrevViewMatrices.GetInvTranslatedViewMatrix();
	//ViewUniformParameters.PrevWorldCameraOrigin = InPrevViewMatrices.GetViewOrigin();
	// previous view world origin is going to be needed only in the base pass or shadow pass
	// therefore is same as previous camera world origin.
	//ViewUniformParameters.PrevWorldViewOrigin = ViewUniformShaderParameters.PrevWorldCameraOrigin;
	//ViewUniformParameters.PrevPreViewTranslation = InPrevViewMatrices.GetPreViewTranslation();
	// can be optimized
	//ViewUniformParameters.PrevInvViewProj = InPrevViewMatrices.GetInvViewProjectionMatrix();
	//ViewUniformParameters.GlobalClippingPlane = FVector4(GlobalClippingPlane.X, GlobalClippingPlane.Y, GlobalClippingPlane.Z, -GlobalClippingPlane.W);

// 	ViewUniformParameters.FieldOfViewWideAngles = 2.f * InViewMatrices.ComputeHalfFieldOfViewPerAxis();
// 	ViewUniformParameters.PrevFieldOfViewWideAngles = 2.f * InPrevViewMatrices.ComputeHalfFieldOfViewPerAxis();
// 	ViewUniformParameters.DiffuseOverrideParameter = LocalDiffuseOverrideParameter;
// 	ViewUniformParameters.SpecularOverrideParameter = SpecularOverrideParameter;
// 	ViewUniformParameters.NormalOverrideParameter = NormalOverrideParameter;
// 	ViewUniformParameters.RoughnessOverrideParameter = LocalRoughnessOverrideParameter;
// 	ViewUniformParameters.PrevFrameGameTime = Family->CurrentWorldTime - Family->DeltaWorldTime;
// 	ViewUniformParameters.PrevFrameRealTime = Family->CurrentRealTime - Family->DeltaWorldTime;
// 	ViewUniformParameters.WorldCameraMovementSinceLastFrame = InViewMatrices.GetViewOrigin() - InPrevViewMatrices.GetViewOrigin();
// 	ViewUniformParameters.CullingSign = bReverseCulling ? -1.0f : 1.0f;
// 	ViewUniformParameters.NearPlane = GNearClippingPlane;
// 	ViewUniformParameters.MaterialTextureMipBias = 0.0f;
// 	ViewUniformParameters.MaterialTextureDerivativeMultiply = 1.0f;
// 
// 	ViewUniformParameters.bCheckerboardSubsurfaceProfileRendering = 0;

	ViewUniformParameters.ScreenToWorld = Matrix(
		Plane(1, 0, 0, 0),
		Plane(0, 1, 0, 0),
		Plane(0, 0, ProjectionMatrixUnadjustedForRHI.M[2][2], 1),
		Plane(0, 0, ProjectionMatrixUnadjustedForRHI.M[3][2], 0))
		* InViewMatrices.GetInvViewProjectionMatrix();

	ViewUniformParameters.ScreenToTranslatedWorld = Matrix(
		Plane(1, 0, 0, 0),
		Plane(0, 1, 0, 0),
		Plane(0, 0, ProjectionMatrixUnadjustedForRHI.M[2][2], 1),
		Plane(0, 0, ProjectionMatrixUnadjustedForRHI.M[3][2], 0))
		* InViewMatrices.GetInvTranslatedViewProjectionMatrix();

// 	ViewUniformParameters.PrevScreenToTranslatedWorld = Matrix(
// 		FPlane(1, 0, 0, 0),
// 		FPlane(0, 1, 0, 0),
// 		FPlane(0, 0, ProjectionMatrixUnadjustedForRHI.M[2][2], 1),
// 		FPlane(0, 0, ProjectionMatrixUnadjustedForRHI.M[3][2], 0))
// 		* InPrevViewMatrices.GetInvTranslatedViewProjectionMatrix();

// 	FVector DeltaTranslation = InPrevViewMatrices.GetPreViewTranslation() - InViewMatrices.GetPreViewTranslation();
// 	FMatrix InvViewProj = InViewMatrices.ComputeInvProjectionNoAAMatrix() * InViewMatrices.GetTranslatedViewMatrix().GetTransposed();
// 	FMatrix PrevViewProj = FTranslationMatrix(DeltaTranslation) * InPrevViewMatrices.GetTranslatedViewMatrix() * InPrevViewMatrices.ComputeProjectionNoAAMatrix();

// 	ViewUniformParameters.ClipToPrevClip = InvViewProj * PrevViewProj;
// 	ViewUniformParameters.TemporalAAJitter = Vector4(
// 		InViewMatrices.GetTemporalAAJitter().X, InViewMatrices.GetTemporalAAJitter().Y,
// 		InPrevViewMatrices.GetTemporalAAJitter().X, InPrevViewMatrices.GetTemporalAAJitter().Y);

	//ViewUniformParameters.UnlitViewmodeMask = !Family->EngineShowFlags.Lighting ? 1 : 0;
	//ViewUniformParameters.OutOfBoundsMask = Family->EngineShowFlags.VisualizeOutOfBoundsPixels ? 1 : 0;

	//ViewUniformParameters.GameTime = Family->CurrentWorldTime;
	//ViewUniformParameters.RealTime = Family->CurrentRealTime;
	//ViewUniformParameters.Random = Math::Rand();
	//ViewUniformParameters.FrameNumber = Family->FrameNumber;

	//ViewUniformParameters.CameraCut = bCameraCut ? 1 : 0;

	//to tail call keep the order and number of parameters of the caller function
	SetupViewRectUniformBufferParameters(ViewUniformParameters, BufferSize, EffectiveViewRect, InViewMatrices, InPrevViewMatrices);
	ViewUniformParameters.TransposeMatrices();
}

void ViewUniformShaderParameters::TransposeMatrices()
{
	TranslatedWorldToClip.Transpose();
	WorldToClip.Transpose();
	TranslatedWorldToView.Transpose();
	ViewToTranslatedWorld.Transpose();
	TranslatedWorldToCameraView.Transpose();
	CameraViewToTranslatedWorld.Transpose();
	ViewToClip.Transpose();
	ViewToClipNoAA.Transpose();
	ClipToView.Transpose();
	ClipToTranslatedWorld.Transpose();
	SVPositionToTranslatedWorld.Transpose();
	ScreenToWorld.Transpose();
	ScreenToTranslatedWorld.Transpose();
}
