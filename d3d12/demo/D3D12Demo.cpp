#include "d3d12demo.h"
#include <d3dcompiler.h>


#define LOG(format,...)									\
	do {												\
		wchar_t info[1024] = {0};						\
		wsprintf(info, format, __VA_ARGS__);			\
		OutputDebugString(info);						\
	} while (0);

#define LOGA(format,...)									\
	do {													\
		char info[1024] = {0};								\
		wsprintfA(info, format, __VA_ARGS__);					\
		OutputDebugStringA(info);							\
	} while (0);

void D3D12Demo::Initialize()
{
	if (EnumAdapter())
	{
		HRESULT hr = D3D12CreateDevice(m_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), &m_D3D12Device);
		if (FAILED(hr))
		{
			return;
		}

		D3D12_COMMAND_QUEUE_DESC CmdQueueDesc;
		ZeroMemory(&CmdQueueDesc, sizeof(CmdQueueDesc));
		CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		hr = m_D3D12Device->CreateCommandQueue(&CmdQueueDesc, __uuidof(ID3D12CommandQueue), &m_D3D12CmdQueue);
		if (FAILED(hr))
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferDesc.Width = 1000;
		SwapChainDesc.BufferDesc.Height = 800;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = FrameCount;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.Flags = 0;
		ComPtr<IDXGISwapChain> SwapChain;
		hr = m_DXGIFactory->CreateSwapChain(m_D3D12CmdQueue.Get(), &SwapChainDesc, &SwapChain);
		if (FAILED(hr))
		{
			return;
		}
		SwapChain.As(&m_DXGISwapChain);

		m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = 1000;
		m_Viewport.Height = 800;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect.left = m_ScissorRect.top = 0;
		m_ScissorRect.bottom = 800;
		m_ScissorRect.right = 1000;

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
			hr = m_DXGISwapChain->GetBuffer(n, __uuidof(ID3D12Resource), &m_D3D12RenderTargets[n]);
			if (FAILED(hr))
			{
				return;
			}
			m_D3D12Device->CreateRenderTargetView(m_D3D12RenderTargets[n].Get(), nullptr, RTVHandle);
			//offset
			RTVHandle.ptr += 1 * m_RTVDescriptorSize;
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
		hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error);
		if (FAILED(hr))
		{
			return;
		}
		hr = m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_D3D12RootSign);
		if (FAILED(hr))
		{
			return;
		}

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		hr = D3DCompileFromFile(TEXT("shader.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &VS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		hr = D3DCompileFromFile(TEXT("shader.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &PS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		GPSDesc.pRootSignature = m_D3D12RootSign.Get();
		GPSDesc.VS = { reinterpret_cast<UINT8*>(VS->GetBufferPointer()), VS->GetBufferSize() };
		GPSDesc.PS = { reinterpret_cast<UINT8*>(PS->GetBufferPointer()), PS->GetBufferSize() };
		GPSDesc.InputLayout = { InputElementDesc, _countof(InputElementDesc) };
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FrontCounterClockwise = FALSE;
		GPSDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		GPSDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		GPSDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		GPSDesc.RasterizerState.DepthClipEnable = TRUE;
		GPSDesc.RasterizerState.MultisampleEnable = FALSE;
		GPSDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		GPSDesc.RasterizerState.ForcedSampleCount = 0;
		GPSDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		GPSDesc.BlendState.AlphaToCoverageEnable = FALSE;
		GPSDesc.BlendState.IndependentBlendEnable = FALSE;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		GPSDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		GPSDesc.DepthStencilState.DepthEnable = FALSE;
		GPSDesc.DepthStencilState.StencilEnable = FALSE;
		//GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		//GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GPSDesc.SampleMask = UINT_MAX;
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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
			float triangleVertices[] =
			{
				0.0f,	0.25f,	0.0f,	1.0f, 0.0f, 0.0f, 1.0f,
				0.25f,	-0.25f,	0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
				-0.25f, -0.25f,	0.0f,	0.0f, 0.0f, 1.0f, 1.0f
			};

			const UINT vertexBufferSize = sizeof(triangleVertices);

			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			HeapProperties.CreationNodeMask = 1;
			HeapProperties.VisibleNodeMask = 1;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = vertexBufferSize;
			ResourceDesc.Height = 1;
			ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			ResourceDesc.Alignment = 0;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			hr = m_D3D12Device->CreateCommittedResource(
				&HeapProperties, 
				D3D12_HEAP_FLAG_NONE, 
				&ResourceDesc, 
				D3D12_RESOURCE_STATE_GENERIC_READ, 
				nullptr, 
				__uuidof(ID3D12Resource), 
				&m_D3D12VertexBuffer);
			if (FAILED(hr))
			{
				return;
			}

			UINT8* pVertexDataBegin;
			D3D12_RANGE readRange;
			readRange.Begin = readRange.End = 0;
			m_D3D12VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
			memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
			m_D3D12VertexBuffer->Unmap(0, nullptr);

			m_VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
			m_VertexBufferView.StrideInBytes = sizeof(float) * 7;
			m_VertexBufferView.SizeInBytes = vertexBufferSize;
		}

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


void D3D12Demo::Draw()
{
	m_D3D12CmdAllocator->Reset();
	m_D3D12CmdList->Reset(m_D3D12CmdAllocator.Get(),m_D3D12PipelineState.Get());

	m_D3D12CmdList->SetGraphicsRootSignature(m_D3D12RootSign.Get());
	m_D3D12CmdList->RSSetViewports(1, &m_Viewport);
	m_D3D12CmdList->RSSetScissorRects(1, &m_ScissorRect);
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_D3D12RenderTargets[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
	m_D3D12CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_D3D12CmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_D3D12CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_D3D12CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_D3D12CmdList->DrawInstanced(3, 1, 0, 0);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_D3D12RenderTargets[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

	m_D3D12CmdList->Close();
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
