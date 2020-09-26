#include "D3D11API.h"

IDXGIFactory*	DXGIFactory = NULL;
IDXGIAdapter*	DXGIAdapter = NULL;
IDXGIOutput*	DXGIOutput = NULL;
IDXGISwapChain* DXGISwapChain = NULL;

ID3D11Device*			D3D11Device = NULL;
ID3D11DeviceContext*	D3D11DeviceContext = NULL;

bool InitD3D11API()
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
		D3D11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &NumQualityLevels);

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
		SwapChainDesc.SampleDesc.Quality = NumQualityLevels - 1;
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
}

void RelaseD3D11API()
{
}


ID3D11Buffer* CreateVertexBuffer(uint32 Size,uint32 InUseage, APIResourceCreateInfo& CreateInfo)
{
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = Size;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;

	Desc.BindFlags = 0;
	Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = CreateInfo.Data;
	Data.SysMemPitch = Size;
	Data.SysMemSlicePitch = 0;

	ID3D11Buffer* Buffer = nullptr;
	if (S_OK == D3D11Device->CreateBuffer(&Desc, &Data, &Buffer))
	{
		return Buffer;
	}
	return nullptr;
}

ID3D11Buffer* CreateIndexBuffer(uint32 Size, uint32 InUseage, APIResourceCreateInfo& CreateInfo)
{
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = Size;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags = 0;

	Desc.BindFlags = 0;
	Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = CreateInfo.Data;
	Data.SysMemPitch = Size;
	Data.SysMemSlicePitch = 0;

	ID3D11Buffer* Buffer = nullptr;
	if (S_OK == D3D11Device->CreateBuffer(&Desc, &Data, &Buffer))
	{
		return Buffer;
	}
	return nullptr;
}

VertexDeclaration* CreateVertexDelcaration(const VertexElementList_t& Elements)
{
	InputDescList_t Desc;
	for (size_t i = 0; i < Elements.size(); ++i)
	{
		const VertexElement& Element = Elements[i];
		D3D11_INPUT_ELEMENT_DESC D3DElement = {0};
		D3DElement.InputSlot = Element.StreamIndex;
		D3DElement.AlignedByteOffset = Element.Offset;
		D3DElement.Format = Element.Type;
		D3DElement.SemanticName = "ATTRIBUTE";
		D3DElement.SemanticIndex = Element.AttributeIndex;
		D3DElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		D3DElement.InstanceDataStepRate = 0;

		Desc.push_back(D3DElement);
	}

	struct FCompareDesc
	{
		bool operator()(const D3D11_INPUT_ELEMENT_DESC& A, const D3D11_INPUT_ELEMENT_DESC &B) const
		{
			return ((int32)A.AlignedByteOffset + A.InputSlot * MAX_uint16) < ((int32)B.AlignedByteOffset + B.InputSlot * MAX_uint16);
		}
	};

	std::sort(Desc.begin(), Desc.end(), FCompareDesc());

	return new VertexDeclaration(Desc);
}


ID3D11VertexShader* CreateVertexBuffer(const std::vector<uint8>& Code)
{
	ID3D11VertexShader* Result;
	if (S_OK == D3D11Device->CreateVertexShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

ID3D11PixelShader* CreatePixelShader(const std::vector<uint8>& Code)
{
	ID3D11PixelShader* Result;
	if (S_OK == D3D11Device->CreatePixelShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

ID3D11HullShader* CreateHullShader(const std::vector<uint8>& Code)
{
	ID3D11HullShader* Result;
	if (S_OK == D3D11Device->CreateHullShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

ID3D11DomainShader* CreateDomainShader(const std::vector<uint8>& Code)
{
	ID3D11DomainShader* Result;
	if (S_OK == D3D11Device->CreateDomainShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

ID3D11GeometryShader* CreateGeometryShader(const std::vector<uint8>& Code)
{
	ID3D11GeometryShader* Result;
	if (S_OK == D3D11Device->CreateGeometryShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

ID3D11ComputeShader* CreateComputeShader(const std::vector<uint8>& Code)
{
	ID3D11ComputeShader* Result;
	if (S_OK == D3D11Device->CreateComputeShader(Code.data(), Code.size(), NULL, &Result))
	{
		return Result;
	}
	return NULL;
}

void SetShaderUniformBuffer(ID3D11VertexShader* VS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->VSSetConstantBuffers(BufferIndex, 1, &BufferRHI);
}

void SetShaderUniformBuffer(ID3D11HullShader* HS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->HSGetConstantBuffers(BufferIndex, 1, &BufferRHI);
}

void SetShaderUniformBuffer(ID3D11DomainShader* DS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->DSSetConstantBuffers(BufferIndex, 1, &BufferRHI);
}

void SetShaderUniformBuffer(ID3D11GeometryShader* GS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->GSSetConstantBuffers(BufferIndex, 1, BufferRHI);
}

void SetShaderUniformBuffer(ID3D11PixelShader* PS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->PSSetConstantBuffers(BufferIndex, 1, BufferRHI);
}

void SetShaderUniformBuffer(ID3D11ComputeShader* CS, uint32 BufferIndex, ID3D11Buffer* BufferRHI)
{
	D3D11DeviceContext->CSSetConstantBuffers(BufferIndex, 1, &BufferRHI);
}
