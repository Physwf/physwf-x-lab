#include "Scene.h"
#include "log.h"
#include "D3D11RHI.h"

ViewMatrices::ViewMatrices(const Vector& InViewOrigin, const Matrix& InViewMatrix, const Matrix& InProjectionMatrix)
{
	Vector LocalViewOrigin = InViewOrigin;
	Matrix ViewRotationMatrix = InViewMatrix;

	ViewMatrix = TranslationMatrix(-LocalViewOrigin) * ViewProjectionMatrix;
	ProjectionMatrix = InProjectionMatrix;
	InvProjectionMatrix = ProjectionMatrix.Inverse();

	ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
	
	InvViewMatrix = ViewRotationMatrix.GetTransposed() * TranslationMatrix(LocalViewOrigin);
	InvViewProjectionMatrix = InvProjectionMatrix * InvViewMatrix;

	bool bApplyPreViewTranslation = true;

	if (true)
	{
		ViewOrigin = LocalViewOrigin;
	}

	Matrix LocalTranslatedViewMatrix = ViewRotationMatrix;
	Matrix LocalInvTranslatedViewMatrix = LocalTranslatedViewMatrix.GetTransposed();

	if (bApplyPreViewTranslation)
	{
		PreViewTranslation = -Vector(LocalViewOrigin);
	}

	TranslatedViewMatrix = LocalTranslatedViewMatrix;
	InvTranslatedViewMatrix = LocalInvTranslatedViewMatrix;

	OverriddenTranslatedViewMatrix = TranslationMatrix(-GetPreViewTranslation()) * GetViewMatrix();
	OverriddenInvTranslatedViewMatrix = GetInvViewMatrix() * TranslationMatrix(GetPreViewTranslation());

	TranslatedViewProjectionMatrix = LocalTranslatedViewMatrix * ProjectionMatrix;
	InvTranslatedViewProjectionMatrix = InvProjectionMatrix * LocalInvTranslatedViewMatrix;
}

void ViewMatrices::UpdateViewMatrix(const Vector& ViewLocation)
{
	ViewOrigin = ViewLocation;

	Matrix ViewRotationMatrix;
	ViewRotationMatrix.SetIndentity();

	ViewMatrix = TranslationMatrix(-ViewLocation) * ViewRotationMatrix;

	ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

	InvViewMatrix = ViewRotationMatrix.GetTransposed() * TranslationMatrix(ViewLocation);
	InvViewProjectionMatrix = GetInvProjectionMatrix() * GetInvViewMatrix();

	PreViewTranslation = -ViewOrigin;

	TranslatedViewMatrix = ViewRotationMatrix;
	InvTranslatedViewMatrix = TranslatedViewMatrix.GetTransposed();
	OverriddenTranslatedViewMatrix = TranslationMatrix(-PreViewTranslation) * ViewMatrix;
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

#pragma pack(push)
#pragma pack(1)
struct ViewUniform
{
	Matrix TranslatedWorldToClip;
	Matrix WorldToClip;
	Matrix TranslatedWorldToView;
	Matrix ViewToTranslatedWorld;
	Matrix TranslatedWorldToCameraView;
	Matrix CameraViewToTranslatedWorld;
	Matrix ViewToClip;
	Matrix ViewToClipNoAA;
	Matrix ClipToView;
	Matrix ClipToTranslatedWorld;
	Matrix SVPositionToTranslatedWorld;
	Matrix ScreenToWorld;
	Matrix ScreenToTranslatedWorld;
	// half3 ViewForward;
	// half3 ViewUp;
	// half3 ViewRight;
	// half3 HMDViewNoRollUp;
	// half3 HMDViewNoRollRight;
	Vector4 InvDeviceZToWorldZTransform;
	Vector4 ScreenPositionScaleBias;
	Vector WorldCameraOrigin;
	float ViewPading01;
	Vector TranslatedWorldCameraOrigin;
	float ViewPading02;
	Vector WorldViewOrigin;
	float ViewPading03;

	Vector PreViewTranslation;
	float ViewPading04;
	Vector4 ViewRectMin;
	Vector4 ViewSizeAndInvSize;
	Vector4 BufferSizeAndInvSize;

	uint32 Random;
	uint32 FrameNumber;
	uint32 StateFrameIndexMod8;
	uint32 ViewPading05;

	float DemosaicVposOffset;
	Vector IndirectLightingColorScale;

	Vector AtmosphericFogSunDirection;
	float AtmosphericFogSunPower;
	float AtmosphericFogPower;
	float AtmosphericFogDensityScale;
	float AtmosphericFogDensityOffset;
	float AtmosphericFogGroundOffset;
	float AtmosphericFogDistanceScale;
	float AtmosphericFogAltitudeScale;
	float AtmosphericFogHeightScaleRayleigh;
	float AtmosphericFogStartDistance;
	float AtmosphericFogDistanceOffset;
	float AtmosphericFogSunDiscScale;
	uint32 AtmosphericFogRenderMask;
	uint32 AtmosphericFogInscatterAltitudeSampleNum;
	LinearColor AtmosphericFogSunColor;

	float AmbientCubemapIntensity;
	float SkyLightParameters;
	float PrePadding_View_2472;
	float PrePadding_View_2476;
	Vector4 SkyLightColor;
	Vector4 SkyIrradianceEnvironmentMap[7];

	float PrePadding_View_2862;
	Vector VolumetricLightmapWorldToUVScale;
	float PrePadding_View_2861;
	Vector VolumetricLightmapWorldToUVAdd;
	float PrePadding_View_2876;
	Vector VolumetricLightmapIndirectionTextureSize;

	float VolumetricLightmapBrickSize;
	Vector VolumetricLightmapBrickTexelSize;
};
#pragma pack(pop)

ViewUniform VU;

ID3D11Buffer* ViewUniformBuffer;

std::vector<Mesh*> AllMeshes;
std::vector<MeshBatch> AllBatches;
DirectionalLight DirLight;

Camera MainCamera;
Actor* SelectedActor;
ViewMatrices VMs;

float ViewZNear = 100.f;
float ViewZFar = 2000000000.f;

void InitScene()
{
	Mesh* m1 = new Mesh();
	m1->ImportFromFBX("shaderBallNoCrease/shaderBall.fbx");
	//m1->ImportFromFBX("k526efluton4-House_15/247_House 15_fbx.fbx");
	//m1->ImportFromFBX("Primitives/Sphere.fbx");
	//m1->GeneratePlane(100.f, 100.f, 1, 1);
	//m1->SetPosition(20.0f, -100.0f, 480.0f);
	//m1->SetRotation(-3.14f / 2.0f, 0, 0);
	m1->InitResource();
	AllMeshes.push_back(m1);
	SelectedActor = m1;
	for (Mesh* m : AllMeshes)
	{
		m->DrawStaticElement();
	}

	DirLight.Color = Vector(1.0f, 1.0f, 1.0f);
	DirLight.Direction = Vector(1.0f, 1.0f, 1.0f);
	DirLight.Intencity = 1000.f;
	DirLight.LightSourceAngle = 0.5357f;
	DirLight.LightSourceSoftAngle = 0.0f;
	DirLight.Direction.Normalize();

	MainCamera.SetViewport((float)WindowWidth, (float)WindowHeight);
	MainCamera.SetPostion(Vector(0, -400, 0));
	MainCamera.LookAt(Vector(0, 0, 0));
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
}

void UpdateView()
{
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


	VU.AtmosphericFogSunDirection = { -0.39815f, -0.05403f, 0.91573f };
	VU.AtmosphericFogSunPower = 1.0f;
	VU.AtmosphericFogPower = 1.0f;
	VU.AtmosphericFogDensityScale = 1.0f;
	VU.AtmosphericFogDensityOffset = 0.0f;
	VU.AtmosphericFogGroundOffset = -98975.89844f;
	VU.AtmosphericFogDistanceScale = 1.0f;
	VU.AtmosphericFogAltitudeScale = 1.0f;
	VU.AtmosphericFogHeightScaleRayleigh = 8.0f;
	VU.AtmosphericFogStartDistance = 0.15f;
	VU.AtmosphericFogDistanceOffset = 0.0f;
	VU.AtmosphericFogSunDiscScale = 1.0f;
	VU.AtmosphericFogRenderMask = 0;
	VU.AtmosphericFogInscatterAltitudeSampleNum = 32;
	VU.AtmosphericFogSunColor = { 2.75f, 2.75f, 2.75f, 2.75f };

	VU.SkyLightParameters = 1.f;
	VU.SkyLightColor = {1.0f,1.0f, 1.0f, 1.0f };
	VU.SkyIrradianceEnvironmentMap[0] = { 0.00155f, -0.0033f,  0.06505f,  0.14057f };
	VU.SkyIrradianceEnvironmentMap[1] = { 0.00003f, -0.00304f,  0.09185f, 0.17386f };
	VU.SkyIrradianceEnvironmentMap[2] = { -0.00289f, -0.00268f, 0.1612f,  0.25372f };
	VU.SkyIrradianceEnvironmentMap[3] = { 0.00864f, -0.0024f,  -0.05994f, -0.00767f };
	VU.SkyIrradianceEnvironmentMap[4] = { 0.00792f, -0.00237f, -0.06537f, -0.00857f };
	VU.SkyIrradianceEnvironmentMap[5] = { 0.00666f, -0.00231f, -0.07398f, -0.01038f };
	VU.SkyIrradianceEnvironmentMap[6] = { -0.00346f, -0.00294f, -0.00201f, 1.00f };

	D3D11DeviceContext->UpdateSubresource(ViewUniformBuffer, 0, NULL, &VU, 0, 0);


	
}
