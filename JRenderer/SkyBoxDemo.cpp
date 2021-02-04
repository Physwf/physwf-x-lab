#include "SkyBoxDemo.h"
#include <cassert>

void SkyBoxDemo::InitPipelineStates()
{
	InitCubemapPass();
	UpdateCubeMap();
	InitSceneColorPass();
}

void SkyBoxDemo::Draw()
{
	m_CommandLists.clear();
	m_SkyBoxCmdList->Reset(m_D3D12CmdAllocator.Get(),m_SkyBoxPipelineState.Get() );
	m_SkyBoxCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_SkyBoxCmdList->IASetVertexBuffers(0, 1, &m_SkyBoxVertexBufferView);
	m_SkyBoxCmdList->SetGraphicsRootSignature(m_SkyBoxRootSignature.Get());
	m_SkyBoxCmdList->SetPipelineState(m_SkyBoxPipelineState.Get());
	m_SkyBoxCmdList->SetDescriptorHeaps(1,m_CBVSRVUAVDescHeap.GetAddressOf());
	auto Handle = m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart();
	//Handle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
	m_SkyBoxCmdList->SetGraphicsRootDescriptorTable(0, Handle);
	m_SkyBoxCmdList->RSSetViewports(1, &m_Viewport);
	m_SkyBoxCmdList->RSSetScissorRects(1, &m_ScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
	m_SkyBoxCmdList->OMSetRenderTargets(1, &rtvHandle, 0, nullptr);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_SkyBoxCmdList->ResourceBarrier(1, &ResourceBarrier);

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };

	m_SkyBoxCmdList->DrawInstanced(4, 1, 0, 0);

	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_SkyBoxCmdList->ResourceBarrier(1, &ResourceBarrier);

	m_CommandLists.push_back(m_CubeMapCmdList.Get());

	m_SkyBoxCmdList->Close();
}

void SkyBoxDemo::InitCubemapPass()
{
	m_CubeMapViewport.TopLeftX = 0;
	m_CubeMapViewport.TopLeftY = 0;
	m_CubeMapViewport.Width = 256;
	m_CubeMapViewport.Height = 256;
	m_CubeMapViewport.MinDepth = 0.0f;
	m_CubeMapViewport.MaxDepth = 1.0f;

	m_CubeMapScissorRect.left = m_ScissorRect.top = 0;
	m_CubeMapScissorRect.right = 256;
	m_CubeMapScissorRect.bottom = 256;

	m_CubeMapRTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	//rendertarget
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = 512;
		ResourceDesc.Height = 512;
		ResourceDesc.DepthOrArraySize = 6;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = m_CubeMapRTFormat;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
		ClearValue.Format = m_CubeMapRTFormat;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), &m_CubeMapRT));
	
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		ZeroMemory(&Desc, 0);
		Desc.Format = m_CubeMapRTFormat;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		m_CubeMapRTHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_CubeMapRTHandle.ptr += 2 * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(m_CubeMapRT.Get(), &Desc, m_CubeMapRTHandle);
	}
	//texture
	{
		TexMetadata Metadata;
		ScratchImage Image;
		if (S_OK == LoadFromHDRFile(TEXT("./hdri/green_sanctuary_1k.hdr"), &Metadata, Image))
		{
			assert(S_OK == CreateTexture(m_D3D12Device.Get(), Metadata, m_HDRI.GetAddressOf()));
		}
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		m_HDRISRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateShaderResourceView(m_HDRI.Get(), &SRVDesc, m_HDRISRVHandle);

// 		D3D12_SAMPLER_DESC Desc;
// 		ZeroMemory(&Desc, sizeof(Desc));
// 		Desc.Filter = D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
// 		Desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.MipLODBias = 0.0f;
// 		m_HDRISRVSamplerHandle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
// 		m_D3D12Device->CreateSampler(&Desc, m_HDRISRVSamplerHandle);
	}
	//rootsignature
	{
		ComPtr<ID3DBlob> Error;
		D3D12_ROOT_PARAMETER RootParams[1];
// 		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
// 		RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
// 		RootParams[0].Descriptor.ShaderRegister = 0;
// 		RootParams[0].Descriptor.RegisterSpace = 0;
// 
// 		RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
// 		RootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
// 		RootParams[1].Descriptor.ShaderRegister = 0;
// 		RootParams[1].Descriptor.RegisterSpace = 0;

		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(&Ranges, sizeof(Ranges));

		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].RegisterSpace = 0;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;

		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].RegisterSpace = 0;
		Ranges[1].OffsetInDescriptorsFromTableStart = 1;

// 		Ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
// 		Ranges[2].NumDescriptors = 1;
// 		Ranges[2].BaseShaderRegister = 0;
// 		Ranges[2].RegisterSpace = 0;
// 		Ranges[2].OffsetInDescriptorsFromTableStart = 0;

		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParams[0].DescriptorTable.NumDescriptorRanges = 2;
		RootParams[0].DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
		SamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.ShaderRegister = 0;
		SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		RootSignatureDesc.NumParameters = _countof(RootParams);
		RootSignatureDesc.pParameters = RootParams;
		RootSignatureDesc.NumStaticSamplers = 1;
		RootSignatureDesc.pStaticSamplers = &SamplerDesc;
		RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_CubeMapRootSignature));
	}
	//pipeline
	{
		ComPtr<ID3DBlob> Error;
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		HRESULT hr;
		hr = D3DCompileFromFile(TEXT("GenCubeMap.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		hr = D3DCompileFromFile(TEXT("GenCubeMap.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}

		D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		GPSDesc.pRootSignature = m_CubeMapRootSignature.Get();
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
		GPSDesc.RTVFormats[0] = m_CubeMapRTFormat;
		GPSDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		GPSDesc.SampleDesc.Count = 1;
		GPSDesc.SampleDesc.Quality = 0;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_CubeMapPipelineState));
	}
	//command list
	{
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_CubeMapPipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_CubeMapCmdList));
		m_CubeMapCmdList->Close();
	}
	//vertex buffer
	{
		//0-1
		//2-3
		float TriangleStripQuad[] = 
		{
			-1.0f,1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f,-1.0f, 0.0f,
			1.0f,-1.0f, 0.0f,
		};

		const UINT VertexBufferSize = sizeof(TriangleStripQuad);

		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
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
			&HeapProps,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_QuadVertexBuffer
		));

		UINT8* pVertexDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_QuadVertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, TriangleStripQuad, sizeof(TriangleStripQuad));
		m_QuadVertexBuffer->Unmap(0, nullptr);

		m_QuadVertexBufferView.BufferLocation = m_QuadVertexBuffer->GetGPUVirtualAddress();
		m_QuadVertexBufferView.StrideInBytes = sizeof(float) * 3;
		m_QuadVertexBufferView.SizeInBytes = VertexBufferSize;
	}
	//constant buffer
	{
		CubeMapViewUniform ViewUniform;
		ViewUniform.FaceTransform[0];

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ViewUniform);
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
			&m_CubeViewUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_CubeViewUniformBuffer->Map(0,&ReadRange,reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &ViewUniform, sizeof(ViewUniform));
		m_CubeViewUniformBuffer->Unmap(0, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.BufferLocation = m_CubeViewUniformBuffer->GetGPUVirtualAddress();
		Desc.SizeInBytes = sizeof(ViewUniform);
		m_CubeViewBufferHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_CubeViewBufferHandle.ptr += m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&Desc, m_CubeViewBufferHandle);
	}
}

void SkyBoxDemo::UpdateCubeMap()
{
	m_CommandLists.clear();


	m_D3D12CmdAllocator->Reset();
	m_CubeMapCmdList->Reset(m_D3D12CmdAllocator.Get(), m_CubeMapPipelineState.Get());

	m_CubeMapCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_CubeMapCmdList->IASetVertexBuffers(0, 1, &m_QuadVertexBufferView);
	m_CubeMapCmdList->SetGraphicsRootSignature(m_CubeMapRootSignature.Get());
	m_CubeMapCmdList->SetPipelineState(m_CubeMapPipelineState.Get());
	m_CubeMapCmdList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	m_CubeMapCmdList->SetGraphicsRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
	//m_CubeMapCmdList->
	m_CubeMapCmdList->RSSetViewports(1, &m_CubeMapViewport);
	m_CubeMapCmdList->RSSetScissorRects(1, &m_CubeMapScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += 2 * m_RTVDescriptorSize;
	m_CubeMapCmdList->OMSetRenderTargets(1, &rtvHandle, 0, nullptr);

	

	m_CubeMapCmdList->DrawInstanced(4, 1, 0, 0);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = m_CubeMapRT.Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_CubeMapCmdList->ResourceBarrier(1, &ResourceBarrier);

	m_CommandLists.push_back(m_CubeMapCmdList.Get());

	m_D3D12CmdQueue->ExecuteCommandLists(m_CommandLists.size(), &m_CommandLists[0]);

	m_CubeMapCmdList->Close();
}

void SkyBoxDemo::InitSceneColorPass()
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
	//texture
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		SRVDesc.TextureCube.MipLevels = 1;
		SRVDesc.TextureCube.MostDetailedMip = 0;
		m_SkyBoxSRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_SkyBoxSRVHandle.ptr += m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(m_CubeMapRT.Get(), &SRVDesc, m_SkyBoxSRVHandle);

// 		D3D12_SAMPLER_DESC Desc;
// 		ZeroMemory(&Desc, sizeof(Desc));
// 		Desc.Filter = D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
// 		Desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
// 		Desc.MipLODBias = 0.0f;
// 		m_SkyBoxSRVSamplerHandle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
// 		m_SkyBoxSRVSamplerHandle.ptr += m_SamplerDescriptorSize;
// 		m_D3D12Device->CreateSampler(&Desc, m_SkyBoxSRVSamplerHandle);
	}
	//rootsignature
	{
		ComPtr<ID3DBlob> Error;
		D3D12_ROOT_PARAMETER RootParams[2];
		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		RootParams[0].Descriptor.ShaderRegister = 0;
		RootParams[0].Descriptor.RegisterSpace = 0;

		RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		RootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParams[1].Descriptor.ShaderRegister = 0;
		RootParams[1].Descriptor.RegisterSpace = 0;

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
		SamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.ShaderRegister = 0;
		SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		RootSignatureDesc.NumParameters = _countof(RootParams);
		RootSignatureDesc.pParameters = RootParams;
		RootSignatureDesc.NumStaticSamplers = 1;
		RootSignatureDesc.pStaticSamplers = &SamplerDesc;
		RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_SkyBoxRootSignature));
	}
	//pipeline
	{
		ComPtr<ID3DBlob> Error;
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		HRESULT hr;
		hr = D3DCompileFromFile(TEXT("SkyBox.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		hr = D3DCompileFromFile(TEXT("SkyBox.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &PS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}

		D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		GPSDesc.pRootSignature = m_SkyBoxRootSignature.Get();
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
		GPSDesc.SampleDesc.Count = 1;
		GPSDesc.SampleDesc.Quality = 0;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), &m_SkyBoxPipelineState));
	}
	//command list
	{
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), m_CubeMapPipelineState.Get(), __uuidof(ID3D12GraphicsCommandList), &m_SkyBoxCmdList));
		m_SkyBoxCmdList->Close();
	}
	//vertex buffer
	{
		float CubeVertices[] =
		{
			//-x
			-1.0f,1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f,-1.0f, 0.0f,
			1.0f,-1.0f, 0.0f,
		};
		const UINT VertexBufferSize = sizeof(CubeVertices);

		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
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
			&HeapProps,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_QuadVertexBuffer
		));

		UINT8* pVertexDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_SkyBoxVertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, CubeVertices, sizeof(CubeVertices));
		m_SkyBoxVertexBuffer->Unmap(0, nullptr);

		m_SkyBoxVertexBufferView.BufferLocation = m_SkyBoxVertexBuffer->GetGPUVirtualAddress();
		m_SkyBoxVertexBufferView.StrideInBytes = sizeof(float) * 3;
		m_SkyBoxVertexBufferView.SizeInBytes = VertexBufferSize;
	}

	//constant buffer
	{
		SkyBoxViewUniform ViewUniform;
		ViewUniform.WorldToProj;

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CreationNodeMask = 1;
		HeapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ViewUniform);
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
			&m_SkyBoxViewUniformBuffer));

		UINT8* pDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_SkyBoxViewUniformBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pDataBegin));
		memcpy(pDataBegin, &ViewUniform, sizeof(ViewUniform));
		m_SkyBoxViewUniformBuffer->Unmap(0, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.BufferLocation = m_CubeViewUniformBuffer->GetGPUVirtualAddress();
		Desc.SizeInBytes = sizeof(ViewUniform);
		m_SkyBoxViewBufferHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_SkyBoxViewBufferHandle.ptr += m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&Desc, m_SkyBoxViewBufferHandle);
	}
}

