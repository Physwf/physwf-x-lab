#include "DeferredShading.h"
#include "Mesh.h"
#include "Light.h"
#include "Scene.h"
#include "Camera.h"
#include "DirectXTex.h"
using namespace DirectX;

std::vector<Mesh> AllMeshes;
std::vector<MeshBatch> AllBatches;

Camera MainCamera;


struct RectangleVertex
{
	Vector2 Position;
	Vector2 UV;
};

struct DrawRectangleParameters
{
	Vector4 PosScaleBias;
	Vector4 UVScaleBias;
	Vector4 InvTargetSizeAndTextureSize;
};
/*
 0------1
 |		|
 2------3
*/

struct ScreenRectangle
{
	RectangleVertex Vertices[4];
	int Indices[6] = { 0,1,2,2,1,3 };

	DrawRectangleParameters UniformPrameters;

	ID3D11Buffer* VertexBuffer = NULL;
	ID3D11Buffer* IndexBuffer = NULL;
	ID3D11Buffer* DrawRectangleParameters = NULL;

	ScreenRectangle(float W, float H)
	{
		Vertices[0].Position = Vector2(1.0f, 1.0f);
		Vertices[0].UV = Vector2(1.0f, 1.0f);
		Vertices[1].Position = Vector2(0.0f, 1.0f);
		Vertices[1].UV = Vector2(0.0f, 1.0f);
		Vertices[2].Position = Vector2(1.0f, 0.0f);
		Vertices[2].UV = Vector2(1.0f, 0.0f);
		Vertices[3].Position = Vector2(0.0f, 0.0f);
		Vertices[3].UV = Vector2(0.0f, 0.0f);

		UniformPrameters.PosScaleBias = Vector4(W, H, 0.0f, 0.0f);
		UniformPrameters.UVScaleBias = Vector4(W, H, 0.0f, 0.0f);
		UniformPrameters.InvTargetSizeAndTextureSize = Vector4(1.0f/W, 1.0f/H, 1.0f / W, 1.0f / H);
	}

	~ScreenRectangle()
	{
		ReleaseResource();
	}

	void InitResource()
	{
		VertexBuffer = CreateVertexBuffer(false, sizeof(Vertices), Vertices);
		IndexBuffer = CreateIndexBuffer(Indices, sizeof(Indices));
		DrawRectangleParameters = CreateConstantBuffer(false,sizeof(UniformPrameters), &UniformPrameters);
	}

	void ReleaseResource()
	{
		if (VertexBuffer)
		{
			VertexBuffer->Release();
		}
		if (IndexBuffer)
		{
			IndexBuffer->Release();
		}
	}
};

ScreenRectangle ScreenRect((float)WindowWidth, (float)WindowHeight);

DirectionalLight DirLight;
DeferredLightUniforms DLU;
ID3D11Buffer* DeferredLightUniformBuffer;
ID3D11Buffer* VSourceTexture;

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

	float View_AmbientCubemapIntensity;
	float View_SkyLightParameters;
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

/** The number of coefficients that are stored for each light sample. */
static const int32 NUM_STORED_LIGHTMAP_COEF = 4;

/** The number of directional coefficients which the lightmap stores for each light sample. */
static const int32 NUM_HQ_LIGHTMAP_COEF = 2;

/** The number of simple coefficients which the lightmap stores for each light sample. */
static const int32 NUM_LQ_LIGHTMAP_COEF = 2;

/** The index at which simple coefficients are stored in any array containing all NUM_STORED_LIGHTMAP_COEF coefficients. */
static const int32 LQ_LIGHTMAP_COEF_INDEX = 2;

/** The maximum value between NUM_LQ_LIGHTMAP_COEF and NUM_HQ_LIGHTMAP_COEF. */
static const int32 MAX_NUM_LIGHTMAP_COEF = 2;

struct PrecomputedLightingUniform
{
	Vector IndirectLightingCachePrimitiveAdd;// FCachedVolumeIndirectLightingPolicy
	float Pading01;
	Vector IndirectLightingCachePrimitiveScale;// FCachedVolumeIndirectLightingPolicy
	float Pading02;
	Vector IndirectLightingCacheMinUV;// FCachedVolumeIndirectLightingPolicy
	float Pading03;
	Vector IndirectLightingCacheMaxUV;// FCachedVolumeIndirectLightingPolicy
	float Pading04;
	Vector4 PointSkyBentNormal;// FCachedPointIndirectLightingPolicy
	float DirectionalLightShadowing;// FCachedPointIndirectLightingPolicy
	Vector Pading05;
	Vector4 StaticShadowMapMasks;// TDistanceFieldShadowsAndLightMapPolicy
	Vector4 InvUniformPenumbraSizes;// TDistanceFieldShadowsAndLightMapPolicy
	Vector4 IndirectLightingSHCoefficients0[3]; // FCachedPointIndirectLightingPolicy
	Vector4 IndirectLightingSHCoefficients1[3]; // FCachedPointIndirectLightingPolicy
	Vector4 IndirectLightingSHCoefficients2;// FCachedPointIndirectLightingPolicy
	Vector4 IndirectLightingSHSingleCoefficient;// FCachedPointIndirectLightingPolicy
	Vector4 LightMapCoordinateScaleBias; // TLightMapPolicy LightmapCoordinateScale = {X=0.484375000 Y=0.968750000 } LightmapCoordinateBias = {X=0.00781250000 Y=0.0156250000 }
	Vector4 ShadowMapCoordinateScaleBias;// TDistanceFieldShadowsAndLightMapPolicy
	Vector4 LightMapScale[MAX_NUM_LIGHTMAP_COEF];// TLightMapPolicy
	Vector4 LightMapAdd[MAX_NUM_LIGHTMAP_COEF];// TLightMapPolicy
};

#pragma pack(pop)
//PrecomputedLightParameters
ID3D11ShaderResourceView* PrecomputedLighting_LightMapTexture;
ID3D11ShaderResourceView* PrecomputedLighting_SkyOcclusionTexture;
ID3D11ShaderResourceView* PrecomputedLighting_AOMaterialMaskTexture;
ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture0;
ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture1;
ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture2;
ID3D11ShaderResourceView* PrecomputedLighting_StaticShadowTexture;
ID3D11SamplerState* PrecomputedLighting_LightMapSampler;
ID3D11SamplerState* PrecomputedLighting_SkyOcclusionSampler;
ID3D11SamplerState* PrecomputedLighting_AOMaterialMaskSampler;
ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler0;
ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler1;
ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler2;
ID3D11SamplerState* PrecomputedLighting_StaticShadowTextureSampler;

struct ShadowMapView
{
	Matrix View;
	Matrix Projection;
};

Matrix ShadowMapProjectionMatrix;
ID3D11Buffer* ShadowMapProjectionUniformBuffer;

Matrix ScreenToShadowMatrix;

ID3D11Buffer* ViewUniformBuffer;
ID3D11Buffer* PrecomputedLightUniformBuffer;
ID3D11Buffer* DirecianlLightShadowMapViewUniformBuffer;

ID3D11InputLayout* PositionOnlyMeshInputLayout;
ID3D11InputLayout* MeshInputLayout;
ID3D11InputLayout* RectangleInputLayout;
ID3D11InputLayout* ShadowProjectionInputLayout;

ID3D11Texture2D* SceneDepthRT;
ID3D11DepthStencilView* SceneDepthDSV;

ID3D11Buffer* DynamicVertexBuffer;

ID3DBlob* PrePassVSBytecode;
ID3DBlob* PrePassPSBytecode;
ID3D11VertexShader* PrePassVS;
ID3D11PixelShader* PrePassPS;
ID3D11RasterizerState* PrePassRasterizerState;
ID3D11BlendState* PrePassBlendState;
ID3D11DepthStencilState* PrePassDepthStencilState;

ID3DBlob* ShadowPassVSBytecode;
ID3DBlob* ShadowPassPSBytecode;
ID3D11Texture2D* ShadowPassRT;
ID3D11DepthStencilView* ShadowPassDSV;
ID3D11VertexShader* ShadowPassVS;
ID3D11PixelShader* ShadowPassPS;
ID3D11RasterizerState* ShadowPassRasterizerState;
ID3D11BlendState* ShadowPassBlendState;
ID3D11DepthStencilState* ShadowPassDepthStencilState;

ID3D11ShaderResourceView* ShadowPassDepthSRV;
ID3D11SamplerState* ShadowPassDepthSamplerState;
ID3DBlob* ShadowProjectionVSBytecode;
ID3DBlob* ShadowProjectionPSBytecode;
ID3D11Texture2D* ShadowProjectionRT;
ID3D11RenderTargetView* ShadowProjectionRTV;
ID3D11VertexShader* ShadowProjectionVS;
ID3D11PixelShader* ShadowProjectionPS;
ID3D11RasterizerState* ShadowProjectionRasterizerState;
ID3D11BlendState* ShadowProjectionBlendState;
ID3D11DepthStencilState* ShadowProjectionDepthStencilState;

ID3DBlob* BasePassVSBytecode;
ID3DBlob* BasePassPSBytecode;

ID3D11Texture2D* BasePassSceneColorRT;
ID3D11RenderTargetView* BasePassSceneColorRTV;
ID3D11Texture2D* BasePassGBufferRT[6];
ID3D11RenderTargetView* BasePassGBufferRTV[6];
ID3D11VertexShader* BasePassVS;
ID3D11PixelShader* BasePassPS;
ID3D11RasterizerState* BasePassRasterizerState;
ID3D11BlendState* BasePassBlendState;
ID3D11DepthStencilState* BasePassDepthStencilState;

ID3DBlob* LightPassVSByteCode;
ID3DBlob* LightPassPSByteCode;
//Scene Color and depth
ID3D11ShaderResourceView* LightPassSceneColorSRV;
ID3D11ShaderResourceView* LightPassSceneDepthSRV;
ID3D11SamplerState* LightPassSceneColorSamplerState;
ID3D11SamplerState* LightPassSceneDepthSamplerState;
//G-Buffer
ID3D11ShaderResourceView* LightPassGBufferSRV[6];
ID3D11SamplerState* LightPassGBufferSamplerState[6];
//Screen Space AO, Custom Depth, Custom Stencil
ID3D11Texture2D* LightPassScreenSapceAOTexture;
ID3D11Texture2D* LightPassCustomDepthTexture;
ID3D11Texture2D* LightPassCustomStencialTexture;
ID3D11ShaderResourceView* LightPassScreenSapceAOSRV;
ID3D11ShaderResourceView* LightPassCustomDepthSRV;
ID3D11ShaderResourceView* LightPassCustomStencialSRV;
ID3D11SamplerState* LightPassScreenSapceAOSamplerState;
ID3D11SamplerState* LightPassCustomDepthSamplerState;
ID3D11SamplerState* LightPassCustomStencialState;

ID3D11VertexShader* LightPassVS;
ID3D11PixelShader* LightPassPS;
ID3D11RasterizerState* LightPassRasterizerState;
ID3D11BlendState* LightPassBlendState;
ID3D11DepthStencilState* LightPassDepthStencilState;
ID3D11ShaderResourceView* LightAttenuationSRV;
ID3D11SamplerState* LightAttenuationSampleState;

std::map<std::string, ParameterAllocation> PrePassVSParams;
std::map<std::string, ParameterAllocation> PrePassPSParams;
std::map<std::string, ParameterAllocation> ShadowPassVSParams;
std::map<std::string, ParameterAllocation> ShadowPassPSParams;
std::map<std::string, ParameterAllocation> ShadowProjectionVSParams;
std::map<std::string, ParameterAllocation> ShadowProjectionPSParams;
ID3D11Buffer* GlobalConstantBuffer;
char GlobalConstantBufferData[4096];

std::map<std::string, ParameterAllocation> BasePassVSParams;
std::map<std::string, ParameterAllocation> BasePassPSParams;
std::map<std::string, ParameterAllocation> LightPassVSParams;
std::map<std::string, ParameterAllocation> LightPassPSParams;


//material
ID3D11ShaderResourceView* BaseColorSRV;
ID3D11SamplerState* BaseColorSampler;

ViewUniform VU;
PrecomputedLightingUniform PrecomputedLightingParameters;

void InitInput()
{
	Mesh m1;
	m1.ImportFromFBX("shaderBallNoCrease/shaderBall.fbx");
	//m1.ImportFromFBX("k526efluton4-House_15/247_House 15_fbx.fbx");
	//m1.ImportFromFBX("Primitives/Sphere.fbx");
	//m1.GeneratePlane(100.f, 100.f, 1, 1);
	//m1.SetPosition(20.0f, -100.0f, 480.0f);
	//m1.SetRotation(-3.14f / 2.0f, 0, 0);
	m1.InitResource();
	AllMeshes.push_back(m1);

	for (Mesh& m : AllMeshes)
	{
		m.DrawStaticElement();
	}

	MainCamera.SetPostion(Vector(0,0 , 400));
	MainCamera.LookAt(Vector(0, 0, -100));
	//Matrix::DXFromPerspectiveFovLH(3.1415926f / 2, 1.0, 1.0f, 10000.f);
	Matrix ProjectionMatrix = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 3.f, (float)WindowWidth/WindowHeight, 100.0f, 600.f);
	//Matrix ProjectionMatrix = ReversedZPerspectiveMatrix(3.1415926f / 2.f, 3.1415926f / 2.f, 1.0f, 1.0f, 1.0,1.0f);
	Matrix ViewRotationMatrix = Matrix::DXLookAtLH(Vector(0, 0, 0), MainCamera.FaceDir, MainCamera.Up);
	ViewMatrices VMs(MainCamera.Eye, ViewRotationMatrix, ProjectionMatrix);
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
	VU.ViewSizeAndInvSize = Vector4((float)WindowWidth, (float)WindowHeight,1.0f/ WindowWidth, 1.0f / WindowHeight);
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
	
	//LightMapRendering.cpp
	const Vector2 LightmapCoordinateScale = { 0.484375000 ,0.968750000 };// LightMapInteraction.GetCoordinateScale();// LightmapCoordinateScale = { X = 0.484375000 Y = 0.968750000 } LightmapCoordinateBias = { X = 0.00781250000 Y = 0.0156250000 }
	const Vector2 LightmapCoordinateBias = { 0.00781250000 ,0156250000 }; //LightMapInteraction.GetCoordinateBias();
	PrecomputedLightingParameters.LightMapCoordinateScaleBias = Vector4(LightmapCoordinateScale.X, LightmapCoordinateScale.Y, LightmapCoordinateBias.X, LightmapCoordinateBias.Y);

	PrecomputedLightingParameters.ShadowMapCoordinateScaleBias = Vector4(0.968750000, 0.968750000, 0.0156250000, 0.0156250000);//{ X = 0.968750000 Y = 0.968750000 Z = 0.0156250000 W = 0.0156250000 }

	bool bAllowHighQualityLightMaps = true;
	const uint32 NumCoef = bAllowHighQualityLightMaps ? NUM_HQ_LIGHTMAP_COEF : NUM_LQ_LIGHTMAP_COEF;
	//const FVector4* Scales = LightMapInteraction.GetScaleArray();Scales = 0x0000006554e6b040 {X=0.0207918882 Y=0.0144486427 Z=0.0207914710 W=6.19691181} Scales[CoefIndex] = {X=0.915465474 Y=0.976352394 Z=0.944339991 W = 0.00000000}
	//const FVector4* Adds = LightMapInteraction.GetAddArray();//Adds = 0x0000006554e6b060 {X=0.986185253 Y=0.995151460 Z=0.986185730 W = -5.75000048} Adds[CoefIndex] = {X=-0.465408266 Y=-0.519816577 Z=-0.434536219 W = 0.282094985}
	Vector4 Scales[] = { { 0.0207918882f ,0.0144486427f ,0.0207914710f ,6.19691181f }, { 0.915465474f , 0.976352394f , 0.944339991f , 0.00000000f } };
	Vector4 Adds[] = { { 0.986185253f , 0.995151460f, 0.986185730f , -5.75000048f }, {-0.465408266f , -0.519816577f , -0.434536219f , 0.282094985f} };
	for (uint32 CoefIndex = 0; CoefIndex < NumCoef; ++CoefIndex)
	{
		PrecomputedLightingParameters.LightMapScale[CoefIndex] = Scales[CoefIndex];
		PrecomputedLightingParameters.LightMapAdd[CoefIndex] = Adds[CoefIndex];
	}
	ViewUniformBuffer = CreateConstantBuffer(false, sizeof(VU), &VU);
	PrecomputedLightUniformBuffer = CreateConstantBuffer(false, sizeof(PrecomputedLightingParameters), &PrecomputedLightingParameters);

	SceneDepthRT = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R24G8_TYPELESS, false, true, true);
	SceneDepthDSV = CreateDepthStencilView2D(SceneDepthRT, DXGI_FORMAT_D24_UNORM_S8_UINT, 0);
	//Prepass
	PrePassVSBytecode = CompileVertexShader(TEXT("DepthOnlyPass.hlsl"), "VS_Main");
	PrePassPSBytecode = CompilePixelShader(TEXT("DepthOnlyPass.hlsl"), "PS_Main");
	GetShaderParameterAllocations(PrePassVSBytecode, PrePassVSParams);
	GetShaderParameterAllocations(PrePassPSBytecode, PrePassPSParams);

	D3D11_INPUT_ELEMENT_DESC PositionOnlyInputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	PositionOnlyMeshInputLayout = CreateInputLayout(PositionOnlyInputDesc, sizeof(PositionOnlyInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), PrePassVSBytecode);
	
	DynamicVertexBuffer = CreateVertexBuffer(true,4*sizeof(Vector4));

	PrePassVS = CreateVertexShader(PrePassVSBytecode);
	PrePassPS = CreatePixelShader(PrePassPSBytecode);

	PrePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE, FALSE>::GetRHI();
	PrePassDepthStencilState = TStaticDepthStencilState<true, D3D11_COMPARISON_GREATER>::GetRHI();
	PrePassBlendState = TStaticBlendState<>::GetRHI();

	//shadow pass
	ShadowPassVSBytecode = CompileVertexShader(TEXT("ShadowDepthVertexShader.hlsl"), "VS_Main");
	ShadowPassPSBytecode = CompilePixelShader(TEXT("ShadowDepthPixelShader.hlsl"), "PS_Main");
	GetShaderParameterAllocations(ShadowPassVSBytecode, ShadowPassVSParams);
	GetShaderParameterAllocations(ShadowPassPSBytecode, ShadowPassPSParams);
	ShadowPassVS = CreateVertexShader(ShadowPassVSBytecode);
	ShadowPassPS = CreatePixelShader(ShadowPassPSBytecode);

	ShadowPassRT = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32_TYPELESS, false, true, true);
	ShadowPassDSV = CreateDepthStencilView2D(ShadowPassRT, DXGI_FORMAT_D32_FLOAT, 0);
	ShadowPassDepthSRV = CreateShaderResourceView2D(ShadowPassRT, DXGI_FORMAT_R32_FLOAT, 1, 0);

	ShadowPassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE>::GetRHI();
	ShadowPassBlendState = TStaticBlendState<>::GetRHI();
	ShadowPassDepthStencilState = TStaticDepthStencilState<true, D3D11_COMPARISON_LESS>::GetRHI();

	ShadowProjectionVSBytecode = CompileVertexShader(TEXT("ShadowProjectionVertexShader.hlsl"), "Main");
	ShadowProjectionPSBytecode = CompilePixelShader(TEXT("ShadowProjectionPixelShader.hlsl"), "Main");
	GetShaderParameterAllocations(ShadowProjectionVSBytecode, ShadowProjectionVSParams);
	GetShaderParameterAllocations(ShadowProjectionPSBytecode, ShadowProjectionPSParams);

	D3D11_INPUT_ELEMENT_DESC ShadowProjectionInputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	ShadowProjectionInputLayout = CreateInputLayout(ShadowProjectionInputDesc, sizeof(ShadowProjectionInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), ShadowProjectionVSBytecode);

	ShadowProjectionRT = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);
	ShadowProjectionRTV = CreateRenderTargetView2D(ShadowProjectionRT, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
	ShadowProjectionVS = CreateVertexShader(ShadowProjectionVSBytecode);
	ShadowProjectionPS = CreatePixelShader(ShadowProjectionPSBytecode);
	GlobalConstantBuffer = CreateConstantBuffer(false,4096);
	memset(GlobalConstantBufferData, 0, sizeof(GlobalConstantBufferData));

	//Base Pass
	BasePassVSBytecode = CompileVertexShader(TEXT("BasePassVertexShader.hlsl"), "VS_Main");
	BasePassPSBytecode = CompilePixelShader(TEXT("BasePassPixelShader.hlsl"), "PS_Main");
	GetShaderParameterAllocations(BasePassVSBytecode, BasePassVSParams);
	GetShaderParameterAllocations(BasePassPSBytecode, BasePassPSParams);

	D3D11_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	1,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 16, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	2,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	3,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	4,	DXGI_FORMAT_R32G32_FLOAT,		0, 64, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	15,	DXGI_FORMAT_R32G32_FLOAT,		0, 72, D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	MeshInputLayout = CreateInputLayout(InputDesc, sizeof(InputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), BasePassVSBytecode);
	BasePassVS = CreateVertexShader(BasePassVSBytecode);
	BasePassPS = CreatePixelShader(BasePassPSBytecode);

	BasePassSceneColorRT = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);
	BasePassGBufferRT[0] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//WorldNormal(3),PerObjectGBufferData(1)
	BasePassGBufferRT[1] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//metalic(1),specular(1),Roughness(1),ShadingModelID|SelectiveOutputMask(1)
	BasePassGBufferRT[2] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//BaseColor(3),GBufferAO(1)
// 	BasePassGBufferRT[3] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//CustomData(4)
// 	BasePassGBufferRT[4] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//PrecomputedShadowFactors(4)
// 	BasePassGBufferRT[5] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, true, true, false);//Velocity(4)

	BasePassSceneColorRTV = CreateRenderTargetView2D(BasePassSceneColorRT, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
	BasePassGBufferRTV[0] = CreateRenderTargetView2D(BasePassGBufferRT[0], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
	BasePassGBufferRTV[1] = CreateRenderTargetView2D(BasePassGBufferRT[1], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
	BasePassGBufferRTV[2] = CreateRenderTargetView2D(BasePassGBufferRT[2], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
// 	BasePassGBufferRTV[3] = CreateRenderTargetView2D(BasePassGBufferRT[4], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
// 	BasePassGBufferRTV[4] = CreateRenderTargetView2D(BasePassGBufferRT[5], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
// 	BasePassGBufferRTV[5] = CreateRenderTargetView2D(BasePassGBufferRT[5], DXGI_FORMAT_R32G32B32A32_FLOAT, 0);

	BasePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK,FALSE, FALSE>::GetRHI();
	BasePassDepthStencilState = TStaticDepthStencilState<false, D3D11_COMPARISON_GREATER_EQUAL>::GetRHI();
	BasePassBlendState = TStaticBlendState<>::GetRHI();

	LightPassVSByteCode = CompileVertexShader(TEXT("DeferredLightVertexShader.hlsl"), "VS_Main");
	LightPassPSByteCode = CompilePixelShader(TEXT("DeferredLightPixelShader.hlsl"), "PS_Main");
	GetShaderParameterAllocations(LightPassVSByteCode, LightPassVSParams);
	GetShaderParameterAllocations(LightPassPSByteCode, LightPassPSParams);

	D3D11_INPUT_ELEMENT_DESC RectangleInputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	1,	DXGI_FORMAT_R32G32_FLOAT,	0, 8,  D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	ScreenRect.InitResource();

	RectangleInputLayout = CreateInputLayout(RectangleInputDesc, sizeof(RectangleInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), LightPassVSByteCode);
	LightPassVS = CreateVertexShader(LightPassVSByteCode);
	LightPassPS = CreatePixelShader(LightPassPSByteCode);

	LightPassSceneColorSRV = CreateShaderResourceView2D(BasePassSceneColorRT, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0);
	LightPassSceneDepthSRV = CreateShaderResourceView2D(SceneDepthRT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1, 0);
	LightPassSceneColorSamplerState = TStaticSamplerState<>::GetRHI();
	LightPassSceneDepthSamplerState = TStaticSamplerState<>::GetRHI();

	LightPassGBufferSRV[0] = CreateShaderResourceView2D(BasePassGBufferRT[0], DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0);
	LightPassGBufferSRV[1] = CreateShaderResourceView2D(BasePassGBufferRT[1], DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0);
	LightPassGBufferSRV[2] = CreateShaderResourceView2D(BasePassGBufferRT[2], DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0);

	LightPassGBufferSamplerState[0] = TStaticSamplerState<>::GetRHI();
	LightPassGBufferSamplerState[1] = TStaticSamplerState<>::GetRHI();
	LightPassGBufferSamplerState[2] = TStaticSamplerState<>::GetRHI();

	LightPassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, FALSE>::GetRHI();
	LightPassDepthStencilState = TStaticDepthStencilState<false, D3D11_COMPARISON_ALWAYS>::GetRHI();
	LightPassBlendState = TStaticBlendState<>::GetRHI();

	LightAttenuationSRV = CreateShaderResourceView2D(ShadowProjectionRT, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0);
	LightAttenuationSampleState = TStaticSamplerState<>::GetRHI();

	DirLight.Color = Vector(1.0f,0.0f,0.0f);
	DirLight.Direction = Vector(-1.0f,-1.0f,-1.0f);
	DirLight.Intencity = 1000.f;
	DirLight.LightSourceAngle = 0.5357f;
	DirLight.LightSourceSoftAngle = 0.0f;

	DLU.LightPosition = Vector(0.0f, 0.0f, 0.0f);
	DLU.LightInvRadius = 0.0f;
	DLU.LightColor = DirLight.Color;
	DirLight.Direction.Normalize();
	DLU.NormalizedLightDirection = -DirLight.Direction;
	DLU.NormalizedLightTangent = -DirLight.Direction;
	DLU.SpotAngles = Vector2(0.0f, 0.0f);
	DLU.SpecularScale = 1.0f;
	DLU.SourceRadius = std::sin(0.5f * DirLight.LightSourceAngle / 180.f * 3.14f);
	DLU.SoftSourceRadius = std::sin(0.5f * DirLight.LightSourceSoftAngle / 180.f * 3.14f);
	DLU.SourceLength = 0.0f;
	DLU.ContactShadowLength;
	//const FVector2D FadeParams = LightSceneInfo->Proxy->GetDirectionalLightDistanceFadeParameters(View.GetFeatureLevel(), LightSceneInfo->IsPrecomputedLightingValid(), View.MaxShadowCascades);
	// use MAD for efficiency in the shader
	//DeferredLightUniformsValue.DistanceFadeMAD = FVector2D(FadeParams.Y, -FadeParams.X * FadeParams.Y);
	DLU.DistanceFadeMAD = Vector2(0.5f,0.5f);
	//UE4 LightRender.h
	//DeferredLightUniformsValue.ShadowedBits = LightSceneInfo->Proxy->CastsStaticShadow() || bHasLightFunction ? 1 : 0;
	//DeferredLightUniformsValue.ShadowedBits |= LightSceneInfo->Proxy->CastsDynamicShadow() && View.Family->EngineShowFlags.DynamicShadows ? 3 : 0;
	DLU.ShadowedBits = 2;
	DeferredLightUniformBuffer = CreateConstantBuffer(false, sizeof(DLU), &DLU);

	Matrix DirectionalLightViewRotationMatrix = Matrix::DXLookToLH(DirLight.Direction);
	Frustum ViewFrustum(3.1415926f / 3.f, (float)WindowWidth / WindowHeight, 100.0f, 600.f);
	Box FrumstumBounds = ViewFrustum.GetBounds();
	Vector FrumstumWorldPosition = MainCamera.GetPosition() + MainCamera.FaceDir * (600.f - 100.f);
	Vector FrumstumViewPosition = DirectionalLightViewRotationMatrix.Transform(FrumstumWorldPosition);
	Matrix DirectionalLightProjectionMatrix = Matrix::DXFromOrthognalLH(FrumstumBounds.Max.X, FrumstumBounds.Min.X, FrumstumBounds.Max.Y, FrumstumBounds.Min.Y, FrumstumBounds.Max.Z, FrumstumBounds.Min.Z);
	//DirectionalLightProjectionMatrix = ProjectionMatrix;
	//ShadowMapProjectionMatrix = DirectionalLightViewRotationMatrix * Matrix::DXFromTranslation(-FrumstumViewPosition) * DirectionalLightProjectionMatrix;
	ShadowMapProjectionMatrix = Matrix::DXFromTranslation(-FrumstumWorldPosition) * DirectionalLightViewRotationMatrix *  DirectionalLightProjectionMatrix;
	//Matrix  InvViewProjectionMatrix = VMs.GetInvViewProjectionMatrix();
	//ScreenToShadowMatrix =  InvViewProjectionMatrix * ShadowMapProjectionMatrix;
	//ScreenToShadowMatrix.Transpose();

	//UE4 ShadowRendering.cpp GetScreenToShadowMatrix
	uint32 TileOffsetX = 0; uint32 TileOffsetY = 0;
	uint32 BorderSize = 0;
	float InvMaxSubjectDepth = 1.0f/1.0f;
	const Vector2 ShadowBufferResolution((float)WindowWidth, (float)WindowHeight);
	uint32 TileResolutionX = WindowWidth;
	uint32 TileResolutionY = WindowHeight;
	const float InvBufferResolutionX = 1.0f / (float)ShadowBufferResolution.X;
	const float ShadowResolutionFractionX = 0.5f * (float)TileResolutionX * InvBufferResolutionX;
	const float InvBufferResolutionY = 1.0f / (float)ShadowBufferResolution.Y;
	const float ShadowResolutionFractionY = 0.5f * (float)TileResolutionY * InvBufferResolutionY;
	// Calculate the matrix to transform a screenspace position into shadow map space
	ScreenToShadowMatrix =
		// Z of the position being transformed is actually view space Z, 
		// Transform it into post projection space by applying the projection matrix,
		// Which is the required space before applying View.InvTranslatedViewProjectionMatrix
		Matrix(
			Plane(1, 0, 0, 0),
			Plane(0, 1, 0, 0),
			Plane(0, 0, VMs.GetProjectionMatrix().M[2][2], 1),
			Plane(0, 0, VMs.GetProjectionMatrix().M[3][2], 0)) *
		// Transform the post projection space position into translated world space
		// Translated world space is normal world space translated to the view's origin, 
		// Which prevents floating point imprecision far from the world origin.
		VMs.GetInvTranslatedViewProjectionMatrix() *
		// Translate to the origin of the shadow's translated world space
		//FTranslationMatrix(PreShadowTranslation - View.ViewMatrices.GetPreViewTranslation()) *
		// Transform into the shadow's post projection space
		// This has to be the same transform used to render the shadow depths
		ShadowMapProjectionMatrix *
		// Scale and translate x and y to be texture coordinates into the ShadowInfo's rectangle in the shadow depth buffer
		// Normalize z by MaxSubjectDepth, as was done when writing shadow depths
		Matrix(
			Plane(ShadowResolutionFractionX,		0,									0, 0),
			Plane(0,								-ShadowResolutionFractionY,			0, 0),
			Plane(0,								0,									InvMaxSubjectDepth, 0),
			Plane(
				(TileOffsetX + BorderSize) * InvBufferResolutionX + ShadowResolutionFractionX,
				(TileOffsetY + BorderSize) * InvBufferResolutionY + ShadowResolutionFractionY,
				0,
				1
			)
		);
	ScreenToShadowMatrix.Transpose();

	ShadowMapProjectionMatrix.Transpose();
	//ShadowMapProjectionUniformBuffer = CreateConstantBuffer(false, sizeof(ShadowMapProjectionMatrix), &ShadowMapProjectionMatrix);

	{
		//CreateWICTextureFromFile(D3D11Device, D3D11DeviceContext, TEXT("./shaderBallNoCrease/checkerA.tga"), (ID3D11Resource**)&BaseColor, &BaseColorSRV);
// 		int Width, Height, nChanels;
// 		unsigned char* Data = stbi_load("./shaderBallNoCrease/checkerA.tga", &Width, &Height, &nChanels, 4);
// 		if (Data)
// 		{
// 			CreateWICTextureFromMemory(D3D11Device, D3D11DeviceContext, Data, Width*Height*nChanels,(ID3D11Resource**)&BaseColor, &BaseColorSRV);
// 			//BaseColor = CreateTexture2D(Width, Height, DXGI_FORMAT_,1,Data);
// 		}

		TexMetadata Metadata;
		ScratchImage sImage;
		if (S_OK == LoadFromTGAFile(TEXT("./shaderBallNoCrease/checkerA.tga"), TGA_FLAGS_FORCE_SRGB, &Metadata, sImage))
		{
			CreateShaderResourceView(D3D11Device, sImage.GetImages(), 1, Metadata, &BaseColorSRV);
			BaseColorSampler = TStaticSamplerState<>::GetRHI();
		}
	}

	{
		{
			TexMetadata Metadata;
			ScratchImage sImage;
			if (S_OK == LoadFromHDRFile(TEXT("./dx11demo/PrecomputedLightingBuffer_LightMapTexture.hdr"), &Metadata, sImage))
			{
				CreateShaderResourceView(D3D11Device, sImage.GetImages(), 1, Metadata, &PrecomputedLighting_LightMapTexture);
				PrecomputedLighting_LightMapSampler = TStaticSamplerState<>::GetRHI();
			}
		}
		{
			TexMetadata Metadata;
			ScratchImage sImage;
			if (S_OK == LoadFromHDRFile(TEXT("./dx11demo/PrecomputedLightingBuffer_StaticShadowTexture.hdr"), &Metadata, sImage))
			{
				CreateShaderResourceView(D3D11Device, sImage.GetImages(), 1, Metadata, &PrecomputedLighting_StaticShadowTexture);
				PrecomputedLighting_StaticShadowTextureSampler = TStaticSamplerState<>::GetRHI();
			}
		}
// 		ID3D11ShaderResourceView* PrecomputedLighting_SkyOcclusionTexture;
// 		ID3D11ShaderResourceView* PrecomputedLighting_AOMaterialMaskTexture;
// 		ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture0;
// 		ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture1;
// 		ID3D11ShaderResourceView* PrecomputedLighting_IndirectLightingCacheTexture2;
// 		ID3D11SamplerState* PrecomputedLighting_SkyOcclusionSampler;
// 		ID3D11SamplerState* PrecomputedLighting_AOMaterialMaskSampler;
// 		ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler0;
// 		ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler1;
// 		ID3D11SamplerState* PrecomputedLighting_IndirectLightingCacheTextureSampler2;
	}
}

void UpdateView()
{
	Matrix ProjectionMatrix = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 3.f, (float)WindowWidth/WindowHeight, 100.0f, 600.f);
	Matrix ViewRotationMatrix = Matrix::DXLookAtLH(Vector(0,0,0), MainCamera.FaceDir, MainCamera.Up);

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


	Matrix DirectionalLightViewRotationMatrix = Matrix::DXLookToLH(DirLight.Direction);
	Frustum ViewFrustum(3.1415926f / 3.f, (float)WindowWidth / WindowHeight, 100.0f, 600.f);
	Box FrumstumBounds = ViewFrustum.GetBounds();
	Vector FrumstumWorldPosition = MainCamera.GetPosition() + MainCamera.FaceDir * (600.f - 100.f);
	Vector FrumstumViewPosition = DirectionalLightViewRotationMatrix.Transform(FrumstumWorldPosition);
	Matrix DirectionalLightProjectionMatrix = Matrix::DXFromOrthognalLH(FrumstumBounds.Max.X, FrumstumBounds.Min.X, FrumstumBounds.Max.Y, FrumstumBounds.Min.Y, FrumstumBounds.Max.Z, FrumstumBounds.Min.Z);
	ShadowMapProjectionMatrix = Matrix::DXFromTranslation(-FrumstumWorldPosition) * DirectionalLightViewRotationMatrix *  DirectionalLightProjectionMatrix;

	//UE4 ShadowRendering.cpp GetScreenToShadowMatrix
	uint32 TileOffsetX = 0; uint32 TileOffsetY = 0;
	uint32 BorderSize = 0;
	float InvMaxSubjectDepth = 1.0f / 1.0f;
	const Vector2 ShadowBufferResolution((float)WindowWidth, (float)WindowHeight);
	uint32 TileResolutionX = WindowWidth;
	uint32 TileResolutionY = WindowHeight;
	const float InvBufferResolutionX = 1.0f / (float)ShadowBufferResolution.X;
	const float ShadowResolutionFractionX = 0.5f * (float)TileResolutionX * InvBufferResolutionX;
	const float InvBufferResolutionY = 1.0f / (float)ShadowBufferResolution.Y;
	const float ShadowResolutionFractionY = 0.5f * (float)TileResolutionY * InvBufferResolutionY;
	// Calculate the matrix to transform a screenspace position into shadow map space
	ScreenToShadowMatrix =
		// Z of the position being transformed is actually view space Z, 
		// Transform it into post projection space by applying the projection matrix,
		// Which is the required space before applying View.InvTranslatedViewProjectionMatrix
		Matrix(
			Plane(1, 0, 0, 0),
			Plane(0, 1, 0, 0),
			Plane(0, 0, VMs.GetProjectionMatrix().M[2][2], 1),
			Plane(0, 0, VMs.GetProjectionMatrix().M[3][2], 0)) *
		// Transform the post projection space position into translated world space
		// Translated world space is normal world space translated to the view's origin, 
		// Which prevents floating point imprecision far from the world origin.
		VMs.GetInvTranslatedViewProjectionMatrix() *
		// Translate to the origin of the shadow's translated world space
		//FTranslationMatrix(PreShadowTranslation - View.ViewMatrices.GetPreViewTranslation()) *
		// Transform into the shadow's post projection space
		// This has to be the same transform used to render the shadow depths
		ShadowMapProjectionMatrix *
		// Scale and translate x and y to be texture coordinates into the ShadowInfo's rectangle in the shadow depth buffer
		// Normalize z by MaxSubjectDepth, as was done when writing shadow depths
		Matrix(
			Plane(ShadowResolutionFractionX, 0, 0, 0),
			Plane(0, -ShadowResolutionFractionY, 0, 0),
			Plane(0, 0, InvMaxSubjectDepth, 0),
			Plane(
			(TileOffsetX + BorderSize) * InvBufferResolutionX + ShadowResolutionFractionX,
				(TileOffsetY + BorderSize) * InvBufferResolutionY + ShadowResolutionFractionY,
				0,
				1
			)
		);
	ScreenToShadowMatrix.Transpose();

	ShadowMapProjectionMatrix.Transpose();
}

void RenderPrePass()
{
	//D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, SceneDepthDSV);
	D3D11DeviceContext->OMSetRenderTargets(0, NULL, SceneDepthDSV);
	const FLOAT ClearColor[] = { 0.f,0.f,0.0f,1.f };
	//D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(SceneDepthDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

	const ParameterAllocation& ViewParams = PrePassVSParams.at("View");
	const ParameterAllocation& PrimitiveParams = PrePassVSParams.at("Primitive");

	D3D11DeviceContext->VSSetConstantBuffers(ViewParams.BufferIndex, 1, &ViewUniformBuffer);

	D3D11DeviceContext->RSSetState(PrePassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState(PrePassBlendState,);
	D3D11DeviceContext->OMSetDepthStencilState(PrePassDepthStencilState, 0);


	for (MeshBatch& MB : AllBatches)
	{
		D3D11DeviceContext->IASetInputLayout(PositionOnlyMeshInputLayout);
		D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT Stride = sizeof(PositionOnlyLocalVertex);
		UINT Offset = 0;
		D3D11DeviceContext->IASetVertexBuffers(0, 1, &MB.PositionOnlyVertexBuffer, &Stride, &Offset);
		D3D11DeviceContext->IASetIndexBuffer(MB.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11DeviceContext->VSSetShader(PrePassVS, 0, 0);
		D3D11DeviceContext->PSSetShader(PrePassPS, 0, 0);

		for (size_t Element = 0; Element < MB.Elements.size(); ++Element)
		{
			D3D11DeviceContext->VSSetConstantBuffers(PrimitiveParams.BufferIndex, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
			D3D11DeviceContext->DrawIndexed(MB.Elements[Element].NumTriangles * 3, MB.Elements[Element].FirstIndex, 0);
		}
	}
}

void RenderShadowProjection()
{
	D3D11DeviceContext->OMSetRenderTargets(1, &ShadowProjectionRTV, NULL);
	//D3D11DeviceContext->OMSetDepthStencilState(NULL, 0);
	const FLOAT ClearColor[] = { 0.f,0.f,0.0f,1.f };
	D3D11DeviceContext->ClearRenderTargetView(ShadowProjectionRTV, ClearColor);

	D3D11DeviceContext->RSSetState(TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE, FALSE>::GetRHI());
	//D3D11DeviceContext->OMSetBlendState(TStaticBlendState<>::GetRHI(),NULL,0);
	D3D11DeviceContext->OMSetDepthStencilState(TStaticDepthStencilState<false, D3D11_COMPARISON_ALWAYS>::GetRHI(), 0);

	D3D11DeviceContext->IASetInputLayout(ShadowProjectionInputLayout);
	D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	const ParameterAllocation& ShadowDepthTexture = ShadowProjectionPSParams.at("ShadowDepthTexture");
	const ParameterAllocation& ShadowDepthTextureSampler = ShadowProjectionPSParams.at("ShadowDepthTextureSampler");
	const ParameterAllocation& SceneDepthTextureParam = ShadowProjectionPSParams.at("SceneTexturesStruct_SceneDepthTexture");
	const ParameterAllocation& SceneDepthTextureSamplerParam = ShadowProjectionPSParams.at("SceneTexturesStruct_SceneDepthTextureSampler");
	const ParameterAllocation& ViewParam = ShadowProjectionPSParams.at("View");
	D3D11DeviceContext->PSSetShaderResources(ShadowDepthTexture.BaseIndex, ShadowDepthTexture.Size, &ShadowPassDepthSRV);
	ShadowPassDepthSamplerState = TStaticSamplerState<>::GetRHI();
	D3D11DeviceContext->PSSetSamplers(ShadowDepthTextureSampler.BaseIndex, ShadowDepthTextureSampler.Size, &ShadowPassDepthSamplerState);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &LightPassSceneDepthSRV);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &LightPassSceneDepthSamplerState);
	D3D11DeviceContext->PSSetConstantBuffers(ViewParam.BufferIndex, 1, &ViewUniformBuffer);

	const ParameterAllocation& ShadowFadeFractionParam = ShadowProjectionPSParams.at("ShadowFadeFraction");
	const ParameterAllocation& ShadowSharpenParam = ShadowProjectionPSParams.at("ShadowSharpen");
	const ParameterAllocation& ScreenToShadowMatrixParam = ShadowProjectionPSParams.at("ScreenToShadowMatrix");
	float fShadowFadeFraction = 0.8f;
	float fShadowSharpen = 0.9f;

	memcpy(GlobalConstantBufferData + ShadowFadeFractionParam.BaseIndex, &fShadowFadeFraction, ShadowFadeFractionParam.Size);
	memcpy(GlobalConstantBufferData + ShadowSharpenParam.BaseIndex, &fShadowSharpen, ShadowSharpenParam.Size);
	memcpy(GlobalConstantBufferData + ScreenToShadowMatrixParam.BaseIndex, &ScreenToShadowMatrix, ScreenToShadowMatrixParam.Size);

	//https://gamedev.stackexchange.com/questions/184702/direct3d-constant-buffer-not-updating
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
	D3D11DeviceContext->UpdateSubresource(GlobalConstantBuffer,0,NULL, GlobalConstantBufferData, 0, 0);
	D3D11DeviceContext->PSSetConstantBuffers(ShadowFadeFractionParam.BufferIndex, 1, &GlobalConstantBuffer);

	D3D11DeviceContext->VSSetShader(ShadowProjectionVS, 0, 0);
	D3D11DeviceContext->PSSetShader(ShadowProjectionPS, 0, 0);

	Vector4 Verts[4] = 
	{
		Vector4(-1.0f, 1.0f, 0.0f),
		Vector4(1.0f, 1.0f, 0.0f),
		Vector4(-1.0f, -1.0f, 0.0f),
		Vector4(1.0f, -1.0f, 0.0f),
	};
	//https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--use-dynamic-resources
	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof(SubResource));
	D3D11DeviceContext->Map(DynamicVertexBuffer,0,D3D11_MAP_WRITE_DISCARD,0, &SubResource);
	memcpy(SubResource.pData, Verts, sizeof(Verts));
	D3D11DeviceContext->Unmap(DynamicVertexBuffer, 0);
	UINT Stride = sizeof(Vector4);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0, 1, &DynamicVertexBuffer, &Stride, &Offset);

	D3D11DeviceContext->Draw(4, 0);
	//reset
	ID3D11ShaderResourceView* SRV = NULL;
	ID3D11SamplerState* Sampler = NULL;
	D3D11DeviceContext->PSSetShaderResources(ShadowDepthTexture.BaseIndex, ShadowDepthTexture.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(ShadowDepthTextureSampler.BaseIndex, ShadowDepthTextureSampler.Size, &Sampler);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &Sampler);
}

void RenderShadowPass()
{

	D3D11DeviceContext->OMSetRenderTargets(0, NULL, ShadowPassDSV);
	D3D11DeviceContext->ClearDepthStencilView(ShadowPassDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	const ParameterAllocation& ViewParams = ShadowPassVSParams.at("View");
	D3D11DeviceContext->VSSetConstantBuffers(ViewParams.BufferIndex, 1, &ViewUniformBuffer);

	const ParameterAllocation& ProjectionMatrixParams = ShadowPassVSParams.at("ProjectionMatrix");
	memcpy(GlobalConstantBufferData + ProjectionMatrixParams.BaseIndex, &ShadowMapProjectionMatrix, ProjectionMatrixParams.Size);
	D3D11DeviceContext->UpdateSubresource(GlobalConstantBuffer, 0, NULL, GlobalConstantBufferData, 0, 0);
	D3D11DeviceContext->VSSetConstantBuffers(ProjectionMatrixParams.BufferIndex, 1, &GlobalConstantBuffer);

	D3D11DeviceContext->RSSetState(PrePassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState(PrePassBlendState,);
	D3D11DeviceContext->OMSetDepthStencilState(ShadowPassDepthStencilState, 0);

	const ParameterAllocation& PrimitiveParams = ShadowPassVSParams.at("Primitive");

	for (MeshBatch& MB : AllBatches)
	{
		D3D11DeviceContext->IASetInputLayout(PositionOnlyMeshInputLayout);
		D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT Stride = sizeof(PositionOnlyLocalVertex);
		UINT Offset = 0;
		D3D11DeviceContext->IASetVertexBuffers(0, 1, &MB.PositionOnlyVertexBuffer, &Stride, &Offset);
		D3D11DeviceContext->IASetIndexBuffer(MB.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11DeviceContext->VSSetShader(ShadowPassVS, 0, 0);
		D3D11DeviceContext->PSSetShader(ShadowPassPS, 0, 0);

		for (size_t Element = 0; Element < MB.Elements.size(); ++Element)
		{
			D3D11DeviceContext->VSSetConstantBuffers(PrimitiveParams.BufferIndex, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
			D3D11DeviceContext->DrawIndexed(MB.Elements[Element].NumTriangles * 3, MB.Elements[Element].FirstIndex, 0);
		}
	}

	RenderShadowProjection();
}

void RenderBasePass()
{
	ID3D11RenderTargetView* MRT[4];
	MRT[0] = BasePassSceneColorRTV;
	MRT[1] = BasePassGBufferRTV[0];
	MRT[2] = BasePassGBufferRTV[1];
	MRT[3] = BasePassGBufferRTV[2];
	D3D11DeviceContext->OMSetRenderTargets(4, MRT, SceneDepthDSV);
	const FLOAT ClearColor[] = { 0.f,0.f,0.f,0.f };
	for (ID3D11RenderTargetView* RTV : MRT)
	{
		if(RTV) D3D11DeviceContext->ClearRenderTargetView(RTV, ClearColor);
	}
	//D3D11DeviceContext->ClearDepthStencilView(SceneDepthDSV, D3D11_CLEAR_DEPTH, 1.f, 0);
	const ParameterAllocation& VSViewParam = BasePassVSParams.at("View");
	const ParameterAllocation& VSPrecomputedLightingParameters = BasePassVSParams.at("PrecomputedLightingParameters");
	D3D11DeviceContext->VSSetConstantBuffers(VSViewParam.BufferIndex, 1, &ViewUniformBuffer);
	D3D11DeviceContext->VSSetConstantBuffers(VSPrecomputedLightingParameters.BufferIndex, 1, &PrecomputedLightUniformBuffer);

	D3D11DeviceContext->RSSetState(BasePassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState();
	D3D11DeviceContext->OMSetDepthStencilState(BasePassDepthStencilState,0);

	const ParameterAllocation& PSViewParam = BasePassPSParams.at("View");
	const ParameterAllocation& PrecomputedLightingBuffer = BasePassPSParams.at("PrecomputedLightingParameters");
	const ParameterAllocation& BaseColorParam = BasePassPSParams.at("Material_BaseColor");
	const ParameterAllocation& BaseColorSamplerParam = BasePassPSParams.at("Material_BaseColorSampler");
	const ParameterAllocation& LightMapTextureParam = BasePassPSParams.at("PrecomputedLighting_LightMapTexture");
	const ParameterAllocation& LightMapSamplerParam = BasePassPSParams.at("PrecomputedLighting_LightMapSampler");
// 	const ParameterAllocation& StaticShadowTextureParam = BasePassParams["PrecomputedLighting_StaticShadowTexture"];
// 	const ParameterAllocation& StaticShadowTextureSamplerParam = BasePassParams["PrecomputedLighting_StaticShadowTextureSampler"];

	D3D11DeviceContext->PSSetConstantBuffers(PSViewParam.BufferIndex, 1, &ViewUniformBuffer);
	D3D11DeviceContext->PSSetConstantBuffers(PrecomputedLightingBuffer.BufferIndex, 1, &PrecomputedLightUniformBuffer);
	D3D11DeviceContext->PSSetShaderResources(BaseColorParam.BaseIndex, BaseColorParam.Size, &BaseColorSRV);
	D3D11DeviceContext->PSSetSamplers(BaseColorSamplerParam.BaseIndex, BaseColorSamplerParam.Size, &BaseColorSampler);
	D3D11DeviceContext->PSSetShaderResources(LightMapTextureParam.BaseIndex, LightMapTextureParam.Size, &PrecomputedLighting_LightMapTexture);
	D3D11DeviceContext->PSSetSamplers(LightMapSamplerParam.BaseIndex, LightMapSamplerParam.Size, &PrecomputedLighting_LightMapSampler);
// 	D3D11DeviceContext->PSSetShaderResources(StaticShadowTextureParam.BaseIndex, StaticShadowTextureParam.Size, &PrecomputedLighting_StaticShadowTexture);
// 	D3D11DeviceContext->PSSetSamplers(StaticShadowTextureSamplerParam.BaseIndex, StaticShadowTextureSamplerParam.Size, &PrecomputedLighting_StaticShadowTextureSampler);

	const ParameterAllocation& PrimitiveParam = BasePassVSParams.at("Primitive");
	const ParameterAllocation& PSPrimitiveParam = BasePassPSParams.at("Primitive");

	for (MeshBatch& MB : AllBatches)
	{
		D3D11DeviceContext->IASetInputLayout(MeshInputLayout);
		D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT Stride = sizeof(LocalVertex);
		UINT Offset = 0;
		D3D11DeviceContext->IASetVertexBuffers(0, 1, &MB.VertexBuffer, &Stride, &Offset);
		D3D11DeviceContext->IASetIndexBuffer(MB.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11DeviceContext->VSSetShader(BasePassVS, 0, 0);
		D3D11DeviceContext->PSSetShader(BasePassPS, 0, 0);

		for (size_t Element = 0; Element < MB.Elements.size(); ++Element)
		{
			D3D11DeviceContext->VSSetConstantBuffers(PrimitiveParam.BufferIndex, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
			D3D11DeviceContext->PSSetConstantBuffers(PSPrimitiveParam.BufferIndex, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
			D3D11DeviceContext->DrawIndexed(MB.Elements[Element].NumTriangles * 3, MB.Elements[Element].FirstIndex, 0);
		}

	}
}

void RenderLight()
{
	D3D11DeviceContext->OMSetRenderTargets(0, NULL, NULL);
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
	const FLOAT ClearColor[] = { 0.f,0.f,0.f,0.f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);

	D3D11DeviceContext->RSSetState(LightPassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState();
	//D3D11DeviceContext->OMSetDepthStencilState(LightPassDepthStencilState, 0);

	D3D11DeviceContext->IASetInputLayout(RectangleInputLayout);
	D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT Stride = sizeof(RectangleVertex);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0, 1, &ScreenRect.VertexBuffer, &Stride, &Offset);
	D3D11DeviceContext->IASetIndexBuffer(ScreenRect.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3D11DeviceContext->VSSetShader(LightPassVS, 0, 0);
	D3D11DeviceContext->PSSetShader(LightPassPS, 0, 0);


	const ParameterAllocation& VSViewParam = LightPassVSParams.at("View");
	const ParameterAllocation& DrawRectangleParametersParam = LightPassVSParams.at("DrawRectangleParameters");
	D3D11DeviceContext->VSSetConstantBuffers(VSViewParam.BufferIndex, 1, &ViewUniformBuffer);
	D3D11DeviceContext->VSSetConstantBuffers(DrawRectangleParametersParam.BufferIndex, 1, &ScreenRect.DrawRectangleParameters);

	const ParameterAllocation& LightAttenuationTextureParam = LightPassPSParams.at("LightAttenuationTexture");
	const ParameterAllocation& LightAttenuationTextureSamplerParam = LightPassPSParams.at("LightAttenuationTextureSampler");
	const ParameterAllocation& SceneColorTextureParam = LightPassPSParams.at("SceneTexturesStruct_SceneColorTexture");
	const ParameterAllocation& SceneColorTextureSamplerParam = LightPassPSParams.at("SceneTexturesStruct_SceneColorTextureSampler");
	const ParameterAllocation& SceneDepthTextureParam = LightPassPSParams.at("SceneTexturesStruct_SceneDepthTexture");
	const ParameterAllocation& SceneDepthTextureSamplerParam = LightPassPSParams.at("SceneTexturesStruct_SceneDepthTextureSampler");
	const ParameterAllocation& GBufferATextureParam = LightPassPSParams.at("SceneTexturesStruct_GBufferATexture");
	const ParameterAllocation& GBufferATextureSamplerParam = LightPassPSParams.at("SceneTexturesStruct_GBufferATextureSampler");
	const ParameterAllocation& DeferredLightUniformParam = LightPassPSParams.at("DeferredLightUniform");
	const ParameterAllocation& PSViewParam = LightPassPSParams.at("View");

	D3D11DeviceContext->PSSetShaderResources(SceneColorTextureParam.BaseIndex, SceneColorTextureParam.Size, &LightPassSceneColorSRV);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &LightPassSceneDepthSRV);
	D3D11DeviceContext->PSSetSamplers(SceneColorTextureSamplerParam.BaseIndex, SceneColorTextureSamplerParam.Size, &LightPassSceneColorSamplerState);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &LightPassSceneDepthSamplerState);

	D3D11DeviceContext->PSSetShaderResources(GBufferATextureParam.BaseIndex, 3, LightPassGBufferSRV);
	D3D11DeviceContext->PSSetSamplers(GBufferATextureSamplerParam.BaseIndex, 3, LightPassGBufferSamplerState);

	D3D11DeviceContext->PSSetShaderResources(LightAttenuationTextureParam.BaseIndex, LightAttenuationTextureParam.Size, &LightAttenuationSRV);
	D3D11DeviceContext->PSSetSamplers(LightAttenuationTextureSamplerParam.BaseIndex, LightAttenuationTextureSamplerParam.Size, &LightAttenuationSampleState );
	D3D11DeviceContext->PSSetConstantBuffers(DeferredLightUniformParam.BufferIndex, 1, &DeferredLightUniformBuffer);
	D3D11DeviceContext->PSSetConstantBuffers(PSViewParam.BufferIndex, 1, &ViewUniformBuffer);


	D3D11DeviceContext->DrawIndexed(6, 0, 0);

	//reset
	ID3D11ShaderResourceView* SRV = NULL;
	ID3D11SamplerState* Sampler = NULL;

	D3D11DeviceContext->PSSetShaderResources(SceneColorTextureParam.BaseIndex, SceneColorTextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetShaderResources(SceneDepthTextureParam.BaseIndex, SceneDepthTextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(SceneColorTextureSamplerParam.BaseIndex, SceneColorTextureSamplerParam.Size, &Sampler);
	D3D11DeviceContext->PSSetSamplers(SceneDepthTextureSamplerParam.BaseIndex, SceneDepthTextureSamplerParam.Size, &Sampler);

	D3D11DeviceContext->PSSetShaderResources(LightAttenuationTextureParam.BaseIndex, LightAttenuationTextureParam.Size, &SRV);
	D3D11DeviceContext->PSSetSamplers(LightAttenuationTextureSamplerParam.BaseIndex, LightAttenuationTextureSamplerParam.Size, &Sampler);

	for (int i = 0; i < 3; ++i)
	{
		D3D11DeviceContext->PSSetShaderResources(GBufferATextureParam.BaseIndex + i, 1, &SRV);
		D3D11DeviceContext->PSSetSamplers(GBufferATextureSamplerParam.BaseIndex + i, 1, &Sampler);
	}
}


