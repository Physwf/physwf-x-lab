#include "TriangleDemo.h"

void TriangleDemo::InitPipelineStates()
{
	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;
	ComPtr<ID3DBlob> Error;

	HRESULT hr;
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
	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_D3D12PipelineState));

	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_D3D12PipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_D3D12CmdList));
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
		assert(S_OK == m_D3D12Device->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_D3D12VertexBuffer));

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
}

void TriangleDemo::Draw()
{
	m_D3D12CmdAllocator->Reset();
	m_D3D12CmdList->Reset(m_D3D12CmdAllocator.Get(), m_D3D12PipelineState.Get());

	m_D3D12CmdList->SetGraphicsRootSignature(m_D3D12RootSign.Get());
	m_D3D12CmdList->RSSetViewports(1, &m_Viewport);
	m_D3D12CmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
	m_D3D12CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_D3D12CmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_D3D12CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_D3D12CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_D3D12CmdList->DrawInstanced(3, 1, 0, 0);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

	m_D3D12CmdList->Close();
}

void TriangleDemoMSAA::InitPipelineStates()
{
	{
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		ComPtr<ID3DBlob> Error;

		HRESULT hr;
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
		GPSDesc.RasterizerState.MultisampleEnable = TRUE;
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
		GPSDesc.SampleDesc.Count = 16;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_D3D12PipelineState));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_D3D12PipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_D3D12CmdList));
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
			assert(S_OK == m_D3D12Device->CreateCommittedResource(
				&HeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&ResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				__uuidof(ID3D12Resource),
				&m_D3D12VertexBuffer));

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
	}

	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAALevels;
		MSAALevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		MSAALevels.SampleCount = 16;
		MSAALevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		assert(S_OK == m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAALevels, sizeof(MSAALevels)));

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
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ResourceDesc.SampleDesc.Count = 16;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
		ClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		/*assert(S_OK ==*/ m_D3D12Device->CreateCommittedResource(&HeapProperitys, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), &m_SceneColorMSAA)/*)*/;

		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		ZeroMemory(&Desc, 0);
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		Desc.Texture2D.MipSlice = 0;
		Desc.Texture2D.PlaneSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
		DescHandle.ptr += 2 * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(m_SceneColorMSAA.Get(),&Desc, DescHandle);
	}

}

void TriangleDemoMSAA::Draw()
{
	m_D3D12CmdAllocator->Reset();
	m_D3D12CmdList->Reset(m_D3D12CmdAllocator.Get(), m_D3D12PipelineState.Get());

	m_D3D12CmdList->SetGraphicsRootSignature(m_D3D12RootSign.Get());
	m_D3D12CmdList->RSSetViewports(1, &m_Viewport);
	m_D3D12CmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_D3D12DescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += 2 * m_RTVDescriptorSize;
	m_D3D12CmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	{


		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_D3D12CmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_D3D12CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_D3D12CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		m_D3D12CmdList->DrawInstanced(3, 1, 0, 0);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

		m_D3D12CmdList->ResolveSubresource(m_BackBuffer[m_FrameIndex].Get(), 0, m_SceneColorMSAA.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_D3D12CmdList->ResourceBarrier(1, &ResourceBarrier);
	}
	
	m_D3D12CmdList->Close();
}
