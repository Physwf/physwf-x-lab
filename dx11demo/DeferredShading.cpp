#include "DeferredShading.h"
#include "Mesh.h"
#include "Light.h"
#include "Scene.h"

std::vector<Mesh> AllMeshes;
std::vector<MeshBatch> AllBatches;

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
		VertexBuffer = CreateVertexBuffer(Vertices, sizeof(Vertices));
		IndexBuffer = CreateIndexBuffer(Indices, sizeof(Indices));
		DrawRectangleParameters = CreateConstantBuffer(&UniformPrameters, sizeof(UniformPrameters));
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
	// half4 ScreenPositionScaleBias;
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

	uint32 Random;
	uint32 FrameNumber;
	uint32 StateFrameIndexMod8;
	uint32 ViewPading05;
};
#pragma pack(pop)
ID3D11Buffer* ViewUniformBuffer;

ID3D11InputLayout* PositionOnlyMeshInputLayout;
ID3D11InputLayout* MeshInputLayout;
ID3D11InputLayout* RectangleInputLayout;

ID3D11Texture2D* SceneDepthRT;
ID3D11DepthStencilView* SceneDepthDSV;

ID3DBlob* PrePassVSBytecode;
ID3DBlob* PrePassPSBytecode;
ID3D11VertexShader* PrePassVS;
ID3D11PixelShader* PrePassPS;
ID3D11RasterizerState* PrePassRasterizerState;
ID3D11BlendState* PrePassBlendState;
ID3D11DepthStencilState* PrePassDepthStencilState;

ID3DBlob* ShadowPassVSBytecode;
ID3DBlob* ShadowPassPSBytecode;
ID3D11DepthStencilView* ShadowPassDSV;
ID3D11VertexShader* ShadowPassVS;
ID3D11PixelShader* ShadowPassPS;
ID3D11RasterizerState* ShadowPassRasterizerState;
ID3D11BlendState* ShadowPassBlendState;
ID3D11DepthStencilState* ShadowPassDepthStencilState;

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

void InitInput()
{
	Mesh m1;
	//m1.ImportFromFBX("shaderBallNoCrease/shaderBall.fbx");
	m1.ImportFromFBX("k526efluton4-House_15/247_House 15_fbx.fbx");
	//m1.GeneratePlane(100.f, 100.f, 1, 1);
	m1.SetPosition(0.0f, 0.0f, 400.0f);
	m1.SetRotation(-3.14f , 0, 0);
	m1.InitResource();
	AllMeshes.push_back(m1);

	for (Mesh& m : AllMeshes)
	{
		m.DrawStaticElement();
	}

	//Matrix::DXFromPerspectiveFovLH(3.1415926f / 2, 1.0, 1.0f, 10000.f);
	Matrix ProjectionMatrix = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 2, 1.0, 100.0f, 600.f);
	//Matrix ProjectionMatrix = ReversedZPerspectiveMatrix(3.1415926f / 2.f, 3.1415926f / 2.f, 1.0f, 1.0f, 1.0,1.0f);
	Matrix ViewRotationMatrix = Matrix::DXLooAtLH(Vector(0, 0, 0), Vector(0, 0, 100), Vector(0, 1, 0));
	ViewMatrices VMs(Vector(0.0f, 0.0f, 0.0f), ViewRotationMatrix, ProjectionMatrix);
	Vector v = ProjectionMatrix.Transform(Vector(700, 700, 1.0f));
	Vector v1 = ProjectionMatrix.Transform(Vector(700, 700, 1000.0f));
	Vector v2 = ProjectionMatrix.Transform(Vector(700, 700, 100.0f));
	Vector v3 = ProjectionMatrix.Transform(Vector(700, 700, 500.0f));
	Vector v4 = ProjectionMatrix.Transform(Vector(700, -700, 50.0f));
	ViewUniform VU;
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

	ViewUniformBuffer = CreateConstantBuffer(&VU, sizeof(VU));

	SceneDepthRT = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R24G8_TYPELESS, false, true, true);
	SceneDepthDSV = CreateDepthStencilView2D(SceneDepthRT, DXGI_FORMAT_D24_UNORM_S8_UINT, 0);
	//Prepass
	PrePassVSBytecode = CompileVertexShader(TEXT("DepthOnlyPass.hlsl"), "VS_Main");
	PrePassPSBytecode = CompilePixelShader(TEXT("DepthOnlyPass.hlsl"), "PS_Main");
	
	D3D11_INPUT_ELEMENT_DESC PositionOnlyInputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	PositionOnlyMeshInputLayout = CreateInputLayout(PositionOnlyInputDesc, sizeof(PositionOnlyInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), PrePassVSBytecode);
	PrePassVS = CreateVertexShader(PrePassVSBytecode);
	PrePassPS = CreatePixelShader(PrePassPSBytecode);
	PrePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE>::GetRHI();
	PrePassDepthStencilState = TStaticDepthStencilState<true, D3D11_COMPARISON_GREATER>::GetRHI();
	PrePassBlendState = TStaticBlendState<>::GetRHI();
	//Base Pass
	BasePassVSBytecode = CompileVertexShader(TEXT("BasePassVertexShader.hlsl"), "VS_Main");
	BasePassPSBytecode = CompilePixelShader(TEXT("BasePassPixelShader.hlsl"), "PS_Main");

	D3D11_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	1,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 16, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	2,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	3,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	4,	DXGI_FORMAT_R32G32_FLOAT,		0, 64, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	5,	DXGI_FORMAT_R32G32_FLOAT,		0, 72, D3D11_INPUT_PER_VERTEX_DATA,0 },
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

	BasePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID,D3D11_CULL_BACK,FALSE, FALSE>::GetRHI();
	BasePassDepthStencilState = TStaticDepthStencilState<false, D3D11_COMPARISON_GREATER_EQUAL>::GetRHI();
	BasePassBlendState = TStaticBlendState<>::GetRHI();

	LightPassVSByteCode = CompileVertexShader(TEXT("DeferredLightVertexShader.hlsl"), "VS_Main");
	LightPassPSByteCode = CompilePixelShader(TEXT("DeferredLightPixelShader.hlsl"), "PS_Main");

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

	LightPassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE>::GetRHI();
	LightPassDepthStencilState = TStaticDepthStencilState<false, D3D11_COMPARISON_ALWAYS>::GetRHI();
	LightPassBlendState = TStaticBlendState<>::GetRHI();

	DirLight.Color = Vector(1.0f,0.0f,0.0f);
	DirLight.Direction = Vector(0.0f,0.0f,1.0f);
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
	DLU.ShadowedBits = 2;
	DeferredLightUniformBuffer = CreateConstantBuffer(&DLU, sizeof(DLU));
}

void RenderPrePass()
{
	//D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, SceneDepthDSV);
	D3D11DeviceContext->OMSetRenderTargets(0, NULL, SceneDepthDSV);
	const FLOAT ClearColor[] = { 0.f,0.f,0.0f,1.f };
	//D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(SceneDepthDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ViewUniformBuffer);

	//D3D11DeviceContext->RSSetState(PrePassRasterizerState);
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
			D3D11DeviceContext->VSSetConstantBuffers(1, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
			D3D11DeviceContext->DrawIndexed(MB.Elements[Element].NumTriangles * 3, MB.Elements[Element].FirstIndex, 0);
		}
	}
}

void RenderShadowPass()
{
 
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
	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ViewUniformBuffer);

	D3D11DeviceContext->RSSetState(BasePassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState();
	D3D11DeviceContext->OMSetDepthStencilState(BasePassDepthStencilState,0);

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
			D3D11DeviceContext->VSSetConstantBuffers(1, 1, &MB.Elements[Element].PrimitiveUniformBuffer);
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

	//D3D11DeviceContext->PSSetShaderResources(0, 1, &LightPassSceneColorSRV);
	D3D11DeviceContext->PSSetShaderResources(1, 1, &LightPassSceneDepthSRV);
	//D3D11DeviceContext->PSSetSamplers(0, 1, &LightPassSceneColorSamplerState);
	D3D11DeviceContext->PSSetSamplers(1, 1, &LightPassSceneDepthSamplerState);

	D3D11DeviceContext->PSSetShaderResources(3, 3, LightPassGBufferSRV);
	D3D11DeviceContext->PSSetSamplers(3, 3, LightPassGBufferSamplerState);

	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ViewUniformBuffer);
	D3D11DeviceContext->VSSetConstantBuffers(2, 1, &ScreenRect.DrawRectangleParameters);

	D3D11DeviceContext->PSSetConstantBuffers(0, 1, &ViewUniformBuffer);
	D3D11DeviceContext->PSSetConstantBuffers(2, 1, &DeferredLightUniformBuffer);
	D3D11DeviceContext->DrawIndexed(6, 0, 0);

	//reset
	ID3D11ShaderResourceView* SRV = NULL;
	ID3D11SamplerState* Sampler = NULL;
	D3D11DeviceContext->PSSetShaderResources(0, 1, &SRV);
	D3D11DeviceContext->PSSetShaderResources(1, 1, &SRV);
	D3D11DeviceContext->PSSetSamplers(0, 1, &Sampler);
	D3D11DeviceContext->PSSetSamplers(1, 1, &Sampler);

	for (int i = 3; i < 6; ++i)
	{
		D3D11DeviceContext->PSSetShaderResources(i, 1, &SRV);
		D3D11DeviceContext->PSSetSamplers(i, 1, &Sampler);
	}
}


