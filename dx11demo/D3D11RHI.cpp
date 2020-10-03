#include "D3D11RHI.h"
#include "log.h"
#include <D3Dcompiler.h>
#include <memory>
#include <string>
#include <stdio.h>
#include <fstream>

IDXGIFactory*	DXGIFactory = NULL;
IDXGIAdapter*	DXGIAdapter = NULL;
IDXGIOutput*	DXGIOutput = NULL;
IDXGISwapChain* DXGISwapChain = NULL;

ID3D11Device*			D3D11Device = NULL;
ID3D11DeviceContext*	D3D11DeviceContext = NULL;

ID3D11RenderTargetView* RenderTargetView = NULL;
ID3D11DepthStencilView* DepthStencialView = NULL;
ID3D11Texture2D* RenderTargetTexture = NULL; 
ID3D11Texture2D* DepthStencialTexture = NULL;
D3D11_VIEWPORT Viewport;
ID3D11RasterizerState* RasterState;

LONG WindowWidth = 720;
LONG WindowHeight = 720;

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

		UINT NumQualityLevels = 0;
		D3D11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,4,&NumQualityLevels);

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
		SwapChainDesc.SampleDesc.Count = 8;
		SwapChainDesc.SampleDesc.Quality = NumQualityLevels-1;
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

	hr = DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture);
	if (FAILED(hr))
	{
		X_LOG("GetBuffer failed!");
		return false;
	}
	hr = D3D11Device->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
	if (FAILED(hr))
	{
		X_LOG("CreateRenderTargetView failed!");
		return false;
	}


	D3D11_TEXTURE2D_DESC DepthStencialDesc;
	ZeroMemory(&DepthStencialDesc, sizeof(D3D11_TEXTURE2D_DESC));
	DepthStencialDesc.Width = WindowWidth;
	DepthStencialDesc.Height = WindowHeight;
	DepthStencialDesc.MipLevels = 1;
	DepthStencialDesc.ArraySize = 1;
	DepthStencialDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencialDesc.SampleDesc.Count = 8;
	DepthStencialDesc.SampleDesc.Quality = 0;
	DepthStencialDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencialDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//DepthStencialDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//DepthStencialDesc.MiscFlags = 
	
	hr = D3D11Device->CreateTexture2D(&DepthStencialDesc, 0, &DepthStencialTexture);
	if (FAILED(hr))
	{
		X_LOG("CreateTexture2D failed!");
		return false;
	}
	hr = D3D11Device->CreateDepthStencilView(DepthStencialTexture,0,&DepthStencialView);
	if (FAILED(hr))
	{
		X_LOG("CreateDepthStencilView failed!");
		return false;
	}

	Viewport.Width = (FLOAT)WindowWidth;
	Viewport.Height = (FLOAT)WindowHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	D3D11DeviceContext->RSSetViewports(1, &Viewport);

	return true;
}

void D3D11ClearViewTarget()
{
	float ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(DepthStencialView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void D3D11Present()
{
	DXGISwapChain->Present(0, 0);
}

ID3D11Buffer* CreateVertexBuffer(void* Data, unsigned int Size)
{
	D3D11_BUFFER_DESC VertexDesc;
	ZeroMemory(&VertexDesc, sizeof(VertexDesc));
	VertexDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexDesc.ByteWidth = Size;
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexDesc.CPUAccessFlags = 0;
	VertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&VertexDesc, &InitData, &Result))
	{
		return Result;
	}
	X_LOG("CreateVertexBuffer  failed!");
	return NULL;
}

ID3D11Buffer* CreateIndexBuffer(void* Data, unsigned int Size)
{
	D3D11_BUFFER_DESC IndexDesc;
	ZeroMemory(&IndexDesc, sizeof(IndexDesc));
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.ByteWidth = Size;
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0;
	IndexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));
	IndexData.pSysMem = Data;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&IndexDesc, &IndexData, &Result))
	{
		return Result;
	}
	X_LOG("CreateIndexBuffer failed!");
	return NULL;
}

ID3D11Buffer* CreateConstantBuffer(void* Data, unsigned int Size)
{
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = Size;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = Data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&Desc, &InitData, &Result))
	{
		return Result;
	}
	X_LOG("CreateConstantBuffer failed!");
	return NULL;
}

class ShaderIncludeHandler : public ID3DInclude
{
public:
	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		std::string IncludeFile;
		switch (IncludeType)
		{
		case D3D_INCLUDE_LOCAL:
			IncludeFile = std::string("./") + pFileName;
			break;
		case D3D_INCLUDE_SYSTEM:
			IncludeFile = std::string("./") + pFileName;
			break;
		}
		X_LOG("filename:%s\n",IncludeFile.c_str());
// 		FILE* file;
// 		fopen_s(&file,IncludeFile.c_str(), "r");
// 		if (file)
// 		{
// 			fseek(file, 0, SEEK_END);
// 			size_t filesize = ftell(file);
// 			X_LOG("filesize:%d\n", filesize);
// 			fseek(file, 0, SEEK_SET);
// 			filedata = std::make_unique<char[]>(filesize);
// 			fread(filedata.get(), 1, filesize, file);
// 			fclose(file);
// 			X_LOG("content:%s\n", filedata.get());
//  			*ppData = filedata.get();
//  			*pBytes = filesize;
// 			return S_OK;
// 		}
		std::ifstream includeFile(IncludeFile.c_str(), std::ios::in | std::ios::binary | std::ios::ate);


		if (includeFile.is_open()) {
			unsigned int fileSize = (unsigned int)includeFile.tellg();
			char* buf = new char[fileSize];
			includeFile.seekg(0, std::ios::beg);
			includeFile.read(buf, fileSize);
			includeFile.close();
			*ppData = buf;
			*pBytes = fileSize;
			return S_OK;
		}
		else {
			return E_FAIL;
		}
// 		CString D(pFileName);
// 		D3DReadFileToBlob(D.GetBuffer(), &blob);
// 		D.ReleaseBuffer();
// 
// 		(*ppData) = blob->GetBufferPointer();
// 		(*pBytes) = blob->GetBufferSize();
		return E_FAIL;
	}
	HRESULT __stdcall Close(LPCVOID pData)
	{
		//blob->Release();
		char* buf = (char*)pData;
		delete[] buf;
		return S_OK;
	}
private:
	std::unique_ptr<char[]> filedata;
	//ID3DBlob* blob;
};
ID3DBlob* CompileVertexShader(const wchar_t* File, const char* EntryPoint)
{
	ID3DBlob* Bytecode;
	ID3DBlob* OutErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG /*| D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY*/;
	ShaderIncludeHandler IncludeHandler;
	D3D_SHADER_MACRO Macros[] =
	{
		"PIXELSHADER",										"1",
		"COMPILER_HLSL",									"1",
		"MATERIAL_TANGENTSPACENORMAL",						"1",
		"LIGHTMAP_UV_ACCESS",								"0",
		"USE_INSTANCING",									"0",
		"TEX_COORD_SCALE_ANALYSIS",							"0",
		"TEX_COORD_SCALE_ANALYSIS",							"0",
		 NULL,NULL
	};
	if (S_OK == D3DCompileFromFile(File, Macros, &IncludeHandler, EntryPoint, VSTarget, VSFlags, 0, &Bytecode, &OutErrorMsg))
	{
		return Bytecode;
	}
	X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
	return NULL;
}

ID3DBlob* CompilePixelShader(const wchar_t* File, const char* EntryPoint) 
{
	ID3DBlob* Bytecode;
	ID3DBlob* OutErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG /*| D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY*/;
	ShaderIncludeHandler IncludeHandler;
	D3D_SHADER_MACRO Macros[] =
	{
		"PIXELSHADER",										"1",
		"COMPILER_HLSL",									"1",
		"MATERIAL_TANGENTSPACENORMAL",						"1",
		"LIGHTMAP_UV_ACCESS",								"0",
		"USE_INSTANCING",									"0",
		"TEX_COORD_SCALE_ANALYSIS",							"0",
		"TEX_COORD_SCALE_ANALYSIS",							"0",
		NULL,NULL
	};
	if (S_OK == D3DCompileFromFile(File, Macros, &IncludeHandler, EntryPoint, VSTarget, VSFlags, 0, &Bytecode, &OutErrorMsg))
	{
		return Bytecode;
	}
	X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
	return NULL;
}

ID3D11VertexShader* CreateVertexShader(ID3DBlob* VSBytecode)
{
	ID3D11VertexShader* Result;
	if (S_OK == D3D11Device->CreateVertexShader(VSBytecode->GetBufferPointer(), VSBytecode->GetBufferSize(), NULL, &Result))
	{
		return Result;
	}

	X_LOG("CreateVertexShader failed!");
	return NULL;
}

ID3D11PixelShader* CreatePixelShader(ID3DBlob* PSBytecode)
{
	ID3D11PixelShader* Result;
	if (S_OK == D3D11Device->CreatePixelShader(PSBytecode->GetBufferPointer(), PSBytecode->GetBufferSize(), NULL, &Result))
	{
		return Result;
	}

	X_LOG("CreateVertexShader failed!");
	return NULL;
}

ID3D11InputLayout* CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* InputDesc, unsigned int Count, ID3DBlob* VSBytecode)
{
	ID3D11InputLayout* Result;
	if (S_OK == D3D11Device->CreateInputLayout(InputDesc, Count, VSBytecode->GetBufferPointer(), VSBytecode->GetBufferSize(), &Result))
	{
		return Result;
	}
	X_LOG("CreateInputLayout failed!");
	return NULL;
}

ID3D11Texture2D* CreateTexture2D(unsigned int W, unsigned int H, DXGI_FORMAT Format)
{
	D3D11_TEXTURE2D_DESC Desc;
	Desc.Width = W;
	Desc.Height = H;
	Desc.Format = Format;
	Desc.ArraySize = 1;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = nullptr;
	Data.SysMemPitch = 0;
	Data.SysMemSlicePitch = 0;

	ID3D11Texture2D* Result;
	if (S_OK == D3D11Device->CreateTexture2D(&Desc,NULL, &Result))
	{
		return Result;
	}
	X_LOG("CreateTexture2D failed!");
	return NULL;
}

ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Texture2D* Resource, DXGI_FORMAT Format)
{
	D3D11_RENDER_TARGET_VIEW_DESC Desc;
	Desc.Format = Format;
	Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Desc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* Result;
	if (S_OK == D3D11Device->CreateRenderTargetView(Resource, &Desc, &Result))
	{
		return Result;
	}
	X_LOG("CreateRenderTargetView failed!");
	return NULL;
}
