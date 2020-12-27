#include "d3d12demo.h"


void D3D12Demo::Initialize()
{
	if (EnumAdapter())
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}

		HRESULT hr;
		assert(S_OK == D3D12CreateDevice(m_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), &m_D3D12Device));

		//MSAA支持查询
		m_MSAALevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_MSAALevels.SampleCount = 32;
		m_MSAALevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		while (m_MSAALevels.SampleCount > 0)
		{
			assert(S_OK == m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &m_MSAALevels, sizeof(m_MSAALevels)));
			if(m_MSAALevels.NumQualityLevels >0) break;//如果SampleCount被支持,则NumQualityLevels>0
			m_MSAALevels.SampleCount >>= 1;
		}

		D3D12_COMMAND_QUEUE_DESC CmdQueueDesc;
		ZeroMemory(&CmdQueueDesc, sizeof(CmdQueueDesc));
		CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		assert(S_OK == m_D3D12Device->CreateCommandQueue(&CmdQueueDesc, __uuidof(ID3D12CommandQueue), &m_D3D12CmdQueue));

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferDesc.Width = 1920;
		SwapChainDesc.BufferDesc.Height = 1080;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = FrameCount;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.Flags = 0;
		ComPtr<IDXGISwapChain> SwapChain;
		assert(S_OK == m_DXGIFactory->CreateSwapChain(m_D3D12CmdQueue.Get(), &SwapChainDesc, &SwapChain));
		SwapChain.As(&m_DXGISwapChain);

		m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = 1920;
		m_Viewport.Height = 1080;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect.left = m_ScissorRect.top = 0;
		m_ScissorRect.right = 1920;
		m_ScissorRect.bottom = 1080;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = FrameCount+1;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_D3D12DescHeap));
		m_RTVDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT n = 0; n < FrameCount; ++n)
		{
			assert(S_OK == m_DXGISwapChain->GetBuffer(n, __uuidof(ID3D12Resource), &m_BackBuffer[n]));
			m_D3D12Device->CreateRenderTargetView(m_BackBuffer[n].Get(), nullptr, RTVHandle);
			//offset
			RTVHandle.ptr += m_RTVDescriptorSize;
		}
		hr = m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), &m_D3D12CmdAllocator);

		D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
		ZeroMemory(&rootSignDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignDesc.NumParameters = 0;
		rootSignDesc.pParameters = nullptr;
		rootSignDesc.NumStaticSamplers = 0;
		rootSignDesc.pStaticSamplers = 0;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_D3D12RootSign));

		InitPipelineStates();

		{
			m_D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &m_Fence);
			m_FenceValue = 1;

			m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_FenceEvent == nullptr)
			{
				
			}

			WaitForPreviousFrame();
		}
	}
}

void D3D12Demo::Render()
{
	Draw();

	ID3D12CommandList* ppCommandLists[] = { m_D3D12CmdList.Get() };
	m_D3D12CmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	m_DXGISwapChain->Present(1, 0);

	WaitForPreviousFrame();
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

void D3D12Demo::WaitForPreviousFrame()
{
	const UINT64 fence = m_FenceValue;
	m_D3D12CmdQueue->Signal(m_Fence.Get(), fence);
	m_FenceValue++;

	if (m_Fence->GetCompletedValue() < fence)
	{
		m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();
}
