#include "DeferredShading.h"
#include "Mesh.h"

std::vector<Mesh> AllMeshes;
std::vector<MeshBatch> AllBatches;

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
	// float4 InvDeviceZToWorldZTransform;
	// half4 ScreenPositionScaleBias;
	// float4 WorldCameraOrigin;
	// float4 TranslatedWorldCameraOrigin;
	// float4 WorldViewOrigin;

	Vector PreViewTranslation;
	//float Padding;
	Vector4 ViewRectMin;
	Vector4 ViewSizeAndInvSize;
};
ID3D11Buffer* ViewUniformBuffer;

ID3D11InputLayout* PositionOnlyMeshInputLayout;

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
ID3D11Texture2D* BasePassRT[8];
ID3D11RenderTargetView* BasePassRTV[8];
ID3D11VertexShader* BasePassVS;
ID3D11PixelShader* BasePassPS;
ID3D11RasterizerState* BasePassRasterizerState;
ID3D11BlendState* BasePassBlendState;
ID3D11DepthStencilState* BasePassDepthStencilState;

void InitInput()
{
	Mesh m1;
	//m1.ImportFromFBX("shaderBallNoCrease/shaderBall.fbx");
	m1.ImportFromFBX("k526efluton4-House_15/247_House 15_fbx.fbx");
	//m1.GeneratePlane(100.f, 100.f, 1, 1);
	m1.SetPosition(0.0f, 0.0f, 500.0f);
	m1.SetRotation(-3.14f / 2, 0, 0);
	m1.InitResource();
	AllMeshes.push_back(m1);

	for (Mesh& m : AllMeshes)
	{
		m.DrawStaticElement();
	}

	ViewUniform VU;
	VU.TranslatedWorldToView = Matrix::DXLooAtLH(Vector(0, 0, 0), Vector(0, 0, 100), Vector(0, 1, 0));
	VU.ViewToClip = Matrix::DXFromPerspectiveFovLH(3.1415926f / 2, 1.0, 1.0f, 10000.f);
	VU.ViewToClip = Matrix::DXReversedZFromPerspectiveFovLH(3.1415926f / 2, 1.0, 1.0f, 10000.f);
	ViewUniformBuffer = CreateConstantBuffer(&VU, sizeof(VU));

	PrePassVSBytecode = CompileVertexShader(TEXT("DepthOnlyPass.hlsl"), "VS_Main");
	PrePassPSBytecode = CompilePixelShader(TEXT("DepthOnlyPass.hlsl"), "PS_Main");
	
	D3D11_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
// 		{ "ATTRIBUTE",	1,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 16, D3D11_INPUT_PER_VERTEX_DATA,0 },
// 		{ "ATTRIBUTE",	2,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA,0 },
// 		{ "ATTRIBUTE",	3,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48, D3D11_INPUT_PER_VERTEX_DATA,0 },
// 		{ "ATTRIBUTE",	4,	DXGI_FORMAT_R32G32_FLOAT,		0, 64, D3D11_INPUT_PER_VERTEX_DATA,0 },
// 		{ "ATTRIBUTE",	5,	DXGI_FORMAT_R32G32_FLOAT,		0, 72, D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	PositionOnlyMeshInputLayout = CreateInputLayout(InputDesc, sizeof(InputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), PrePassVSBytecode);
	PrePassVS = CreateVertexShader(PrePassVSBytecode);
	PrePassPS = CreatePixelShader(PrePassPSBytecode);
	PrePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE>::GetRHI();
	PrePassDepthStencilState = TStaticDepthStencilState<true, D3D11_COMPARISON_GREATER>::GetRHI();
	PrePassBlendState = TStaticBlendState<>::GetRHI();


// 	BasePassRT[0] = RenderTargetTexture;
// 	BasePassRT[1] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//WorldNormal(3),PerObjectGBufferData(1)
// 	BasePassRT[2] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//metalic(1),specular(1),Roughness(1),ShadingModelID|SelectiveOutputMask(1)
// 	BasePassRT[3] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//BaseColor(3),GBufferAO(1)
// 	BasePassRT[4] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//CustomData(4)
// 	BasePassRT[5] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//PrecomputedShadowFactors(4)
// 	BasePassRT[6] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//Velocity(4)
// 	BasePassRT[7] = CreateTexture2D(WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);//
// 
// 	BasePassRTV[0] = RenderTargetView;
// 	BasePassRTV[1] = CreateRenderTargetView(BasePassRT[1], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[2] = CreateRenderTargetView(BasePassRT[2], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[3] = CreateRenderTargetView(BasePassRT[3], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[4] = CreateRenderTargetView(BasePassRT[4], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[5] = CreateRenderTargetView(BasePassRT[5], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[6] = CreateRenderTargetView(BasePassRT[6], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 	BasePassRTV[7] = CreateRenderTargetView(BasePassRT[7], DXGI_FORMAT_R32G32B32A32_FLOAT);
// 
// 	BasePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID,D3D11_CULL_NONE,FALSE>::GetRHI();
// 	BasePassDepthStencilState = TStaticDepthStencilState<false, D3D11_COMPARISON_GREATER>::GetRHI();
// 	BasePassBlendState = TStaticBlendState<>::GetRHI();
}

void RenderPrePass()
{
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencialView);
	const FLOAT ClearColor[] = { 0.f,0.f,0.0f,1.f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(DepthStencialView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ViewUniformBuffer);

	//D3D11DeviceContext->RSSetState(PrePassRasterizerState);
	//D3D11DeviceContext->OMSetBlendState(PrePassBlendState,);
	//D3D11DeviceContext->OMSetDepthStencilState(PrePassDepthStencilState, 0);


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
	D3D11DeviceContext->OMSetRenderTargets(8, BasePassRTV, DepthStencialView);
	const FLOAT ClearColor[] = { 0.f,0.f,0.f,0.f };
	for (ID3D11RenderTargetView* RTV : BasePassRTV)
	{
		D3D11DeviceContext->ClearRenderTargetView(RTV, ClearColor);
	}
	D3D11DeviceContext->OMSetDepthStencilState(BasePassDepthStencilState,0);
	//D3D11DeviceContext->ClearDepthStencilView(DepthStencialView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (MeshBatch& MB : AllBatches)
	{
		D3D11DeviceContext->IASetInputLayout(PositionOnlyMeshInputLayout);
		D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT Stride = sizeof(StaticMeshBuildVertex);
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

}


