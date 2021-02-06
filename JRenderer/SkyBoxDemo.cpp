#include "SkyBoxDemo.h"
#include <d3dx12.h>
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
	m_SkyBoxCmdList->IASetIndexBuffer(&m_SkyBoxIndexBufferView);
	m_SkyBoxCmdList->SetGraphicsRootSignature(m_SkyBoxRootSignature.Get());
	m_SkyBoxCmdList->SetPipelineState(m_SkyBoxPipelineState.Get());
	m_SkyBoxCmdList->SetDescriptorHeaps(1,m_CBVSRVUAVDescHeap.GetAddressOf());
	m_SkyBoxCmdList->SetGraphicsRootConstantBufferView(0, m_SkyBoxViewUniformBuffer->GetGPUVirtualAddress());
	m_SkyBoxCmdList->SetGraphicsRootDescriptorTable(1,m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
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

	m_CommandLists.push_back(m_SkyBoxCmdList.Get());

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
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
		ClearValue.Format = m_CubeMapRTFormat;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), &m_CubeMapRT));
	
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Format = m_CubeMapRTFormat;
		Desc.Texture2DArray.MipSlice = 0;
		Desc.Texture2DArray.FirstArraySlice = 0;
		Desc.Texture2DArray.ArraySize = 6;
		Desc.Texture2DArray.PlaneSlice = 0;
		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		m_CubeMapRTHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_CubeMapRTHandle.ptr += 2 * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(m_CubeMapRT.Get(), &Desc, m_CubeMapRTHandle);
	}
	
	//rootsignature
	{
		ComPtr<ID3DBlob> Error;
		D3D12_ROOT_PARAMETER RootParams[2];
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

		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].RegisterSpace = 0;
		Ranges[0].OffsetInDescriptorsFromTableStart = 1;

		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].RegisterSpace = 0;
		Ranges[1].OffsetInDescriptorsFromTableStart = 0;

// 		Ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
// 		Ranges[2].NumDescriptors = 1;
// 		Ranges[2].BaseShaderRegister = 0;
// 		Ranges[2].RegisterSpace = 0;
// 		Ranges[2].OffsetInDescriptorsFromTableStart = 0;

		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
		RootParams[0].DescriptorTable.NumDescriptorRanges = 1;
		RootParams[0].DescriptorTable.pDescriptorRanges = &Ranges[0];

		RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		RootParams[1].DescriptorTable.pDescriptorRanges = &Ranges[1];

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
		SamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.ShaderRegister = 0;
		SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		ZeroMemory(&RootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		RootSignatureDesc.NumParameters = _countof(RootParams);
		RootSignatureDesc.pParameters = RootParams;
		RootSignatureDesc.NumStaticSamplers = 1;
		RootSignatureDesc.pStaticSamplers = &SamplerDesc;
		RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		ComPtr<ID3DBlob> Signature;
		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), __uuidof(ID3D12RootSignature), &m_CubeMapRootSignature));
	}
	//pipeline
	{
		ComPtr<ID3DBlob> Error;
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> GS;
		ComPtr<ID3DBlob> PS;

		HRESULT hr;
		hr = D3DCompileFromFile(TEXT("GenCubeMap.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &VS, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		hr = D3DCompileFromFile(TEXT("GenCubeMap.hlsl"), nullptr, nullptr, "GSMain", "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &GS, &Error);
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
		GPSDesc.GS = { reinterpret_cast<UINT8*>(GS->GetBufferPointer()), GS->GetBufferSize() };
		GPSDesc.PS = { reinterpret_cast<UINT8*>(PS->GetBufferPointer()), PS->GetBufferSize() };
		GPSDesc.InputLayout = { InputElementDesc, _countof(InputElementDesc) };
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
		GPSDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		GPSDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		GPSDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		GPSDesc.RasterizerState.DepthClipEnable = FALSE;
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
		GPSDesc.SampleMask = UINT_MAX;
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = m_CubeMapRTFormat;
		GPSDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
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
			-1.0f,1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f,-1.0f, -1.0f,
			1.0f,-1.0f, -1.0f,
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
		XMStoreFloat4x4(&ViewUniform.FaceTransform[0], XMMatrixIdentity());//-Z
		XMStoreFloat4x4(&ViewUniform.FaceTransform[1], XMMatrixRotationY(90.0f));//+X
		XMStoreFloat4x4(&ViewUniform.FaceTransform[2], XMMatrixRotationY(180.0f));//+Z
		XMStoreFloat4x4(&ViewUniform.FaceTransform[3], XMMatrixRotationY(-90.0f));//-Z
		XMStoreFloat4x4(&ViewUniform.FaceTransform[4], XMMatrixRotationX(-90.0f));//+Y
		XMStoreFloat4x4(&ViewUniform.FaceTransform[5], XMMatrixRotationX(90.0f));//-Y
		XMStoreFloat4x4(&ViewUniform.Projection, XMMatrixIdentity());

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
		m_D3D12Device->CreateConstantBufferView(&Desc, m_CubeViewBufferHandle);
	}
	//texture
	{
		TexMetadata Metadata;
		ScratchImage Image;
		if (S_OK == LoadFromHDRFile(TEXT("./hdri/green_sanctuary_1k.hdr"), &Metadata, Image))
		{
			assert(S_OK == CreateTexture(m_D3D12Device.Get(), Image.GetMetadata(), m_HDRI.GetAddressOf()));
			std::vector<D3D12_SUBRESOURCE_DATA> SubResources;
			assert(S_OK == PrepareUpload(m_D3D12Device.Get(), Image.GetImages(), Image.GetImageCount(), Metadata, SubResources));
			const UINT64 UploadBufferSize = GetRequiredIntermediateSize(m_HDRI.Get(), 0, static_cast<unsigned int>(SubResources.size()));
			ComPtr<ID3D12Resource> TextureUploadHeap;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(TextureUploadHeap.GetAddressOf())
			));

			m_D3D12CmdAllocator->Reset();
			m_CubeMapCmdList->Reset(m_D3D12CmdAllocator.Get(), m_CubeMapPipelineState.Get());
			UpdateSubresources(m_CubeMapCmdList.Get(),
				m_HDRI.Get(), TextureUploadHeap.Get(),
				0, 0, static_cast<unsigned int>(SubResources.size()),
				SubResources.data());

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
			ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			ResourceBarrier.Transition.pResource = m_HDRI.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			m_CubeMapCmdList->ResourceBarrier(1, &ResourceBarrier);

			m_CubeMapCmdList->Close();
			ID3D12CommandList* CommandList[] = { m_CubeMapCmdList.Get() };
			m_D3D12CmdQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

			WaitForPreviousFrame();
		}
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.PlaneSlice = 0;
		m_HDRISRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_HDRISRVHandle.ptr += m_CBVSRVUAVDescriptorSize;
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
	m_CubeMapCmdList->Close();

	m_CommandLists.push_back(m_CubeMapCmdList.Get());
	m_D3D12CmdQueue->ExecuteCommandLists(m_CommandLists.size(), &m_CommandLists[0]);

	WaitForPreviousFrame();
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
	
	//rootsignature
	{
		ComPtr<ID3DBlob> Error;
		D3D12_ROOT_PARAMETER RootParams[2];
		RootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		RootParams[0].Descriptor.ShaderRegister = 0;
		RootParams[0].Descriptor.RegisterSpace = 0;

		D3D12_DESCRIPTOR_RANGE Range;
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Range.NumDescriptors = 1;
		Range.BaseShaderRegister = 0;
		Range.RegisterSpace = 0;
		Range.OffsetInDescriptorsFromTableStart = 0;
		RootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		RootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		RootParams[1].DescriptorTable.pDescriptorRanges = &Range;

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
		SamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
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
		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error);
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
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
		GPSDesc.RasterizerState.DepthClipEnable = FALSE;
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
		GPSDesc.SampleMask = UINT_MAX;
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = m_RTFormat;
		GPSDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
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
			//-z
			-1.0f,1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f,-1.0f, -1.0f,
			1.0f,-1.0f, -1.0f,
			//+z
			-1.0f,1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
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
			&m_SkyBoxVertexBuffer
		));

		UINT8* pVertexDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_SkyBoxVertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, CubeVertices, VertexBufferSize);
		m_SkyBoxVertexBuffer->Unmap(0, nullptr);

		m_SkyBoxVertexBufferView.BufferLocation = m_SkyBoxVertexBuffer->GetGPUVirtualAddress();
		m_SkyBoxVertexBufferView.StrideInBytes = sizeof(float) * 3;
		m_SkyBoxVertexBufferView.SizeInBytes = VertexBufferSize;
	}
	//index buffer
	{
		UINT16 Indices[] = {
			0,1,2,2,1,3,//-z
			1,5,3,3,5,7,//+x
			5,4,7,7,4,6,//+z
			4,0,6,6,0,2,//+z
			4,5,0,0,5,1,//+y
			7,6,3,3,6,2,//-y
		};
		const UINT32 IndexBufferSize = sizeof(Indices);
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
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
			&HeapProps,
			D3D12_HEAP_FLAG_NONE,
			&ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(ID3D12Resource),
			&m_SkyBoxIndexBuffer
		));

		UINT8* pVertexDataBegin;
		D3D12_RANGE ReadRange;
		ReadRange.Begin = ReadRange.End = 0;
		m_SkyBoxIndexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, Indices, sizeof(Indices));
		m_SkyBoxIndexBuffer->Unmap(0, nullptr);

		m_SkyBoxIndexBufferView.BufferLocation = m_SkyBoxIndexBuffer->GetGPUVirtualAddress();
		m_SkyBoxIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		m_SkyBoxIndexBufferView.SizeInBytes = sizeof(Indices);
	}
	//constant buffer
	{
		SkyBoxViewUniform ViewUniform;
		XMStoreFloat4x4(&ViewUniform.WorldToProj, XMMatrixPerspectiveFovLH(90.0f,1920.f/1080.f,10,10000.f));

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
		m_SkyBoxViewBufferHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&Desc, m_SkyBoxViewBufferHandle);
	}
	//texture
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		SRVDesc.TextureCube.MipLevels = 1;
		SRVDesc.TextureCube.MostDetailedMip = 0;
		m_SkyBoxSRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_SkyBoxSRVHandle.ptr += 3 * m_CBVSRVUAVDescriptorSize;
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
}

