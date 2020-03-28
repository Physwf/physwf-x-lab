#include "d3d12demo.h"
#include <d3dcompiler.h>


#define LOG(format,...)									\
	do {												\
		wchar_t info[1024] = {0};						\
		wsprintf(info, format, __VA_ARGS__);			\
		OutputDebugString(info);						\
	} while (0);

void D3D12Demo::Initialize()
{
	if (EnumAdapter())
	{
		HRESULT hr = D3D12CreateDevice(m_DXGIAdapter.Get(),D3D_FEATURE_LEVEL_12_0,__uuidof(ID3D12Device), &m_D3D12Device);
		if (FAILED(hr))
		{
			return;
		}

		D3D12_COMMAND_QUEUE_DESC CmdQueueDesc;
		ZeroMemory(&CmdQueueDesc, sizeof(CmdQueueDesc));
		CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		hr = m_D3D12Device->CreateCommandQueue(&CmdQueueDesc,__uuidof(ID3D12CommandQueue),&m_D3D12CmdQueue);
		if (FAILED(hr))
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferDesc.Width = 1000;
		SwapChainDesc.BufferDesc.Height = 800;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = 0;
		ComPtr<IDXGISwapChain> SwapChain;
		hr = m_DXGIFactory->CreateSwapChain(m_D3D12CmdQueue.Get(), &SwapChainDesc,&SwapChain);
		SwapChain.As(&m_DXGISwapChain);

		m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_D3D12DescHeap);
		if (FAILED(hr))
		{
			return;
		}
		m_RTVDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT n = 0; n < FrameCount; ++n)
		{
			hr = m_DXGISwapChain->GetBuffer(n, __uuidof(ID3D12DescriptorHeap), &m_D3D12RenderTargets[n]);
			if (FAILED(hr))
			{
				return;
			}
			m_D3D12Device->CreateRenderTargetView(m_D3D12RenderTargets[n].Get(), nullptr, RTVHandle);
			//offset
		}
		hr = m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), &m_D3D12CmdAllocator);

		D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
		ZeroMemory(&rootSignDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		ComPtr<ID3DBlob> Signature;
		ComPtr<ID3DBlob> Error;
		hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error);
		if (FAILED(hr))
		{
			return;
		}
		hr = m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature) ,&m_D3D12RootSign);
		if (FAILED(hr))
		{
			return;
		}

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		hr = D3DCompileFromFile(TEXT("shaders.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &VS, nullptr);
		if (FAILED(hr))
		{
			return;
		}
		hr = D3DCompileFromFile(TEXT("shaders.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &PS, nullptr);
		if (FAILED(hr))
		{
			return;
		}
		D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		GPSDesc.InputLayout.pInputElementDescs = InputElementDesc;
		GPSDesc.InputLayout.NumElements = _countof(InputElementDesc);
		GPSDesc.pRootSignature = m_D3D12RootSign.Get();
		GPSDesc.VS.pShaderBytecode = reinterpret_cast<UINT8*>(VS->GetBufferPointer(), VS->GetBufferSize());
		GPSDesc.PS.pShaderBytecode = reinterpret_cast<UINT8*>(PS->GetBufferPointer(), PS->GetBufferSize());
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FrontCounterClockwise = 0;
		GPSDesc.RasterizerState.DepthBias = 0;
		GPSDesc.RasterizerState.DepthBiasClamp = 0.0f;
		GPSDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
		GPSDesc.RasterizerState.DepthClipEnable = true;
		GPSDesc.RasterizerState.MultisampleEnable = true;
		GPSDesc.RasterizerState.AntialiasedLineEnable = true;
		GPSDesc.RasterizerState.ForcedSampleCount = 0;
		GPSDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
		GPSDesc.BlendState.AlphaToCoverageEnable = false;
		GPSDesc.BlendState.IndependentBlendEnable = false;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = true;
		GPSDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		//GPSDesc.BlendState.RenderTarget[0].LogicOp = 
		//GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		GPSDesc.DepthStencilState.DepthEnable = true;
		GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GPSDesc.DepthStencilState.StencilEnable = false;
		GPSDesc.SampleMask = UINT_MAX;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		GPSDesc.SampleDesc.Count = 1;
		hr = m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_D3D12PipelineState);
		if (FAILED(hr))
		{
			return;
		}

		hr = m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_D3D12PipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_D3D12CmdList);
		if (FAILED(hr))
		{
			return;
		}
		m_D3D12CmdList->Close();

		{
			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
			D3D12_RESOURCE_DESC ResourceDesc;

			m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), &m_D3D12VertexBuffer);
		}
	}
}

bool D3D12Demo::EnumAdapter()
{
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory4), &m_DXGIFactory);
	if (FAILED(hr))
	{
		return false;
	}
	UINT iIndex = 0;
	while (true)
	{
		ComPtr<IDXGIAdapter> Adpater;
		hr = m_DXGIFactory->EnumAdapters(iIndex, Adpater.GetAddressOf());
		if (hr == DXGI_ERROR_NOT_FOUND) break;

		if (iIndex == 0)
		{
			m_DXGIAdapter = Adpater;
		}
		DXGI_ADAPTER_DESC Desc;
		Adpater->GetDesc(&Desc);
		LOG(TEXT("Adpater %d:\n  \tDescription:%s\n  \tVendorId:%u\n  \tDeviceId:%u\n  \tSubSysId:%u\n  \tRevision:%u\n  \tDedicatedVideoMemory:%u\n  \tDedicatedSystemMemory:%u\n \tSharedSystemMemory:%u\n"), 
			iIndex,
			Desc.Description, 
			Desc.VendorId,
			Desc.DeviceId,
			Desc.SubSysId,
			Desc.Revision,
			Desc.DedicatedVideoMemory, 
			Desc.DedicatedSystemMemory, 
			Desc.SharedSystemMemory);
		++iIndex;
	}
	return true;
}
