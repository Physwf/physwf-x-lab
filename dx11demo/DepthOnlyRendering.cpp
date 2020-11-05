#include "DepthOnlyRendering.h"
#include "Scene.h"
#include "RenderTargets.h"

ID3D11InputLayout* PositionOnlyMeshInputLayout;

ID3DBlob* PrePassVSBytecode;
ID3DBlob* PrePassPSBytecode;
ID3D11VertexShader* PrePassVS;
ID3D11PixelShader* PrePassPS;
ID3D11RasterizerState* PrePassRasterizerState;
ID3D11BlendState* PrePassBlendState;
ID3D11DepthStencilState* PrePassDepthStencilState;

std::map<std::string, ParameterAllocation> PrePassVSParams;
std::map<std::string, ParameterAllocation> PrePassPSParams;


void InitPrePass()
{
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

	PrePassVS = CreateVertexShader(PrePassVSBytecode);
	PrePassPS = CreatePixelShader(PrePassPSBytecode);

	PrePassRasterizerState = TStaticRasterizerState<D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE, FALSE>::GetRHI();
	PrePassDepthStencilState = TStaticDepthStencilState<true, D3D11_COMPARISON_GREATER>::GetRHI();
	PrePassBlendState = TStaticBlendState<>::GetRHI();
}


void RenderPrePass()
{
	//D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, SceneDepthDSV);
	//D3D11DeviceContext->OMSetRenderTargets(0, NULL, SceneDepthDSV);
	//const FLOAT ClearColor[] = { 0.f,0.f,0.0f,1.f };
	//D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	//D3D11DeviceContext->ClearDepthStencilView(SceneDepthDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

	RenderTargets& SceneContex = RenderTargets::Get();
	SceneContex.BeginRenderingPrePass(true);

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
