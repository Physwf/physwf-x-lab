#include "DeferredShading.h"
#include "Mesh.h"

std::vector<Mesh> AllMeshes;

struct ViewUniform
{
	Matrix View;
	Matrix Proj;
};
ID3D11Buffer* ViewUniformBuffer;

ID3D11InputLayout* MeshInputLayout;

ID3DBlob* PrePassVSBytecode;
ID3DBlob* PrePassPSBytecode;
ID3D11VertexShader* PrePassVS;
ID3D11PixelShader* PrePassPS;

ID3D11VertexShader* BasePassVS;
ID3D11PixelShader* BasePassPS;

void InitInput()
{
	Mesh m1;
	m1.ImportFromFBX("k526efluton4-House_15/247_House 15_fbx.FBX");
	//m1.GeneratePlane(100.f, 100.f, 1, 1);
	m1.SetPosition(0.0f, 0.0f, 500.0f);
	m1.SetRotation(-3.14f / 2, 0, 0);
	m1.InitResource();
	AllMeshes.push_back(m1);

	ViewUniform VU;
	VU.View = Matrix::DXLooAtLH(Vector(0,0,0),Vector(0,0,100), Vector(0, 1,0));
	VU.Proj = Matrix::DXFromPerspectiveFovLH(3.1415926f/2, 1.0, 1.0f, 10000.f);
	//VU.Proj = Matrix::DXFromPerspectiveLH(700, 700, 0.0f, 10000.f);
	ViewUniformBuffer = CreateConstantBuffer(&VU, sizeof(VU));

	PrePassVSBytecode = CompileVertexShader(TEXT("DepthOnlyPass.hlsl"), "VS_Main");
	PrePassPSBytecode = CompilePixelShader(TEXT("DepthOnlyPass.hlsl"), "PS_Main");
	
	D3D11_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{ "ATTRIBUTE",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	1,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	2,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 24, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "ATTRIBUTE",	3,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 36, D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	MeshInputLayout = CreateInputLayout(InputDesc, sizeof(InputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), PrePassVSBytecode);
	PrePassVS = CreateVertexShader(PrePassVSBytecode);
	PrePassPS = CreatePixelShader(PrePassPSBytecode);
}

void RenderPrePass()
{
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencialView);
	const FLOAT ClearColor[] = { 0.f,0.f,0.f,0.f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(DepthStencialView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D3D11DeviceContext->VSSetConstantBuffers(0, 1, &ViewUniformBuffer);

	for (Mesh& m : AllMeshes)
	{
		MeshRenderState RS = { MeshInputLayout, PrePassVS , PrePassPS };
		m.Draw(D3D11DeviceContext, RS);
	}
}

void RenderBasePass()
{

}

void RenderLight()
{

}


