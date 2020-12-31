#include "ShadingModels.h"

void ShadingModelDemo::InitPipelineStates()
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
	m_MSAALevels.Format = m_RTFormat;
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

	InitSceneColorRT();
	InitSceneDepthRT();
}

void ShadingModelDemo::InitSceneColorRT()
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
		//Desc.Texture2DMS.MipSlice = 0;
		//Desc.Texture2DMS.PlaneSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		DescHandle.ptr += 2 * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(m_SceneColorRTMSAA.Get(), &Desc, DescHandle);
	}
}

void ShadingModelDemo::InitSceneDepthRT()
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
		ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ResourceDesc.SampleDesc.Count = m_SampleCount;
		ResourceDesc.SampleDesc.Quality = m_SampleQuality;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
		ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ClearValue.DepthStencil.Depth = 1.f;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperitys, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), &m_SceneDepthRTMSAA));

		D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
		ZeroMemory(&Desc, 0);
		Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		Desc.Flags = D3D12_DSV_FLAG_NONE;// D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		//Desc.Texture2DMS.MipSlice = 0;
		//Desc.Texture2DMS.PlaneSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateDepthStencilView(m_SceneDepthRTMSAA.Get(), &Desc, DescHandle);
	}
}

void PhongShadingModel::OnMouseMove(float fScreenX, float fScreenY)
{
	UpdatePointLight(fScreenX, fScreenY);
}

void PhongShadingModel::OnKeyDown(unsigned char KeyCode)
{
	float ShadingModel = 0;
	switch (KeyCode)
	{
	case 49://0
		ShadingModel = 0;
		break;
	case 50://1
		ShadingModel = 1;
		break;
	}

	MaterialUniform Material;
	Material.ka = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	Material.kd = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	Material.ks = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	Material.alpha = 5.f;
	Material.ShadingModel = ShadingModel;

	UINT8* pDataBegin;
	D3D12_RANGE readRange;
	readRange.Begin = readRange.End = 0;
	m_MaterialUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
	memcpy(pDataBegin, &Material, sizeof(Material));
	m_MaterialUniformBuffer->Unmap(0, nullptr);
}

void PhongShadingModel::InitPipelineStates()
{
	ShadingModelDemo::InitPipelineStates();

	InitMeshResourcces();
}

void PhongShadingModel::Draw()
{
	m_CommandLists.clear();

	m_D3D12CmdAllocator->Reset();
	m_PrimitiveCmdList->Reset(m_D3D12CmdAllocator.Get(), m_PrimitivePipelineState.Get());

	m_PrimitiveCmdList->IASetIndexBuffer(&m_PrimitiveIndexBufferView);
	m_PrimitiveCmdList->IASetVertexBuffers(0, 1, &m_PrimitiveVertexBufferView);
	m_PrimitiveCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_PrimitiveCmdList->SetGraphicsRootSignature(m_PrimitiveRootSign.Get());
	//m_PrimitiveCmdList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(0, m_ViewUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(1, m_PrimitiveUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(2, m_AmbientLightUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(3, m_PointLightUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(4, m_MaterialUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->SetGraphicsRootConstantBufferView(5, m_ViewUniformBuffer->GetGPUVirtualAddress());
	m_PrimitiveCmdList->RSSetViewports(1, &m_Viewport);
	m_PrimitiveCmdList->RSSetScissorRects(1, &m_ScissorRect);

	{
		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		RTVHandle.ptr += 2 * m_RTVDescriptorSize;
		D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_PrimitiveCmdList->OMSetRenderTargets(1, &RTVHandle,TRUE, &DSVHandle);
		float ClearColor[] = { 0,0,0,0 };
		m_PrimitiveCmdList->ClearRenderTargetView(RTVHandle, ClearColor, 0, NULL);
		m_PrimitiveCmdList->ClearDepthStencilView(DSVHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, NULL);
		m_PrimitiveCmdList->DrawIndexedInstanced(m.Indices.size(), 1, 0, 0, 0);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		m_PrimitiveCmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		m_PrimitiveCmdList->ResourceBarrier(1, &ResourceBarrier);

		m_PrimitiveCmdList->ResolveSubresource(m_BackBuffer[m_FrameIndex].Get(), 0, m_SceneColorRTMSAA.Get(), 0, m_RTFormat);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_SceneColorRTMSAA.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_PrimitiveCmdList->ResourceBarrier(1, &ResourceBarrier);

		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_PrimitiveCmdList->ResourceBarrier(1, &ResourceBarrier);
	}

	m_PrimitiveCmdList->Close();
	m_CommandLists.push_back(m_PrimitiveCmdList.Get());

}

void PhongShadingModel::InitMeshResourcces()
{
	ComPtr<ID3DBlob> Error;
	{
		D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
		ZeroMemory(&rootSignDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignDesc.NumParameters = 6;
		D3D12_ROOT_PARAMETER RootParameters[6];
		//view
		RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		RootParameters[0].Descriptor.ShaderRegister = 0;
		RootParameters[0].Descriptor.RegisterSpace = 0;
		//Primitive
		RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		RootParameters[1].Descriptor.ShaderRegister = 1;
		RootParameters[1].Descriptor.RegisterSpace = 0;
		
		//AmbientLight
		RootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParameters[2].Descriptor.ShaderRegister = 2;
		RootParameters[2].Descriptor.RegisterSpace = 0;
		//PointLight
		RootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParameters[3].Descriptor.ShaderRegister = 3;
		RootParameters[3].Descriptor.RegisterSpace = 0;
		//Material
		RootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParameters[4].Descriptor.ShaderRegister = 4;
		RootParameters[4].Descriptor.RegisterSpace = 0;
		//view
		RootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParameters[5].Descriptor.ShaderRegister = 0;
		RootParameters[5].Descriptor.RegisterSpace = 0;

		rootSignDesc.pParameters = RootParameters;
		rootSignDesc.NumStaticSamplers = 0;
		rootSignDesc.pStaticSamplers = 0;
		rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		assert(S_OK == D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_PrimitiveRootSign));
	}

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;

// 	D3D_SHADER_MACRO Micros[] = 
// 	{
// 		{"USE_BLINPHONG",	bUseBlinPhong ? "1" : "0"},
// 		{"USE_PHONG",		bUseBlinPhong ? "0" : "1"},
// 		{ NULL,				NULL },
// 	};
	HRESULT hr;
	hr = D3DCompileFromFile(TEXT("PhongModel.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
	if (FAILED(hr))
	{
		LOGA("%s", Error->GetBufferPointer());
		return;
	}
	hr = D3DCompileFromFile(TEXT("PhongModel.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PS, &Error);
	if (FAILED(hr))
	{
		LOGA("%s", Error->GetBufferPointer());
		return;
	}
	D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
	ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	GPSDesc.pRootSignature = m_PrimitiveRootSign.Get();
	GPSDesc.VS = { reinterpret_cast<UINT8*>(VS->GetBufferPointer()), VS->GetBufferSize() };
	GPSDesc.PS = { reinterpret_cast<UINT8*>(PS->GetBufferPointer()), PS->GetBufferSize() };
	GPSDesc.InputLayout = { InputElementDesc, _countof(InputElementDesc) };
	GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
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
	GPSDesc.DepthStencilState.DepthEnable = TRUE;
	GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	GPSDesc.DepthStencilState.StencilEnable = FALSE;
	GPSDesc.SampleMask = UINT_MAX;
	GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GPSDesc.NumRenderTargets = 1;
	GPSDesc.RTVFormats[0] = m_RTFormat;
	GPSDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	GPSDesc.SampleDesc.Count = m_SampleCount;
	GPSDesc.SampleDesc.Quality = m_SampleQuality;
	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_PrimitivePipelineState));

	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_PrimitivePipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_PrimitiveCmdList));
	m_PrimitiveCmdList->Close();

	m.LoadFBX("./Primitives/Sphere.fbx");

	{
		const UINT VertexBufferSize = m.Vertices.size() * sizeof(MeshVertex);

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = VertexBufferSize;
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
			&m_PrimitiveVertexBuffer));

		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_PrimitiveVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, m.Vertices.data(), m.Vertices.size() * sizeof(MeshVertex));
		m_PrimitiveVertexBuffer->Unmap(0, nullptr);

		m_PrimitiveVertexBufferView.BufferLocation = m_PrimitiveVertexBuffer->GetGPUVirtualAddress();
		m_PrimitiveVertexBufferView.StrideInBytes = sizeof(MeshVertex);
		m_PrimitiveVertexBufferView.SizeInBytes = VertexBufferSize;
	}

	{
		const UINT IndexBufferSize = m.Indices.size() * sizeof(int);

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = IndexBufferSize;
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
			&m_PrimitiveIndexBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_PrimitiveIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, m.Indices.data(), m.Indices.size() * sizeof(int));
		m_PrimitiveIndexBuffer->Unmap(0, nullptr);

		m_PrimitiveIndexBufferView.BufferLocation = m_PrimitiveIndexBuffer->GetGPUVirtualAddress();
		m_PrimitiveIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_PrimitiveIndexBufferView.SizeInBytes = IndexBufferSize;
	}

	{
		ViewUniform View;
		View.ViewSizeAndInvSize = XMFLOAT4(1920.f, 1080.f, 1.f / 1920.f, 1.f / 1080.f);
		View.ViewRectMin = XMFLOAT4(0, 0, 0, 0);

		{
			XMFLOAT4X4 PerspectiveProjection;
			XMMATRIX XMPerspectiveProjection = XMMatrixPerspectiveFovLH(90.f, 1920.f / 1080.f, 10, 100000.f);
			XMStoreFloat4x4(&PerspectiveProjection, XMPerspectiveProjection);
			View.WorldToClip = PerspectiveProjection;

			XMVECTOR Determinent;
			XMMATRIX XMInversePerspectiveProjection = XMMatrixInverse(&Determinent, XMPerspectiveProjection);
			XMFLOAT4X4 InversePerspectiveProjection;
			XMStoreFloat4x4(&InversePerspectiveProjection, XMInversePerspectiveProjection);
			View.ClipToWorld = InversePerspectiveProjection;

			float Mx = 2.0f * View.ViewSizeAndInvSize.z;
			float My = -2.0f * View.ViewSizeAndInvSize.w;
			float Ax = -1.0f /*- 2.0f * EffectiveViewRect.Min.X * View.ViewSizeAndInvSize.Z*/;
			float Ay = 1.0f /*+ 2.0f * EffectiveViewRect.Min.Y * View.ViewSizeAndInvSize.W*/;
			XMFLOAT4X4 Temp = XMFLOAT4X4
			(
				Mx, 0, 0, 0,
				0, My, 0, 0,
				0, 0, 1, 0,
				Ax, Ay, 0, 1
			);
			XMMATRIX XMTemp;
			XMTemp = XMLoadFloat4x4(&Temp);
			XMTemp = XMMatrixMultiply(XMTemp, XMInversePerspectiveProjection);
			XMStoreFloat4x4(&Temp, XMTemp);
			View.SvPositionToWorld = Temp;
		}

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(View);
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
			&m_ViewUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_ViewUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &View, sizeof(View));
		m_ViewUniformBuffer->Unmap(0, nullptr);
	}

	{
		PrimitiveUniform Primitive;
		XMMATRIX LocalToWorld = XMMatrixTranslation(0,0,40.f);
		XMStoreFloat4x4(&Primitive.LocalToWorld, LocalToWorld);
		Primitive.LocalToWorldDeterminantSign = -1;

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(Primitive);
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
			&m_PrimitiveUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_PrimitiveUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &Primitive, sizeof(Primitive));
		m_PrimitiveUniformBuffer->Unmap(0, nullptr);

// 		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
// 		ZeroMemory(&Desc, 0);
// 		Desc.BufferLocation = m_PrimitiveUniformBuffer->GetGPUVirtualAddress();
// 		Desc.SizeInBytes = sizeof(Primitive);
// 
// 		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
// 		m_D3D12Device->CreateConstantBufferView(&Desc, DescHandle);
	}

	{
		AmbientLightUniform AmbientLight;
		AmbientLight.AmbientLightColor = XMFLOAT3(1.f, 1.f, 1.f);

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(AmbientLight);
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
			&m_AmbientLightUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_AmbientLightUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &AmbientLight, sizeof(AmbientLight));
		m_AmbientLightUniformBuffer->Unmap(0, nullptr);
	}

	{
		PointLightUniform PointLight;
		PointLight.LightPosition = XMFLOAT4(100.f, 100.f, -100.f,1.0f);
		PointLight.LightColor = XMFLOAT4(1.f, 1.f, 1.f,1.0f);
		PointLight.Intencity = 20000.f;
		PointLight.FadeOffExponent = 2.f;

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(PointLight);
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
			&m_PointLightUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_PointLightUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &PointLight, sizeof(PointLight));
		m_PointLightUniformBuffer->Unmap(0, nullptr);

// 		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
// 		ZeroMemory(&Desc, 0);
// 		Desc.BufferLocation = m_PointLightUniformBuffer->GetGPUVirtualAddress();
// 		Desc.SizeInBytes = sizeof(PointLight);
// 
// 		D3D12_CPU_DESCRIPTOR_HANDLE DescHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
// 		DescHandle.ptr += m_CBVSRVUAVDescriptorSize;
// 		m_D3D12Device->CreateConstantBufferView(&Desc, DescHandle);
	}

	{
		MaterialUniform Material;
		Material.ka = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
		Material.kd = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
		Material.ks = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		Material.alpha = 5.f;
		Material.ShadingModel = 0;

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(Material);
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
			&m_MaterialUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = readRange.End = 0;
		m_MaterialUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &Material, sizeof(Material));
		m_MaterialUniformBuffer->Unmap(0, nullptr);
	}
}

void PhongShadingModel::UpdatePointLight(float fScreenX, float fScreenY)
{
	PointLightUniform PointLight;
	float XYAmplify = 200.f;
	PointLight.LightPosition = XMFLOAT4((fScreenX - 0.5f)*XYAmplify, (0.5f - fScreenY)*XYAmplify, -100.f, 1.0f);
	PointLight.LightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	PointLight.Intencity = 20000.f;
	PointLight.FadeOffExponent = 2.f;

	UINT8* pDataBegin;
	D3D12_RANGE readRange;
	readRange.Begin = readRange.End = 0;
	m_PointLightUniformBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
	memcpy(pDataBegin, &PointLight, sizeof(PointLight));
	m_PointLightUniformBuffer->Unmap(0, nullptr);
}

