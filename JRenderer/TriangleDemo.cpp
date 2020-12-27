#include "TriangleDemo.h"

void TriangleDemo::InitPipelineStates()
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = 1920;
	m_Viewport.Height = 1080;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = m_ScissorRect.top = 0;
	m_ScissorRect.right = 1920;
	m_ScissorRect.bottom = 1080;

	m_RTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_SampleCount = 1;
	m_SampleQuality = 0;

	InitTriangleResources();
}

void TriangleDemo::Draw()
{
	m_D3D12CmdAllocator->Reset();
	m_TriangleCmdList->Reset(m_D3D12CmdAllocator.Get(), m_TrianglePipelineState.Get());

	m_TriangleCmdList->SetGraphicsRootSignature(m_TriangleRootSign.Get());
	m_TriangleCmdList->RSSetViewports(1, &m_Viewport);
	m_TriangleCmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
	m_TriangleCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_TriangleCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_TriangleCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_TriangleCmdList->IASetVertexBuffers(0, 1, &m_TriangleVertexBufferView);
	m_TriangleCmdList->DrawInstanced(3, 1, 0, 0);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);

	m_TriangleCmdList->Close();
}

void TriangleDemo::InitTriangleResources()
{
	ComPtr<ID3DBlob> Error;

	{
		D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
		ZeroMemory(&rootSignDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignDesc.NumParameters = 0;
		rootSignDesc.pParameters = nullptr;
		rootSignDesc.NumStaticSamplers = 0;
		rootSignDesc.pStaticSamplers = 0;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		assert(S_OK == D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_TriangleRootSign));
	}
	
	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;

	HRESULT hr;
	hr = D3DCompileFromFile(TEXT("triangle.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
	if (FAILED(hr))
	{
		LOGA("%s", Error->GetBufferPointer());
		return;
	}
	hr = D3DCompileFromFile(TEXT("triangle.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PS, &Error);
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
	GPSDesc.pRootSignature = m_TriangleRootSign.Get();
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
	GPSDesc.RTVFormats[0] = m_RTFormat;
	GPSDesc.SampleDesc.Count = m_SampleCount;
	GPSDesc.SampleDesc.Quality = m_SampleQuality;
	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_TrianglePipelineState));

	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_TrianglePipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_TriangleCmdList));
	m_TriangleCmdList->Close();

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
		assert(S_OK == m_D3D12Device->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_TriangleVertexBuffer));

		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_TriangleVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_TriangleVertexBuffer->Unmap(0, nullptr);

		m_TriangleVertexBufferView.BufferLocation = m_TriangleVertexBuffer->GetGPUVirtualAddress();
		m_TriangleVertexBufferView.StrideInBytes = sizeof(float) * 7;
		m_TriangleVertexBufferView.SizeInBytes = vertexBufferSize;
	}
}

void TriangleDemoMSAA::InitPipelineStates()
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = 1920;
	m_Viewport.Height = 1080;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_ScissorRect.left = m_ScissorRect.top = 0;
	m_ScissorRect.right = 1920;
	m_ScissorRect.bottom = 1080;

	m_RTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	//MSAA支持查询
	m_MSAALevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_MSAALevels.SampleCount = 32;
	m_MSAALevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	while (m_MSAALevels.SampleCount > 0)
	{
		assert(S_OK == m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &m_MSAALevels, sizeof(m_MSAALevels)));
		if (m_MSAALevels.NumQualityLevels > 0) break;//如果SampleCount被支持,则NumQualityLevels>0
		m_MSAALevels.SampleCount >>= 1;
	}

	m_SampleCount = m_MSAALevels.SampleCount;
	m_SampleQuality = m_MSAALevels.NumQualityLevels - 1;

	InitTriangleResources();
	
	InitSceneColorRT();
}

void TriangleDemoMSAA::Draw()
{
	m_CommandLists.clear();

	m_D3D12CmdAllocator->Reset();
	m_TriangleCmdList->Reset(m_D3D12CmdAllocator.Get(), m_TrianglePipelineState.Get());

	m_TriangleCmdList->SetGraphicsRootSignature(m_TriangleRootSign.Get());
	m_TriangleCmdList->RSSetViewports(1, &m_Viewport);
	m_TriangleCmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += 2 * m_RTVDescriptorSize;
	m_TriangleCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_TriangleCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_TriangleCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_TriangleCmdList->IASetVertexBuffers(0, 1, &m_TriangleVertexBufferView);
		m_TriangleCmdList->DrawInstanced(3, 1, 0, 0);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);

		m_TriangleCmdList->ResolveSubresource(m_BackBuffer[m_FrameIndex].Get(), 0, m_SceneColorRTMSAA.Get(), 0, m_RTFormat);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_TriangleCmdList->ResourceBarrier(1, &ResourceBarrier);
	}
	
	m_TriangleCmdList->Close();

	m_CommandLists.push_back(m_TriangleCmdList.Get());
}

void TriangleDemoMSAA::InitSceneColorRT()
{
	{
		D3D12_HEAP_PROPERTIES HeapProperitys;
		ZeroMemory(&HeapProperitys, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperitys.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProperitys.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperitys.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperitys.CreationNodeMask = 1;
		HeapProperitys.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = 1920;
		ResourceDesc.Height = 1080;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = m_RTFormat;
		ResourceDesc.SampleDesc.Count = m_SampleCount;
		ResourceDesc.SampleDesc.Quality = m_SampleQuality;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
		ClearValue.Format = m_RTFormat;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperitys, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), &m_SceneColorRTMSAA));

		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		ZeroMemory(&Desc, 0);
		Desc.Format = m_RTFormat;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		Desc.Texture2D.MipSlice = 0;
		Desc.Texture2D.PlaneSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		DescHandle.ptr += 2 * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(m_SceneColorRTMSAA.Get(), &Desc, DescHandle);
	}
}

void TriangleDemoMSAA2::InitPipelineStates()
{
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = 1920;
	m_Viewport.Height = 1080;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_ScissorRect.left = m_ScissorRect.top = 0;
	m_ScissorRect.right = 1920;
	m_ScissorRect.bottom = 1080;

	m_RTFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;

	//MSAA支持查询
	m_MSAALevels.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_MSAALevels.SampleCount = 32;
	m_MSAALevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	while (m_MSAALevels.SampleCount > 0)
	{
		assert(S_OK == m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &m_MSAALevels, sizeof(m_MSAALevels)));
		if (m_MSAALevels.NumQualityLevels > 0) break;//如果SampleCount被支持,则NumQualityLevels>0
		m_MSAALevels.SampleCount >>= 1;
	}

	m_SampleCount = m_MSAALevels.SampleCount;
	m_SampleQuality = m_MSAALevels.NumQualityLevels - 1;

	InitTriangleResources();

	InitSceneColorRT();

	InitQuadResources();
}

void TriangleDemoMSAA2::Draw()
{
	m_CommandLists.clear();

	m_D3D12CmdAllocator->Reset();
	m_TriangleCmdList->Reset(m_D3D12CmdAllocator.Get(), m_TrianglePipelineState.Get());

	m_TriangleCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_TriangleCmdList->IASetVertexBuffers(0, 1, &m_TriangleVertexBufferView);
	m_TriangleCmdList->SetGraphicsRootSignature(m_TriangleRootSign.Get());
	m_TriangleCmdList->RSSetViewports(1, &m_Viewport);
	m_TriangleCmdList->RSSetScissorRects(1, &m_ScissorRect);

	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += 2 * m_RTVDescriptorSize;
		m_TriangleCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_TriangleCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_TriangleCmdList->DrawInstanced(3, 1, 0, 0);
	}
	

	m_TriangleCmdList->Close();
	m_CommandLists.push_back(m_TriangleCmdList.Get());

	m_QuadCmdList->Reset(m_D3D12CmdAllocator.Get(), m_QuadPipelineState.Get());
	m_QuadCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_QuadCmdList->IASetVertexBuffers(0, 1, &m_QuadVertexBufferView);
	m_QuadCmdList->SetGraphicsRootSignature(m_QuadRootSign.Get());
	D3D12_GPU_DESCRIPTOR_HANDLE TriangleSRHandle =  m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart();
	m_QuadCmdList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	m_QuadCmdList->SetGraphicsRootDescriptorTable(1, TriangleSRHandle);
	m_QuadCmdList->SetGraphicsRoot32BitConstant(0, m_SampleCount, 0);
	//m_QuadCmdList->SetDescriptorHeaps(1, m_SamplerHeap.GetAddressOf());
	m_QuadCmdList->RSSetViewports(1, &m_Viewport);
	m_QuadCmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_QuadCmdList->ResourceBarrier(1, &ResourceBarrier);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_QuadCmdList->ResourceBarrier(1, &ResourceBarrier);

	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
		m_QuadCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_QuadCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_QuadCmdList->DrawInstanced(5,1,0,0);
	}


	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_QuadCmdList->ResourceBarrier(1, &ResourceBarrier);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_QuadCmdList->ResourceBarrier(1, &ResourceBarrier);

	m_QuadCmdList->Close();
	m_CommandLists.push_back(m_QuadCmdList.Get());
}

void TriangleDemoMSAA2::InitQuadResources()
{
	ComPtr<ID3DBlob> Error;

	{
		D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
		ZeroMemory(&rootSignDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignDesc.NumParameters = 2;
		D3D12_ROOT_PARAMETER RootParams[2];
		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		RootParams[0].Constants.ShaderRegister = 0;
		RootParams[0].Constants.RegisterSpace = 0;
		RootParams[0].Constants.Num32BitValues = 1;
		RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_DESCRIPTOR_RANGE DescRange;
		DescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		DescRange.NumDescriptors = 1;
		DescRange.BaseShaderRegister = 0;
		DescRange.RegisterSpace = 0;
		DescRange.OffsetInDescriptorsFromTableStart = 0;
		RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		RootParams[1].DescriptorTable.pDescriptorRanges = &DescRange;
		RootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootSignDesc.pParameters = RootParams;
		rootSignDesc.NumStaticSamplers = 0;
		rootSignDesc.pStaticSamplers = 0;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		//rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		ComPtr<ID3DBlob> Signature;
		assert(S_OK == D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_QuadRootSign));
	}

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;

	HRESULT hr;
	hr = D3DCompileFromFile(TEXT("quad.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
	if (FAILED(hr))
	{
		LOGA("%s", Error->GetBufferPointer());
		return;
	}
	hr = D3DCompileFromFile(TEXT("quad.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PS, &Error);
	if (FAILED(hr))
	{
		LOGA("%s", Error->GetBufferPointer());
		return;
	}
	D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
	ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	GPSDesc.pRootSignature = m_QuadRootSign.Get();
	GPSDesc.VS = { reinterpret_cast<UINT8*>(VS->GetBufferPointer()), VS->GetBufferSize() };
	GPSDesc.PS = { reinterpret_cast<UINT8*>(PS->GetBufferPointer()), PS->GetBufferSize() };
	GPSDesc.InputLayout = { InputElementDesc, _countof(InputElementDesc) };
	GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
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
	GPSDesc.RTVFormats[0] = m_BackBufferFormat;
	GPSDesc.SampleDesc.Count = 1;
	GPSDesc.SampleDesc.Quality = 0;
	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_QuadPipelineState));

	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_QuadPipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_QuadCmdList));
	m_QuadCmdList->Close();

	{
		float QuadVertices[] =
		{
			-1.0f,	-1.0f,		0.0f, 1.0f,	
			1.0f,  -1.0f,		1.0f, 1.0f,
			1.0f,  1.0f,		1.0f, 0.0f,
			-1.0f, 1.0f,		0.0f, 0.0f,
			-1.0f,	-1.0f,		0.0f, 1.0f,
		};

		const UINT vertexBufferSize = sizeof(QuadVertices);

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
		assert(S_OK == m_D3D12Device->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_QuadVertexBuffer));

		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_QuadVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, QuadVertices, sizeof(QuadVertices));
		m_QuadVertexBuffer->Unmap(0, nullptr);

		m_QuadVertexBufferView.BufferLocation = m_QuadVertexBuffer->GetGPUVirtualAddress();
		m_QuadVertexBufferView.StrideInBytes = sizeof(float) * 4;
		m_QuadVertexBufferView.SizeInBytes = vertexBufferSize;
	}
	/*
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(m_SampleCount);
		ResourceDesc.Height = 1;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		ResourceDesc.Alignment = 0;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_R32_UINT;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_QuadConstants));

		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_QuadConstants->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, &m_SampleCount, sizeof(m_SampleCount));
		m_QuadVertexBuffer->Unmap(0, nullptr);
	}
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.BufferLocation = m_QuadConstants->GetGPUVirtualAddress();
		Desc.SizeInBytes = 4;
		D3D12_CPU_DESCRIPTOR_HANDLE TriangleSRHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateConstantBufferView(&Desc, TriangleSRHandle);
	}
	*/
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		Desc.Texture2D.MipLevels = 1;
		Desc.Texture2D.MostDetailedMip = 0;
		Desc.Texture2D.PlaneSlice = 0;
		Desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0,1,2,3);
		D3D12_CPU_DESCRIPTOR_HANDLE TriangleSRHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		//TriangleSRHandle.ptr += m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(m_SceneColorRTMSAA.Get(), &Desc, TriangleSRHandle);
	}

// 	{
// 		D3D12_SAMPLER_DESC Desc;
// 		ZeroMemory(&Desc, sizeof(Desc));
// 		Desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
// 		Desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.MipLODBias = 0.0f;
// 		D3D12_CPU_DESCRIPTOR_HANDLE TriangleSamplerHandle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
// 		m_D3D12Device->CreateSampler(&Desc, TriangleSamplerHandle);
// 	}


}
