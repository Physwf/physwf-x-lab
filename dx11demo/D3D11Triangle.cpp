#include <dxgi.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <stdio.h>
#include "log.h"

IDXGIFactory*	DXGIFactory = NULL;
IDXGIAdapter*	DXGIAdapter = NULL;
IDXGIOutput*	DXGIOutput = NULL;
IDXGISwapChain* DXGISwapChain = NULL;

ID3D11Device*			D3D11Device = NULL;
ID3D11DeviceContext*	D3D11DeviceContext = NULL;

UINT WindowWidth = 500;
UINT WindowHeight = 400;

bool D3D11Setup()
{
	//create DXGIFactory
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&DXGIFactory);
	if (FAILED(hr))
	{
		X_LOG("CreateDXGIFactory failed!");
		return FALSE;
	}
	//EnumAapter
	for (UINT AdpaterIndex = 0; SUCCEEDED(DXGIFactory->EnumAdapters(AdpaterIndex, &DXGIAdapter)); ++AdpaterIndex)
	{
		if (DXGIAdapter)
		{
			DXGI_ADAPTER_DESC DESC;
			if (SUCCEEDED(DXGIAdapter->GetDesc(&DESC)))
			{
				X_LOG("Adapter %d: Description:%ls  VendorId:%x, DeviceId:0x%x, SubSysId:0x%x Revision:%u DedicatedVideoMemory:%uM DedicatedSystemMemory:%uM SharedSystemMemory:%uM \n",
					AdpaterIndex,
					DESC.Description,
					DESC.VendorId,
					DESC.DeviceId,
					DESC.SubSysId,
					DESC.Revision,
					DESC.DedicatedVideoMemory / (1024 * 1024),
					DESC.DedicatedSystemMemory / (1024 * 1024),
					DESC.SharedSystemMemory / (1024 * 1024)
				/*DESC.AdapterLuid*/);
			}
			
			for (UINT OutputIndex = 0; SUCCEEDED(DXGIAdapter->EnumOutputs(OutputIndex, &DXGIOutput)); ++OutputIndex)
			{
				if (DXGIOutput)
				{
					DXGI_OUTPUT_DESC OutDesc;
					if (SUCCEEDED(DXGIOutput->GetDesc(&OutDesc)))
					{
						X_LOG("\tOutput:%u, DeviceName:%ls, Width:%u Height:%u \n", OutputIndex, OutDesc.DeviceName, OutDesc.DesktopCoordinates.right - OutDesc.DesktopCoordinates.left, OutDesc.DesktopCoordinates.bottom - OutDesc.DesktopCoordinates.top);
					}
				}
			}
			break;
		}
	}

	//Create D3D11Device
	if (DXGIAdapter)
	{
		D3D_FEATURE_LEVEL FeatureLevels[] = 
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		D3D_FEATURE_LEVEL OutFeatureLevel;
		hr = D3D11CreateDevice(
			DXGIAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			FeatureLevels,
			6,
			D3D11_SDK_VERSION,
			&D3D11Device,
			&OutFeatureLevel,
			&D3D11DeviceContext
		);

		if (FAILED(hr))
		{
			X_LOG("D3D11CreateDevice failed!");
			return false;
		}

		extern HWND g_hWind;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof SwapChainDesc);
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.BufferDesc.Width = WindowWidth;
		SwapChainDesc.BufferDesc.Height = WindowHeight;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.OutputWindow = g_hWind;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		hr = DXGIFactory->CreateSwapChain(D3D11Device, &SwapChainDesc, &DXGISwapChain);
		if (FAILED(hr))
		{
			X_LOG("CreateSwapChain failed!");
			return false;
		}
	}
	return true;
}

ID3D11InputLayout* InputLayout = NULL;
ID3D11RenderTargetView* RenderTargetView = NULL;
D3D11_VIEWPORT Viewport;

ID3D11Buffer* VertexBuffer = NULL;
ID3D11Buffer* IndexBuffer = NULL;
ID3D11VertexShader* VertexShader = NULL;
ID3D11PixelShader* PixelShader = NULL;

void CreateTriangleBuffer()
{
	HRESULT hr;

	//create vertex buffer
	struct SimpleVertex
	{
		float x,y,z;
		float r,g,b,a;
	};

	SimpleVertex TriangleVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f,			1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.45f, -0.5, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f },
		{ -0.45f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f }
	};

	D3D11_BUFFER_DESC VertexDesc;
	ZeroMemory(&VertexDesc, sizeof(VertexDesc));
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexDesc.ByteWidth = sizeof(SimpleVertex) * 3;
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexDesc.CPUAccessFlags = 0;
	VertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = TriangleVertices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	hr = D3D11Device->CreateBuffer(&VertexDesc, &InitData, &VertexBuffer);
	if (FAILED(hr))
	{
		X_LOG("CreateBuffer for vertex failed!");
		return;
	}

	//create index buffer
	unsigned int Indices[] = { 0,1,2 };
	D3D11_BUFFER_DESC IndexDesc;
	ZeroMemory(&IndexDesc, sizeof(IndexDesc));
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.ByteWidth = sizeof(unsigned int) * 3;
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0;
	IndexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));
	IndexData.pSysMem = Indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	hr = D3D11Device->CreateBuffer(&IndexDesc, &IndexData, &IndexBuffer);
	if (FAILED(hr))
	{
		X_LOG("CreateBuffer for index failed!");
		return;
	}

	//create vertex shader
	ID3DBlob* VSBytecode;
	ID3DBlob* PSBytecode;
	ID3DBlob* OutErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(TEXT("Triangle.hlsl"), NULL, NULL, "VS_Main", VSTarget, VSFlags, 0, &VSBytecode, &OutErrorMsg);
	if (FAILED(hr))
	{
		X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
		return;
	}

	hr = D3D11Device->CreateVertexShader(VSBytecode->GetBufferPointer(), VSBytecode->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr))
	{
		X_LOG("CreateVertexShader failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
		return;
	}
	if (OutErrorMsg)
	{
		OutErrorMsg->Release();
	}

	//create pixel shader
	LPCSTR PSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0";
	UINT PSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	hr = D3DCompileFromFile(TEXT("Triangle.hlsl"), NULL, NULL, "PS_Main", PSTarget, PSFlags, 0, &PSBytecode, &OutErrorMsg);
	if (FAILED(hr))
	{
		X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
		return;
	}

	hr = D3D11Device->CreatePixelShader(PSBytecode->GetBufferPointer(), PSBytecode->GetBufferSize(), NULL, &PixelShader);
	if (FAILED(hr))
	{
		X_LOG("CreatePixelShader failed!");
		return;
	}
	if (OutErrorMsg)
	{
		OutErrorMsg->Release();
	}

	//create input layout
	D3D11_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	UINT NumElements = ARRAYSIZE(InputDesc);
	hr = D3D11Device->CreateInputLayout(InputDesc, NumElements, VSBytecode->GetBufferPointer(), VSBytecode->GetBufferSize(), &InputLayout);
	if (FAILED(hr))
	{
		X_LOG("CreateInputLayout failed!");
		return;
	}

	//VSBytecode->Release();
	//PSBytecode->Release();
	//create render target view
	ID3D11Texture2D* BackBuffer = NULL;
	hr = DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	if (FAILED(hr))
	{
		X_LOG("GetBuffer failed!");
		return;
	}
	hr = D3D11Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	BackBuffer->Release();

	Viewport.Width = (FLOAT)WindowWidth;
	Viewport.Height = (FLOAT)WindowHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	D3D11DeviceContext->IASetInputLayout(InputLayout);
	D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT Stride = sizeof(SimpleVertex);
	UINT Offset = 0;
	D3D11DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	//D3D11DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
	D3D11DeviceContext->RSSetViewports(1,&Viewport);

	D3D11DeviceContext->VSSetShader(VertexShader, 0, 0);
	D3D11DeviceContext->PSSetShader(PixelShader, 0, 0);
}

void RenderTriangle()
{
	float ClearColor[] = { 0.1f, 0.0f, 0.1f, 1.0f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	//D3D11DeviceContext->DrawIndexed(3, 0, 0);
	D3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11DeviceContext->Draw(3, 0);

	DXGISwapChain->Present(0, 0);
}