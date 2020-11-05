#include "AtmosphereRendering.h"


ID3D11Buffer* AtmosphereFogVertexBuffer;
ID3D11Buffer* AtmosphereFogIndexBuffer;
ID3D11InputLayout* AtmosphereFogInputLayout;

ID3DBlob* AtmosphereFogVSBytecode;
ID3DBlob* AtmosphereFogPSBytecode;
ID3D11VertexShader* AtmosphereFogVertexShader;
ID3D11PixelShader* AtmosphereFogPixelShader;
ID3D11RasterizerState* AtmosphereFogRasterState;
ID3D11DepthStencilState* AtmosphereFogDepthStencilState;
ID3D11RenderTargetView* AtmosphereFogRTV;

std::map<std::string, ParameterAllocation> AtmosphereFogVSParams;
std::map<std::string, ParameterAllocation> AtmosphereFogPSParams;


void InitAtomosphereFog()
{
	Vector2 Vertices[] = {
		Vector2(-1,-1),
		Vector2(-1,+1),
		Vector2(+1,+1),
		Vector2(+1,-1),
	};
	uint16 Indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	AtmosphereFogVertexBuffer = CreateVertexBuffer(false, sizeof(Vertices), Vertices);
	AtmosphereFogIndexBuffer = CreateIndexBuffer(Indices, sizeof(Indices));

	AtmosphereFogVSBytecode = CompileVertexShader(TEXT("AtmosphericFogShader.hlsl"), "VS_Main");
	AtmosphereFogPSBytecode = CompilePixelShader(TEXT("AtmosphericFogShader.hlsl"), "PS_Main");
	GetShaderParameterAllocations(AtmosphereFogVSBytecode, AtmosphereFogVSParams);
	GetShaderParameterAllocations(AtmosphereFogPSBytecode, AtmosphereFogPSParams);
	D3D11_INPUT_ELEMENT_DESC InputDesc[] = 
	{
		"ATTRIBUTE",0,DXGI_FORMAT_R32G32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA,0,
	};
	AtmosphereFogInputLayout = CreateInputLayout(InputDesc, sizeof(InputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC), AtmosphereFogVSBytecode);
}

void RenderAtmosphereFog()
{

}

