#include "ShadingModels.h"
#include "DirectXTex.h"
#include "d3dx12.h"
using namespace DirectX;
#include <pix3.h>

#define PI 3.14

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

// void PBRShadingModel::Draw()
// {
// 	DrawSkyBox();
// 	DrawPrimitives();
// 	PostProcess();
// }

void PBRShadingModel::LoadGenEnviPipelineState()
{
	//EnvironmentMap
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].RegisterSpace = 0;
		Ranges[0].OffsetInDescriptorsFromTableStart = 1;

		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].RegisterSpace = 0;
		Ranges[1].OffsetInDescriptorsFromTableStart = 2;

		D3D12_ROOT_PARAMETER RootParameter;
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		RootParameter.DescriptorTable.NumDescriptorRanges = _countof(Ranges);
		RootParameter.DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
		SamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		SamplerDesc.ShaderRegister = 0;
		SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_ROOT_SIGNATURE_DESC RSDesc;
		ZeroMemory(&RSDesc, sizeof(RSDesc));
		RSDesc.NumParameters = 1;
		RSDesc.pParameters = &RootParameter;
		RSDesc.NumStaticSamplers = 1;
		RSDesc.pStaticSamplers = &SamplerDesc;
		RSDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RSBlob;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RSDesc,D3D_ROOT_SIGNATURE_VERSION_1,RSBlob.GetAddressOf(),Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0,RSBlob->GetBufferPointer(),RSBlob->GetBufferSize(),__uuidof(ID3D12RootSignature),(void**)mGenEnviMapRootSignature.GetAddressOf()));

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> GS;
		ComPtr<ID3DBlob> PS;

		HRESULT hr;
		hr = D3DCompileFromFile(TEXT("GenEnvironmentMap.hlsl"),NULL,NULL,"VSMain","vs_5_0",D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,0,VS.GetAddressOf(),Error.GetAddressOf());
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			assert(FALSE);
		}
		hr = D3DCompileFromFile(TEXT("GenEnvironmentMap.hlsl"), NULL, NULL, "GSMain", "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, GS.GetAddressOf(), Error.GetAddressOf());
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			assert(FALSE);
		}
		hr = D3DCompileFromFile(TEXT("GenEnvironmentMap.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf());
		if (FAILED(hr))
		{
			LOGA("%s", Error->GetBufferPointer());
			assert(FALSE);
		}

		D3D12_INPUT_ELEMENT_DESC InputDesc[] = {
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
		ZeroMemory(&PSODesc, sizeof(PSODesc));
		PSODesc.pRootSignature = mGenEnviMapRootSignature.Get();
		PSODesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		PSODesc.GS = { GS->GetBufferPointer(),GS->GetBufferSize() };
		PSODesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
		//PSODesc.StreamOutput;
		PSODesc.BlendState.IndependentBlendEnable = true;
		PSODesc.BlendState.RenderTarget[0].BlendEnable = true;
		PSODesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		PSODesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		PSODesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		PSODesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		PSODesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		PSODesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		PSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		PSODesc.SampleMask = 0xffffffff;
		PSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		PSODesc.DepthStencilState.DepthEnable = FALSE;
		PSODesc.DepthStencilState.StencilEnable = FALSE;
		PSODesc.InputLayout.pInputElementDescs = InputDesc;
		PSODesc.InputLayout.NumElements = _countof(InputDesc);
		//PSODesc.IBStripCutValue;
		PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PSODesc.NumRenderTargets = 1;
		PSODesc.SampleDesc = { 1,0 };
		PSODesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&PSODesc,__uuidof(ID3D12PipelineState),(void**)mGenEnviMapPSO.GetAddressOf()));
	}

	{
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mGenEnviMapPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mGenEnviMapCmdList.GetAddressOf()));
		mGenEnviMapCmdList->SetName(TEXT("mGenEnviMapCmdList"));
		mGenEnviMapCmdList->Close();
	}
}

void PBRShadingModel::LoadSkyboxPipelineState()
{
	//root sign
	{
		D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTable;
		ZeroMemory(&DescriptorTable, sizeof(DescriptorTable));

		D3D12_DESCRIPTOR_RANGE Ranges[2];
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].RegisterSpace = 0;
		Ranges[0].OffsetInDescriptorsFromTableStart = 3;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].RegisterSpace = 0;
		Ranges[1].OffsetInDescriptorsFromTableStart = 0;
		DescriptorTable.NumDescriptorRanges = 2;
		DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_ROOT_PARAMETER Parameters[2];
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].DescriptorTable = DescriptorTable;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_STATIC_SAMPLER_DESC CubeMapSamplerDesc;
		ZeroMemory(&CubeMapSamplerDesc, sizeof(CubeMapSamplerDesc));
		CubeMapSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		CubeMapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.ShaderRegister = 0;
		CubeMapSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		CubeMapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.NumParameters = 1;
		RootSignDesc.pParameters = Parameters;
		RootSignDesc.NumStaticSamplers = 1;
		RootSignDesc.pStaticSamplers = &CubeMapSamplerDesc;
		
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mSkyboxRootSignature.GetAddressOf()));
	
		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		if (S_OK != D3DCompileFromFile(TEXT("./Skybox.hlsl"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
		}
		
		if (S_OK != D3DCompileFromFile(TEXT("./Skybox.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
		}

		D3D12_INPUT_ELEMENT_DESC InputElements[] = 
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineDesc;
		ZeroMemory(&PipelineDesc, sizeof(PipelineDesc));
		PipelineDesc.pRootSignature = mSkyboxRootSignature.Get();
		PipelineDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		PipelineDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
		PipelineDesc.BlendState.AlphaToCoverageEnable = FALSE;
		PipelineDesc.BlendState.IndependentBlendEnable = FALSE;
		PipelineDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		PipelineDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		PipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		PipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		PipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		PipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		PipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		PipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		PipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		PipelineDesc.SampleMask = 0xffffffff;
		PipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		PipelineDesc.DepthStencilState.DepthEnable = FALSE;
		PipelineDesc.DepthStencilState.StencilEnable = FALSE;
		PipelineDesc.InputLayout.NumElements = _countof(InputElements);
		PipelineDesc.InputLayout.pInputElementDescs = InputElements;
		PipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PipelineDesc.NumRenderTargets = 1;
		PipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		PipelineDesc.SampleDesc.Count = 1;
		PipelineDesc.SampleDesc.Quality = 0;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&PipelineDesc, __uuidof(ID3D12PipelineState), (void**)mSkyboxPSO.GetAddressOf()));
	
		{
			assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mSkyboxPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mSkyboxCommandList.GetAddressOf()));
			mSkyboxCommandList->SetName(TEXT("mSkyboxCommandList"));
			mSkyboxCommandList->Close();
		}

		
	}
}

void PBRShadingModel::LoadTonemapPipelineState()
{
	//Histogram
	{
		D3D12_DESCRIPTOR_RANGE Ranges[3];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].OffsetInDescriptorsFromTableStart = 7;//hdr render target
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].OffsetInDescriptorsFromTableStart = 10;
		Ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		Ranges[2].BaseShaderRegister = 0;
		Ranges[2].NumDescriptors = 1;
		Ranges[2].OffsetInDescriptorsFromTableStart = 8;//histogram
		D3D12_ROOT_PARAMETER Parameters[1];
		ZeroMemory(&Parameters, sizeof(Parameters));
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		Parameters[0].DescriptorTable.NumDescriptorRanges = 3;
		Parameters[0].DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_ROOT_SIGNATURE_DESC SignDesc;
		ZeroMemory(&SignDesc, sizeof(SignDesc));
		SignDesc.NumParameters = 1;
		SignDesc.pParameters = Parameters;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&SignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mTonemapHistogramRootSignature.GetAddressOf()));

		ComPtr<ID3DBlob> CS;

		if (S_OK != D3DCompileFromFile(TEXT("Tonemap.hlsl"), NULL, NULL, "HistogramCSMain", "cs_5_0", 0, 0, CS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}

		D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc;
		ZeroMemory(&PipelineDesc, sizeof(PipelineDesc));
		PipelineDesc.pRootSignature = mTonemapHistogramRootSignature.Get();
		PipelineDesc.CS = { CS->GetBufferPointer(),CS->GetBufferSize() };
		assert(S_OK == m_D3D12Device->CreateComputePipelineState(&PipelineDesc, __uuidof(ID3D12PipelineState), (void**)mTonemapHistogramPSO.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_D3D12ComputeCmdAllocator.Get(), mTonemapHistogramPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mTonemapHistgramCommandList.GetAddressOf()));
		mTonemapHistgramCommandList->SetName(TEXT("mTonemapHistgramCommandList"));
		mTonemapHistgramCommandList->Close();
	}

	//Luminance Average
	{
		D3D12_DESCRIPTOR_RANGE Ranges[3];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].OffsetInDescriptorsFromTableStart = 10;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].OffsetInDescriptorsFromTableStart = 8;//histogram
		Ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		Ranges[2].BaseShaderRegister = 1;
		Ranges[2].NumDescriptors = 1;
		Ranges[2].OffsetInDescriptorsFromTableStart = 11;//average luminance

		D3D12_ROOT_PARAMETER Parameters[1];
		ZeroMemory(&Parameters, sizeof(Parameters));
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].DescriptorTable.NumDescriptorRanges = _countof(Ranges);
		Parameters[0].DescriptorTable.pDescriptorRanges = Ranges;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.NumParameters = 1;
		RootSignDesc.pParameters = Parameters;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mTonemapAvgLuminanceRootSignature.GetAddressOf()));
		
		ComPtr<ID3DBlob> CS;
		if (S_OK != D3DCompileFromFile(TEXT("Tonemap.hlsl"), nullptr, nullptr, "CSMain", "cs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, CS.GetAddressOf(),Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}
		D3D12_COMPUTE_PIPELINE_STATE_DESC CPSDesc;
		ZeroMemory(&CPSDesc, sizeof(CPSDesc));
		CPSDesc.pRootSignature = mTonemapAvgLuminanceRootSignature.Get();
		CPSDesc.CS = { CS->GetBufferPointer(),CS->GetBufferSize() };

		assert(S_OK == m_D3D12Device->CreateComputePipelineState(&CPSDesc, __uuidof(ID3D12PipelineState), (void**)mTonemapAvgLuminacePSO.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_D3D12ComputeCmdAllocator.Get(), mTonemapAvgLuminacePSO.Get(), __uuidof(ID3D12GraphicsCommandList),(void**)mTonemapAvgLuminaceCommandList.GetAddressOf()));
		mTonemapAvgLuminaceCommandList->SetName(TEXT("mTonemapAvgLuminaceCommandList"));
		mTonemapAvgLuminaceCommandList->Close();
	}

	//
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 1;
		Ranges[0].OffsetInDescriptorsFromTableStart = 7;//hdr render target
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].BaseShaderRegister = 1;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].OffsetInDescriptorsFromTableStart = 12;//average luminance


		D3D12_ROOT_PARAMETER Paramaters[1];
		ZeroMemory(&Paramaters, sizeof(Paramaters));
		Paramaters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Paramaters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		Paramaters[0].DescriptorTable.NumDescriptorRanges = 2;
		Paramaters[0].DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_STATIC_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
		SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		SamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		SamplerDesc.ShaderRegister = 0;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.NumParameters = 1;
		RootSignDesc.pParameters = Paramaters;
		RootSignDesc.NumStaticSamplers = 1;
		RootSignDesc.pStaticSamplers = &SamplerDesc;
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mTonemapRootSignature.GetAddressOf()));

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		if (S_OK != D3DCompileFromFile(TEXT("Tonemap.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		if (S_OK != D3DCompileFromFile(TEXT("Tonemap.hlsl"), nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}

		D3D12_INPUT_ELEMENT_DESC InputDesc[] = {
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mTonemapRootSignature.Get();
		GPSDesc.VS = { VS->GetBufferPointer(), VS->GetBufferSize() };
		GPSDesc.PS = { PS->GetBufferPointer(), PS->GetBufferSize() };
		GPSDesc.BlendState.AlphaToCoverageEnable = FALSE;
		GPSDesc.BlendState.IndependentBlendEnable = true;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = true;
		GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		GPSDesc.SampleMask = 0xffffffff;
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		GPSDesc.DepthStencilState.DepthEnable = FALSE;
		GPSDesc.InputLayout.pInputElementDescs = InputDesc;
		GPSDesc.InputLayout.NumElements = _countof(InputDesc);
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		GPSDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mTonemapPSO.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mTonemapPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mTonemapCommandList.GetAddressOf()));
		mTonemapCommandList->SetName(TEXT("mTonemapCommandList"));
		mTonemapCommandList->Close();
	}

}

void PBRShadingModel::LoadGenEnviAssets()
{
	{
		mGenEnviViewport.TopLeftX = 0;
		mGenEnviViewport.TopLeftY = 0;
		mGenEnviViewport.Width = 512.0f;
		mGenEnviViewport.Height = 512.0f;
		mGenEnviViewport.MinDepth = 0.0f;
		mGenEnviViewport.MaxDepth = 1.0f;
		mGenEnviScissorRect.left = 0.f;
		mGenEnviScissorRect.top = 0.f;
		mGenEnviScissorRect.right = 512.0f;
		mGenEnviScissorRect.bottom = 512.0f;
	}
	//EnvironmentMap RenderTarget
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Width = 512;
		ResourceDesc.Height = 512;
		ResourceDesc.DepthOrArraySize = 6;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(ClearValue));
		ClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), (void**)mEnvironmentMap.GetAddressOf()));
		mEnvironmentMap->SetName(TEXT("EnvironmentMap"));

		mGenEnviRTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mGenEnviRTVHandle.ptr += 2 * m_RTVDescriptorSize;
		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));
		RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		RTVDesc.Texture2DArray.ArraySize = 6;
		RTVDesc.Texture2DArray.FirstArraySlice = 0;
		RTVDesc.Texture2DArray.MipSlice = 0;
		RTVDesc.Texture2DArray.PlaneSlice = 0;
		m_D3D12Device->CreateRenderTargetView(mEnvironmentMap.Get(), &RTVDesc, mGenEnviRTVHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		SRVDesc.TextureCube.MipLevels = 1;
		SRVDesc.TextureCube.MostDetailedMip = 0;
		mEnviromentMapSRV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateShaderResourceView(mEnvironmentMap.Get(), &SRVDesc, mEnviromentMapSRV);
	}
	
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)CommandList.GetAddressOf()));
	CommandList->Close();
	CommandList->Reset(m_D3D12CmdAllocator.Get(), nullptr);
	//VB
	{
		//0-1
		//2-3
		float TriangleStripQuad[] =
		{
			-1.0f,1.0f, -1.f,
			1.0f, 1.0f, -1.f,
			-1.0f,-1.0f, -1.f,
			1.0f,-1.0f, -1.f,
		};

		const UINT VertexBufferSize = sizeof(TriangleStripQuad);

		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = VertexBufferSize;
		ResourceDesc.Height = 1;//BUFFER的Height必须为1
		ResourceDesc.DepthOrArraySize = 1;//BUFFER的DepthOrArraySize必须为1
		ResourceDesc.MipLevels = 1;//BUFFER的MipLevels必须为1
		ResourceDesc.SampleDesc = { 1,0 };//BUFFER的SampleDesc必须为{ 1,0 }
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;//BUFFER的Format必须为DXGI_FORMAT_UNKNOWN
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//BUFFER的Layout必须为D3D12_TEXTURE_LAYOUT_ROW_MAJOR
		//D3D12_HEAP_TYPE_UPLOAD的初始状态必须为D3D12_RESOURCE_STATE_GENERIC_READ
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mGenEnviMapVBUpload.GetAddressOf()));
		mGenEnviMapVBUpload->SetName(TEXT("mGenEnviMapVBUpload"));

		D3D12_RANGE Range;
		Range.Begin = Range.End = 0;
		void* pData = nullptr;
		assert(S_OK == mGenEnviMapVBUpload->Map(0, &Range, &pData));
		memcpy(pData, TriangleStripQuad, VertexBufferSize);
		mGenEnviMapVBUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = VertexBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mGenEnviMapVB.GetAddressOf()));
		mGenEnviMapVB->SetName(TEXT("mGenEnviMapVB"));

		CommandList->CopyResource(mGenEnviMapVB.Get(), mGenEnviMapVBUpload.Get());
		D3D12_RESOURCE_BARRIER Barrier;
		ZeroMemory(&Barrier, sizeof(Barrier));
		Barrier.Transition.pResource = mGenEnviMapVB.Get();
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		CommandList->ResourceBarrier(1, &Barrier);

		mGenEnviMapVBView.BufferLocation = mGenEnviMapVB->GetGPUVirtualAddress();
		mGenEnviMapVBView.SizeInBytes = VertexBufferSize;
		mGenEnviMapVBView.StrideInBytes = sizeof(float) * 3;
	}
	//GS Constant Buffer

	ComPtr<ID3D12Resource> ConstBufferUpload;

	{
		struct GSView
		{
			XMFLOAT4X4 FaceTransform[6];
			XMFLOAT4X4 Projection;
			float padding[16];
		};

		GSView View;
		XMStoreFloat4x4(&View.FaceTransform[0], XMMatrixRotationY(PI / 2.f));//+X
		XMStoreFloat4x4(&View.FaceTransform[1], XMMatrixRotationY(-PI / 2.f));//-X
		XMStoreFloat4x4(&View.FaceTransform[2], XMMatrixRotationX(PI / 2.f)* XMMatrixRotationY(PI));//+Y
		XMStoreFloat4x4(&View.FaceTransform[3], XMMatrixRotationX(-PI / 2.f) * XMMatrixRotationY(PI));//-Y
		XMStoreFloat4x4(&View.FaceTransform[4], XMMatrixRotationY(PI));//+Z
		XMStoreFloat4x4(&View.FaceTransform[5], XMMatrixIdentity());//-Z
		XMStoreFloat4x4(&View.Projection, XMMatrixIdentity());

		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(View);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ConstBufferUpload.GetAddressOf()));
		ConstBufferUpload->SetName(TEXT("ConstBufferUpload"));

		void* pData;
		ConstBufferUpload->Map(0, nullptr, &pData);
		memcpy(pData, &View, sizeof(View));
		ConstBufferUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mGenEnviMapConstBuffer.GetAddressOf()));
		mGenEnviMapConstBuffer->SetName(TEXT("mGenEnviMapConstBuffer"));


		CommandList->CopyResource(mGenEnviMapConstBuffer.Get(), ConstBufferUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mGenEnviMapConstBuffer.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		CommandList->ResourceBarrier(1,&ResourceBarrier);

		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
		ZeroMemory(&ConstViewDesc, sizeof(ConstViewDesc));
		ConstViewDesc.BufferLocation = mGenEnviMapConstBuffer->GetGPUVirtualAddress();
		ConstViewDesc.SizeInBytes = sizeof(View);
		mGenEnviMapCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mGenEnviMapCBView.ptr += 1 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mGenEnviMapCBView);
	}
	//HDRI Shader Resource
	{
		TexMetadata Metadata;
		ScratchImage Image;

		assert(S_OK == LoadFromHDRFile(TEXT("./hdri/wide_street_02_1k.hdr"), &Metadata, Image));

		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(Metadata.dimension);
		ResourceDesc.Width = Metadata.width;
		ResourceDesc.Height = Metadata.height;
		ResourceDesc.DepthOrArraySize = Metadata.arraySize;
		ResourceDesc.MipLevels = Metadata.mipLevels;
		ResourceDesc.Format = Metadata.format;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mHDRI.GetAddressOf()));
		mHDRI->SetName(TEXT("mHDRI"));

		std::vector<D3D12_SUBRESOURCE_DATA> SubResources;
		assert(S_OK == PrepareUpload(m_D3D12Device.Get(), Image.GetImages(), Image.GetImageCount(), Metadata, SubResources));
		const UINT64 UploadBufferSize = GetRequiredIntermediateSize(mHDRI.Get(), 0, SubResources.size());

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = UploadBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mHDRIUpload.GetAddressOf()));
		mHDRIUpload->SetName(TEXT("mHDRIUpload"));

		UpdateSubresources(CommandList.Get(), mHDRI.Get(), mHDRIUpload.Get(), 0,0, (UINT)SubResources.size(), SubResources.data());

		D3D12_RESOURCE_BARRIER Barrier;
		ZeroMemory(&Barrier, sizeof(Barrier));
		Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		Barrier.Transition.pResource = mHDRI.Get();
		Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		CommandList->ResourceBarrier(1, &Barrier);

		mHDRISRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mHDRISRVHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = Metadata.format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.PlaneSlice = 0;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0;
		SRVDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0, 1, 2, 3);
		m_D3D12Device->CreateShaderResourceView(mHDRI.Get(), &SRVDesc, mHDRISRVHandle);
	}

	{
		CommandList->Close();
		ID3D12CommandList* List[] = { CommandList.Get() };
		m_D3D12CmdQueue->ExecuteCommandLists(1, List);
		WaitForPreviousFrame();
	}
}

void PBRShadingModel::LoadSkyboxAssets()
{
	ComPtr<ID3D12GraphicsCommandList> CmdList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)CmdList.GetAddressOf()));
	CmdList->Close();
	CmdList->Reset(m_D3D12CmdAllocator.Get(), nullptr);
	//VB
	ComPtr<ID3D12Resource> SkyboxVBUpload;
	{
// 		float CubeVertices[] =
// 		{
// 			//-z
// 			-1000.0f,1000.0f, -1000.0f,
// 			1000.0f, 1000.0f, -1000.0f,
// 			-1000.0f,-1000.0f, -1000.0f,
// 			1000.0f,-1000.0f, -1000.0f,
// 			//+z
// 			-1000.0f,1000.0f, 1000.0f,
// 			1000.0f, 1000.0f, 1000.0f,
// 			-1000.0f,-1000.0f, 1000.0f,
// 			1000.0f,-1000.0f, 1000.0f,
// 		};

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
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = VertexBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)SkyboxVBUpload.GetAddressOf()));
		SkyboxVBUpload->SetName(TEXT("SkyboxVBUpload"));
		//D3D12_RANGE Range;
		void* pData;
		SkyboxVBUpload->Map(0, nullptr, (void**)&pData);
		memcpy(pData, CubeVertices, VertexBufferSize);
		SkyboxVBUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mSkyboxVB.GetAddressOf()));
		mSkyboxVB->SetName(TEXT("mSkyboxVB"));

		CmdList->CopyResource(mSkyboxVB.Get(), SkyboxVBUpload.Get());
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mSkyboxVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		CmdList->ResourceBarrier(1, &ResourceBarrier);

		mSkyboxVBView.BufferLocation = mSkyboxVB->GetGPUVirtualAddress();
		mSkyboxVBView.SizeInBytes = VertexBufferSize;
		mSkyboxVBView.StrideInBytes = sizeof(float) * 3;
	}

	//IB
	ComPtr<ID3D12Resource> SkyboxIBUpload;
	{
		UINT16 Indices[] =
		{
			1,5,3,3,5,7,//+x
			4,0,6,6,0,2,//-x
			4,5,0,0,5,1,//+y
			7,6,3,3,6,2,//-y
			5,4,7,7,4,6,//+z
			0,1,2,2,1,3,//-z
		};
		const UINT32 IndexBufferSize = sizeof(Indices);

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = IndexBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)SkyboxIBUpload.GetAddressOf()));
		SkyboxIBUpload->SetName(TEXT("SkyboxIBUpload"));

		void* pData;
		SkyboxIBUpload->Map(0, nullptr, (void**)&pData);
		memcpy(pData, Indices, IndexBufferSize);
		SkyboxIBUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mSkyboxIB.GetAddressOf()));
		mSkyboxIB->SetName(TEXT("mSkyboxIB"));

		CmdList->CopyResource(mSkyboxIB.Get(), SkyboxIBUpload.Get());
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mSkyboxIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		CmdList->ResourceBarrier(1, &ResourceBarrier);

		mSkyboxIBView.BufferLocation = mSkyboxIB->GetGPUVirtualAddress();
		mSkyboxIBView.SizeInBytes = IndexBufferSize;
		mSkyboxIBView.Format = DXGI_FORMAT_R16_UINT;
	}

	//constant buffer
	ComPtr<ID3D12Resource> SkyboxCBUpload;
	{
		struct ViewUniform
		{
			XMFLOAT4X4 WorldToProj;
			char padding[256 - sizeof(XMFLOAT4X4)];
		};

		ViewUniform View;
		XMStoreFloat4x4(&View.WorldToProj, XMMatrixIdentity() /*XMMatrixPerspectiveFovLH(90.0f,1920.f/1080.f,10,10000.f)*/);
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = sizeof(View);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)SkyboxCBUpload.GetAddressOf()));
		SkyboxCBUpload->SetName(TEXT("SkyboxCBUpload"));

		void* pData;
		D3D12_RANGE Range;
		Range.Begin = Range.End = 0;
		SkyboxCBUpload->Map(0, &Range, &pData);
		memcpy(pData, &View, sizeof(ViewUniform));
		SkyboxCBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mSkyboxCB.GetAddressOf()));
		mSkyboxCB->SetName(TEXT("mSkyboxCB"));

		CmdList->CopyResource(mSkyboxCB.Get(), SkyboxCBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mSkyboxCB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		CmdList->ResourceBarrier(1, &ResourceBarrier);

		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstBufferView;
		ZeroMemory(&ConstBufferView, sizeof(ConstBufferView));
		ConstBufferView.BufferLocation = mSkyboxCB->GetGPUVirtualAddress();
		ConstBufferView.SizeInBytes = sizeof(ViewUniform);
		mSkyboxCBVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mSkyboxCBVHandle.ptr += 3 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&ConstBufferView, mSkyboxCBVHandle);
	}

	{
		CmdList->Close();
		ID3D12CommandList* List[] = { CmdList.Get() };
		m_D3D12CmdQueue->ExecuteCommandLists(1, List);
		WaitForPreviousFrame();
	}
}

void PBRShadingModel::LoadCommonAssets()
{
	{
		m_Viewport.TopLeftX = 0.f;
		m_Viewport.TopLeftY = 0.f;
		m_Viewport.Width = 1920.f;
		m_Viewport.Height = 1080.f;
		m_Viewport.MinDepth = 0.f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect.left = 0.f;
		m_ScissorRect.top = 0.f;
		m_ScissorRect.right = 1920.f;
		m_ScissorRect.bottom = 1080.f;
	}
	//HDR Render Target
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		ResourceDesc.Width = 1920;
		ResourceDesc.Height = 1080;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearValue;
		ClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		memset(&ClearValue.Color, 0, sizeof(ClearValue.Color));
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), (void**)mHDRRT.GetAddressOf()));
		mHDRRT->SetName(TEXT("mHDRRT"));

		mHDRRTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mHDRRTVHandle.ptr += 3 * m_RTVDescriptorSize;

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));
		RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;
		m_D3D12Device->CreateRenderTargetView(mHDRRT.Get(), &RTVDesc, mHDRRTVHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		mHDRSRV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mHDRSRV.ptr += 7 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mHDRRT.Get(), &SRVDesc, mHDRSRV);

	}
	//Depth
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ResourceDesc.Width = 1920;
		ResourceDesc.Height = 1080;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE ClearValue;
		ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ClearValue.DepthStencil.Depth = 1.0f;
		ClearValue.DepthStencil.Stencil = 0;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void**)mDepthStencial.GetAddressOf()));
		mDepthStencial->SetName(TEXT("mDepthStencial"));

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(DSVDesc));
		DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

		mDepthStencialHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateDepthStencilView(mDepthStencial.Get(), &DSVDesc, mDepthStencialHandle);
	}
}

void PBRShadingModel::LoadTonemapAssets()
{
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	{
		assert(S_OK == m_D3D12Device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(),nullptr,__uuidof(ID3D12GraphicsCommandList),(void**)CommandList.GetAddressOf()));
		CommandList->Close();
	}
	//Histogram Buffer
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = 256 * sizeof(UINT);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, __uuidof(ID3D12Resource), (void**)mTonemapHistogramResult.GetAddressOf()));
		mTonemapHistogramResult->SetName(TEXT("mTonemapHistogramResult"));

		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		ZeroMemory(&UAVDesc, sizeof(UAVDesc));
		UAVDesc.Format = DXGI_FORMAT_R32_UINT;
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.NumElements = 256;
		UAVDesc.Buffer.StructureByteStride = 0;
		UAVDesc.Buffer.CounterOffsetInBytes = 0;
		UAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		mTonemapHistogramResultUAVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mTonemapHistogramResultUAVHandle.ptr += 8 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateUnorderedAccessView(mTonemapHistogramResult.Get(), nullptr, &UAVDesc, mTonemapHistogramResultUAVHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Format = DXGI_FORMAT_R32_UINT;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.FirstElement = 0;
		SRVDesc.Buffer.NumElements = 256;
		SRVDesc.Buffer.StructureByteStride = 0;
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		mTonemapHistogramResultSRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mTonemapHistogramResultSRVHandle.ptr += 9 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mTonemapHistogramResult.Get(), &SRVDesc, mTonemapHistogramResultSRVHandle);
	}
	//Histogram CB
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(TonemapUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mTonemapHistogramCB.GetAddressOf()));
		mTonemapHistogramCB->SetName(TEXT("mTonemapHistogramCB"));

		TonemapUniform Tonemap;
		Tonemap.InputWidth = 1920;
		Tonemap.InputHeight = 1080;
		Tonemap.MinLogL = -10.f;
		Tonemap.LogRange = 12.f;
		Tonemap.InverseLogRange = 1.f / Tonemap.LogRange;
		
		void* pData;
		mTonemapHistogramCB->Map(0, nullptr, &pData);
		memcpy(pData,&Tonemap,sizeof(Tonemap));
		mTonemapHistogramCB->Unmap(0, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mTonemapHistogramCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(TonemapUniform);
		
		mTonemapHistogramCBVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mTonemapHistogramCBVHandle.ptr += 10 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mTonemapHistogramCBVHandle);
	}
	//Luminance Average Buffer
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(float);
		ResourceDesc.Height = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, __uuidof(ID3D12Resource), (void**)mTonemapAvgLuminaceBuffer.GetAddressOf()));
		mTonemapAvgLuminaceBuffer->SetName(TEXT("mTonemapAvgLuminaceBuffer"));

		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		ZeroMemory(&UAVDesc, sizeof(UAVDesc));
		UAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.NumElements = 1;
		UAVDesc.Buffer.StructureByteStride = 0;
		UAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		mTonemapAvgLuminaceBufferUAVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mTonemapAvgLuminaceBufferUAVHandle.ptr += 11 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateUnorderedAccessView(mTonemapAvgLuminaceBuffer.Get(), NULL, &UAVDesc, mTonemapAvgLuminaceBufferUAVHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Buffer.FirstElement = 0;
		SRVDesc.Buffer.NumElements = 1;
		SRVDesc.Buffer.StructureByteStride = 0;
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		mTonemapAvgLuminaceBufferSRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mTonemapAvgLuminaceBufferSRVHandle.ptr += 12 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mTonemapAvgLuminaceBuffer.Get(), &SRVDesc, mTonemapAvgLuminaceBufferSRVHandle);
	}
	//VB
	{
		float Square[] = {
			-1.0f,1.0f,
			1.0f,1.0f,
			-1.f,-1.f,
			1.0f,-1.0f,
		};

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(Square);
		ResourceDesc.Height = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mTonemapVBUpload.GetAddressOf()));
		mTonemapVBUpload->SetName(TEXT("mTonemapVBUpload"));

		void* pData;
		mTonemapVBUpload->Map(0, nullptr, &pData);
		memcpy(pData, Square, sizeof(Square));
		mTonemapVBUpload->Unmap(0, 0);

		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)mTonemapVB.GetAddressOf()));
		mTonemapVB->SetName(TEXT("mTonemapVB"));

		mTonemapVBView.BufferLocation = mTonemapVB->GetGPUVirtualAddress();
		mTonemapVBView.SizeInBytes = sizeof(Square);
		mTonemapVBView.StrideInBytes = 2 * sizeof(float);

		CommandList->Reset(m_D3D12CmdAllocator.Get(), nullptr);

		CommandList->CopyResource(mTonemapVB.Get(), mTonemapVBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mTonemapVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		CommandList->ResourceBarrier(1, &ResourceBarrier);

		CommandList->Close();
		m_D3D12CmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)CommandList.GetAddressOf());
		WaitForPreviousFrame();
	}
}

void PBRShadingModel::InitPipelineStates()
{
	LoadGenEnviPipelineState();
	LoadSkyboxPipelineState();
	LoadTonemapPipelineState();
	LoadGenEnviAssets();
	LoadSkyboxAssets();
	LoadCommonAssets();
	LoadTonemapAssets();
}

void PBRShadingModel::GenEnvironmentMap()
{

	mGenEnviMapCmdList->Reset(m_D3D12CmdAllocator.Get(), mGenEnviMapPSO.Get());

	PIXBeginEvent(mGenEnviMapCmdList.Get(), 0, TEXT("GenEnvironmentMap"));

	mGenEnviMapCmdList->OMSetRenderTargets(1, &mGenEnviRTVHandle, FALSE, NULL);
	const FLOAT ClearColor[] = { 0,0,0, 0 };
	mGenEnviMapCmdList->ClearRenderTargetView(mGenEnviRTVHandle, ClearColor, 0, NULL);
	mGenEnviMapCmdList->IASetVertexBuffers(0, 1, &mGenEnviMapVBView);
	mGenEnviMapCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ID3D12DescriptorHeap* Heaps[] = {m_CBVSRVUAVDescHeap.Get()};
	mGenEnviMapCmdList->SetGraphicsRootSignature(mGenEnviMapRootSignature.Get());
	mGenEnviMapCmdList->SetDescriptorHeaps(_countof(Heaps), Heaps);
	mGenEnviMapCmdList->SetGraphicsRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
	mGenEnviMapCmdList->RSSetViewports(1, &mGenEnviViewport);
	mGenEnviMapCmdList->RSSetScissorRects(1, &mGenEnviScissorRect);

	mGenEnviMapCmdList->DrawInstanced(4, 1, 0, 0);

	for (UINT32 i = 0; i < 6; ++i)
	{
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.Subresource = i;
		ResourceBarrier.Transition.pResource = mEnvironmentMap.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		mGenEnviMapCmdList->ResourceBarrier(1, &ResourceBarrier);
	}
	
	PIXEndEvent(mGenEnviMapCmdList.Get());

	mGenEnviMapCmdList->Close();


	m_CommandLists.push_back(mGenEnviMapCmdList.Get());
}

void PBRShadingModel::DrawSkyBox()
{
	mSkyboxCommandList->Reset(m_D3D12CmdAllocator.Get(), mSkyboxPSO.Get());
	
	mSkyboxCommandList->OMSetRenderTargets(1, &mHDRRTVHandle, FALSE, nullptr);
	const FLOAT ClearColor[] = { 0,0,0, 0 };
	mSkyboxCommandList->ClearRenderTargetView(mHDRRTVHandle, ClearColor, 0, NULL);
	mSkyboxCommandList->IASetVertexBuffers(0, 1, &mSkyboxVBView);
	mSkyboxCommandList->IASetIndexBuffer(&mSkyboxIBView);
	mSkyboxCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ID3D12DescriptorHeap* Heaps[] = {m_CBVSRVUAVDescHeap.Get()};
	mSkyboxCommandList->SetGraphicsRootSignature(mSkyboxRootSignature.Get());
	mSkyboxCommandList->SetDescriptorHeaps(_countof(Heaps), Heaps);
	mSkyboxCommandList->SetGraphicsRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
	mSkyboxCommandList->RSSetViewports(1, &m_Viewport);
	mSkyboxCommandList->RSSetScissorRects(1, &m_ScissorRect);

	mSkyboxCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	
	mSkyboxCommandList->Close();

	m_CommandLists.push_back(mSkyboxCommandList.Get());
}

void PBRShadingModel::PostProcess()
{
	//tonemap
	{
		//histogram
		{
			mTonemapHistgramCommandList->Reset(m_D3D12ComputeCmdAllocator.Get(), mTonemapHistogramPSO.Get());
			mTonemapHistgramCommandList->SetComputeRootSignature(mTonemapHistogramRootSignature.Get());
			mTonemapHistgramCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
			mTonemapHistgramCommandList->SetComputeRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
			mTonemapHistgramCommandList->Dispatch(64, 48, 1);

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
			ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			ResourceBarrier.Transition.pResource = mTonemapHistogramResult.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			mTonemapHistgramCommandList->ResourceBarrier(1, &ResourceBarrier);

			mTonemapHistgramCommandList->Close();
			m_ComputeCommandList.push_back(mTonemapHistgramCommandList.Get());
		}

		//average
		{
			mTonemapAvgLuminaceCommandList->Reset(m_D3D12ComputeCmdAllocator.Get(), mTonemapAvgLuminacePSO.Get());
			mTonemapAvgLuminaceCommandList->SetComputeRootSignature(mTonemapAvgLuminanceRootSignature.Get());
			mTonemapAvgLuminaceCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
			mTonemapAvgLuminaceCommandList->SetComputeRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
			mTonemapAvgLuminaceCommandList->Dispatch(1, 1, 1);

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
			ResourceBarrier.Transition.pResource = mTonemapHistogramResult.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			mTonemapAvgLuminaceCommandList->ResourceBarrier(1, &ResourceBarrier);

			mTonemapAvgLuminaceCommandList->Close();
			m_ComputeCommandList.push_back(mTonemapAvgLuminaceCommandList.Get());
		}
		WaitForPreviousFrame();
		ExecuteComputeCommandList();
		//tonemap
		{
			mTonemapCommandList->Reset(m_D3D12CmdAllocator.Get(), mTonemapPSO.Get());

			D3D12_RESOURCE_BARRIER ResourceBarrier;
			ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
			ResourceBarrier.Transition.pResource = mTonemapAvgLuminaceBuffer.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			mTonemapCommandList->ResourceBarrier(1, &ResourceBarrier);

			ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			mTonemapCommandList->ResourceBarrier(1, &ResourceBarrier);

			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
			RTVHandle.ptr += m_FrameIndex * m_RTVDescriptorSize;
			mTonemapCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, NULL);
			FLOAT Color[4] = { 0 };
			mTonemapCommandList->ClearRenderTargetView(RTVHandle, Color, 0, NULL);

			mTonemapCommandList->SetGraphicsRootSignature(mTonemapRootSignature.Get());
			mTonemapCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
			mTonemapCommandList->SetGraphicsRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
			mTonemapCommandList->IASetVertexBuffers(0, 1, &mTonemapVBView);
			mTonemapCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			mTonemapCommandList->RSSetScissorRects(1, &m_ScissorRect);
			mTonemapCommandList->RSSetViewports(1, &m_Viewport);

			mTonemapCommandList->DrawInstanced(4, 1, 0, 1);

			ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			mTonemapCommandList->ResourceBarrier(1, &ResourceBarrier);

			ResourceBarrier.Transition.pResource = mTonemapAvgLuminaceBuffer.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			mTonemapCommandList->ResourceBarrier(1, &ResourceBarrier);

			ResourceBarrier.Transition.pResource = mHDRRT.Get();
			ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			mTonemapCommandList->ResourceBarrier(1, &ResourceBarrier);

			mTonemapCommandList->Close();
			m_CommandLists.push_back(mTonemapCommandList.Get());
		}
		WaitForComputeFrame();
		ExecuteDirectCommandList();
	}
}

void PBRShadingModelRealIBL::InitPipelineStates()
{
	PBRShadingModel::InitPipelineStates();
	LoadPrimitivePipelineState();
	LoadPrimitiveAssets();
	GenEnvironmentMap();
	WaitForPreviousFrame();
}

void PBRShadingModelRealIBL::Draw()
{
	DrawSkyBox();
	WaitForPreviousFrame();
	DrawPrimitives();
	ExecuteDirectCommandList();
	PostProcess();
}

void PBRShadingModelRealIBL::LoadPrimitivePipelineState()
{
	//
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].NumDescriptors = 3;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].OffsetInDescriptorsFromTableStart = 4;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].OffsetInDescriptorsFromTableStart = 0;

		D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTable;
		ZeroMemory(&DescriptorTable, sizeof(DescriptorTable));
		DescriptorTable.NumDescriptorRanges = 2;
		DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_ROOT_PARAMETER Parameters[2];
		ZeroMemory(&Parameters, sizeof(Parameters));
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].DescriptorTable = DescriptorTable;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_STATIC_SAMPLER_DESC CubeMapSamplerDesc;
		ZeroMemory(&CubeMapSamplerDesc, sizeof(CubeMapSamplerDesc));
		CubeMapSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		CubeMapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		CubeMapSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		CubeMapSamplerDesc.ShaderRegister = 0;
		CubeMapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.NumParameters = 1;
		RootSignDesc.pParameters = Parameters;
		RootSignDesc.NumStaticSamplers = 1;
		RootSignDesc.pStaticSamplers = &CubeMapSamplerDesc;
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc,D3D_ROOT_SIGNATURE_VERSION_1,RootSign.GetAddressOf(),Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mPrimitiveRootSignature.GetAddressOf()));

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;

		if (S_OK != D3DCompileFromFile(TEXT("RealTimeIBL.hlsl"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}

		if (S_OK != D3DCompileFromFile(TEXT("RealTimeIBL.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s", Error->GetBufferPointer());
			return;
		}

		D3D12_INPUT_ELEMENT_DESC InputDesc[] = 
		{ 
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"NORMAL",	0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineDesc;
		ZeroMemory(&PipelineDesc, sizeof(PipelineDesc));
		PipelineDesc.pRootSignature = mPrimitiveRootSignature.Get();
		PipelineDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		PipelineDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
		PipelineDesc.BlendState.IndependentBlendEnable = TRUE;
		PipelineDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		PipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		PipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		PipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		PipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		PipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		PipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		PipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		PipelineDesc.SampleMask = 0xffffffff;
		PipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		PipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		PipelineDesc.RasterizerState.FrontCounterClockwise = TRUE;
		PipelineDesc.DepthStencilState.DepthEnable = TRUE;
		PipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		PipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		PipelineDesc.InputLayout.pInputElementDescs = InputDesc;
		PipelineDesc.InputLayout.NumElements = _countof(InputDesc);
		PipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PipelineDesc.NumRenderTargets = 1;
		PipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		PipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		PipelineDesc.SampleDesc = { 1,0 };
		PipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&PipelineDesc, __uuidof(ID3D12PipelineState), (void**)mPrimitvePSO.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mPrimitvePSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mPrimitiveCommandList.GetAddressOf()));
		mPrimitiveCommandList->SetName(TEXT("mPrimitiveCommandList"));
		mPrimitiveCommandList->Close();
	}
}

void PBRShadingModelRealIBL::LoadPrimitiveAssets()
{
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)CommandList.GetAddressOf()));
	CommandList->Close();
	CommandList->Reset(m_D3D12CmdAllocator.Get(), nullptr);
	//Mesh
	mPrimitive.LoadFBX("./Primitives/Sphere.fbx");
	
	//VB
	ComPtr<ID3D12Resource> PrimitiveVBUpload;
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc,sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = mPrimitive.Vertices.size() * sizeof(MeshVertex);
		ResourceDesc.Height = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)PrimitiveVBUpload.GetAddressOf()));
		PrimitiveVBUpload->SetName(TEXT("PrimitiveVBUpload"));

		void* pData;
		PrimitiveVBUpload->Map(0, NULL,&pData);
		memcpy(pData, mPrimitive.Vertices.data(), mPrimitive.Vertices.size() * sizeof(MeshVertex));
		PrimitiveVBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveVB.GetAddressOf()));
		mPrimitiveVB->SetName(TEXT("mPrimitiveVB"));

		CommandList->CopyResource(mPrimitiveVB.Get(), PrimitiveVBUpload.Get());
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPrimitiveVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		CommandList->ResourceBarrier(1, &ResourceBarrier);

		mPrimitiveVBView.BufferLocation = mPrimitiveVB->GetGPUVirtualAddress();
		mPrimitiveVBView.StrideInBytes = sizeof(MeshVertex);
		mPrimitiveVBView.SizeInBytes = mPrimitive.Vertices.size() * sizeof(MeshVertex);

	}
	//IB
	ComPtr<ID3D12Resource> PrimitiveIBUpload;
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = mPrimitive.Indices.size() * sizeof(int);
		ResourceDesc.Height = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps,D3D12_HEAP_FLAG_NONE,&ResourceDesc,D3D12_RESOURCE_STATE_GENERIC_READ,NULL,__uuidof(ID3D12Resource),(void**)PrimitiveIBUpload.GetAddressOf()));
		PrimitiveIBUpload->SetName(TEXT("PrimitiveIBUpload"));

		void* pData;
		PrimitiveIBUpload->Map(0, NULL, &pData);
		memcpy(pData, mPrimitive.Indices.data(), mPrimitive.Indices.size() * sizeof(int));
		PrimitiveIBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveIB.GetAddressOf()));
		mPrimitiveIB->SetName(TEXT("mPrimitiveIB"));

		CommandList->CopyResource(mPrimitiveIB.Get(), PrimitiveIBUpload.Get());
		
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPrimitiveIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		CommandList->ResourceBarrier(1, &ResourceBarrier);

		mPrimitiveIBView.BufferLocation = mPrimitiveIB->GetGPUVirtualAddress();
		mPrimitiveIBView.Format = DXGI_FORMAT_R32_UINT;
		mPrimitiveIBView.SizeInBytes = mPrimitive.Indices.size() * sizeof(int);

	}
	//CB
	{
		//primitive
		{
			PrimitiveUniform Primitive;
			XMStoreFloat4x4(&Primitive.LocalToWorld, XMMatrixIdentity());
			D3D12_HEAP_PROPERTIES HeapProps;
			ZeroMemory(&HeapProps, sizeof(HeapProps));
			HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.Width = sizeof(Primitive);
			ResourceDesc.Height = 1;
			ResourceDesc.DepthOrArraySize = 1.0f;
			ResourceDesc.MipLevels = 1.0f;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mPrimitiveCB.GetAddressOf()));
			mPrimitiveCB->SetName(TEXT("mPrimitiveCB"));

			void* pData;
			mPrimitiveCB->Map(0, nullptr, &pData);
			memcpy(pData, &Primitive, sizeof(Primitive));
			mPrimitiveCB->Unmap(0, nullptr);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, 0);
			ConstViewDesc.BufferLocation = mPrimitiveCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(Primitive);
			mPrimitiveCBV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
			mPrimitiveCBV.ptr += 4 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveCBV);
		}
		//View
		{
			ViewUniform View;
			View.ViewOrigin = {60,0,0,0};
			XMFLOAT4 Target = { 0,0,0,0 };
			XMFLOAT4 UpVector = { 0,1,0,0 };
			XMMATRIX WorldToView = XMMatrixLookAtLH(XMLoadFloat4(&View.ViewOrigin), XMLoadFloat4(&Target), XMLoadFloat4(&UpVector));
			XMMATRIX ViewToClip = XMMatrixPerspectiveFovLH(90.f,1920.f/1080.f, 1.0f, 10000.f);
			XMStoreFloat4x4(&View.WorldToClip, XMMatrixMultiply(WorldToView, ViewToClip));

			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(HeapProperties));
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = sizeof(View);
			ResourceDesc.Height = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveViewCB.GetAddressOf()));
			mPrimitiveViewCB->SetName(TEXT("mPrimitiveViewCB"));

			void* pData;
			assert(S_OK == mPrimitiveViewCB->Map(0, NULL, &pData));
			memcpy(pData, &View, sizeof(View));
			mPrimitiveViewCB->Unmap(0, NULL);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, sizeof(ConstViewDesc));
			ConstViewDesc.BufferLocation = mPrimitiveViewCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(View);
			mPrimitiveViewCBV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
			mPrimitiveViewCBV.ptr += 5 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveViewCBV);
		}

		{
			PBRMaterialUniform Material;
			Material.BaseColor = { 1.0f,1.0f, 1.0f, 1.0f, };
			Material.SpecularColor = { 1.0f,1.0f, 1.0f, 1.0f, };
			Material.fRoughtness = 0.001f;

			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(HeapProperties));
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = sizeof(Material);
			ResourceDesc.Height = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveMaterialCB.GetAddressOf()));
			mPrimitiveMaterialCB->SetName(TEXT("mPrimitiveMaterialCB"));

			void* pData;
			mPrimitiveMaterialCB->Map(0, NULL, &pData);
			memcpy(pData, &Material, sizeof(Material));
			mPrimitiveMaterialCB->Unmap(0, NULL);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, sizeof(ConstViewDesc));
			ConstViewDesc.BufferLocation = mPrimitiveMaterialCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(Material);
			mPrimitiveMaterialCBV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
			mPrimitiveMaterialCBV.ptr += 6 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveMaterialCBV);
		}
	}
	//Lights
	{
		
	}

	{
		CommandList->Close();
		m_D3D12CmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)CommandList.GetAddressOf());
		WaitForPreviousFrame();
	}
}

void PBRShadingModelRealIBL::DrawPrimitives()
{
	mPrimitiveCommandList->Reset(m_D3D12CmdAllocator.Get(), mPrimitvePSO.Get());

	mPrimitiveCommandList->OMSetRenderTargets(1, &mHDRRTVHandle, FALSE, &mDepthStencialHandle);
	const FLOAT ClearColor[] = { 0,0,0, 0 };
	mPrimitiveCommandList->ClearDepthStencilView(mDepthStencialHandle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, NULL);

	mPrimitiveCommandList->IASetIndexBuffer(&mPrimitiveIBView);
	mPrimitiveCommandList->IASetVertexBuffers(0, 1, &mPrimitiveVBView);
	mPrimitiveCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D12DescriptorHeap* Heaps[] = { m_CBVSRVUAVDescHeap.Get() };
	mPrimitiveCommandList->SetDescriptorHeaps(_countof(Heaps), Heaps);
	mPrimitiveCommandList->SetGraphicsRootSignature(mPrimitiveRootSignature.Get());
	mPrimitiveCommandList->SetGraphicsRootDescriptorTable(0, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
	//mPrimitiveCommandList->SetGraphicsRootDescriptorTable(1, m_CBVSRVUAVDescHeap->GetGPUDescriptorHandleForHeapStart());
	mPrimitiveCommandList->RSSetScissorRects(1, &m_ScissorRect);
	mPrimitiveCommandList->RSSetViewports(1, &m_Viewport);

	mPrimitiveCommandList->DrawIndexedInstanced(mPrimitive.Indices.size(), 1, 0, 0, 0);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = mHDRRT.Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	mPrimitiveCommandList->ResourceBarrier(1, &ResourceBarrier);


	mPrimitiveCommandList->Close();

	m_CommandLists.push_back(mPrimitiveCommandList.Get());
}

void PBRShadingModelPrecomputeIBL::InitPipelineStates()
{
	PBRShadingModel::InitPipelineStates();

	GenEnvironmentMap();
	WaitForPreviousFrame();

	LoadGenPrefilterEnviPipelineState();
	LoadGenIntegratedBRDFPipelineState();
	LoadPrimitiveBRDFPipelineState();
	LoadGenPrefilterEnvironmentAssets();
	LoadGenIntegratedBRDFAssets();
	LoadPrimitiveAssets();

	GenPrefilterEnvironmentMap();
	GenIntegratedBRDF();
}

void PBRShadingModelPrecomputeIBL::Draw()
{
	DrawSkyBox();
	WaitForPreviousFrame();
	DrawPrimitives();
	ExecuteDirectCommandList();
	PostProcess();
}

void PBRShadingModelPrecomputeIBL::GenPrefilterEnvironmentMap()
{

	for (UINT32 i = 0; i < 8; ++i)
	{
		mGenPrefilterEnvironmentCmdList->Reset(m_D3D12CmdAllocator.Get(), mGenPrefilterEnvironmentMapPSO.Get());

		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		RTVHandle.ptr += (4 + i) * m_RTVDescriptorSize;
		FLOAT Color[] = { 0,0,0,0 };
		mGenPrefilterEnvironmentCmdList->ClearRenderTargetView(RTVHandle, Color, 0, NULL);
		mGenPrefilterEnvironmentCmdList->OMSetRenderTargets(1, &RTVHandle, FALSE, NULL);
		mGenPrefilterEnvironmentCmdList->IASetVertexBuffers(0, 1, &mGenPrefilterEnviromentVBView);
		mGenPrefilterEnvironmentCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mGenPrefilterEnvironmentCmdList->SetGraphicsRootSignature(mGenPrefilterEnvironmentMapRootSignature.Get());
		mGenPrefilterEnvironmentCmdList->SetDescriptorHeaps(1, mGenPrefilterEnvironmentDH.GetAddressOf());
		mGenPrefilterEnvironmentCmdList->SetGraphicsRootDescriptorTable(0, mGenPrefilterEnvironmentDH->GetGPUDescriptorHandleForHeapStart());
		float fRoughness =  0.1f + i * 0.1f;
		mGenPrefilterEnvironmentCmdList->SetGraphicsRoot32BitConstants(1,1, &fRoughness, 0);
		D3D12_VIEWPORT VP = {0.f,0.f ,float(512>>(i+1)) ,float(512 >> (i + 1)) ,0.f ,1.f };
		mGenPrefilterEnvironmentCmdList->RSSetViewports(1, &VP);
		D3D12_RECT Rect = {0,0,(512 >> (i + 1)),(512 >> (i + 1 ))};
		mGenPrefilterEnvironmentCmdList->RSSetScissorRects(1, &Rect);
		mGenPrefilterEnvironmentCmdList->DrawInstanced(4, 1, 0, 0);

		mGenPrefilterEnvironmentCmdList->Close();

		m_CommandLists.push_back(mGenPrefilterEnvironmentCmdList.Get());

		WaitForPreviousFrame();
		ExecuteDirectCommandList();
	}
	
}

void PBRShadingModelPrecomputeIBL::GenIntegratedBRDF()
{
	mGenIntegratedBRDFCmdList->Reset(m_D3D12CmdAllocator.Get(), mGenIntegratedBRDFPSO.Get());

	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
	RTVHandle.ptr += 12 * m_RTVDescriptorSize;
	FLOAT Color[] = { 0,0,0,0 };
	mGenIntegratedBRDFCmdList->ClearRenderTargetView(RTVHandle, Color, 0, NULL);
	mGenIntegratedBRDFCmdList->OMSetRenderTargets(1, &RTVHandle, FALSE, NULL);
	mGenIntegratedBRDFCmdList->IASetVertexBuffers(0, 1, &mGenIntegratedBRDFVBView);
	mGenIntegratedBRDFCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	mGenIntegratedBRDFCmdList->SetGraphicsRootSignature(mGenIntegratedBRDFRootSignature.Get());
	//mGenIntegratedBRDFCmdList->SetDescriptorHeaps(1, mGenIntegratedBRDFDH.GetAddressOf());
	//mGenIntegratedBRDFCmdList->SetGraphicsRootDescriptorTable(0, mGenIntegratedBRDFDH->GetGPUDescriptorHandleForHeapStart());
	D3D12_VIEWPORT VP = { 0.f,0.f ,512.f ,512.f ,0.f ,1.f };
	mGenIntegratedBRDFCmdList->RSSetViewports(1, &VP);
	D3D12_RECT Rect = { 0,0,512,512 };
	mGenIntegratedBRDFCmdList->RSSetScissorRects(1, &Rect);
	mGenIntegratedBRDFCmdList->DrawInstanced(4, 1, 0, 0);

	mGenIntegratedBRDFCmdList->Close();

	m_CommandLists.push_back(mGenIntegratedBRDFCmdList.Get());

	ExecuteDirectCommandList();
	WaitForPreviousFrame();
}

void PBRShadingModelPrecomputeIBL::DrawPrimitives()
{
	mPrimitiveCommandList->Reset(m_D3D12CmdAllocator.Get(), mPrimitivePSO.Get());
	FLOAT ClearColor[] = { 0,0,0,0 };
	//mPrimitiveCommandList->ClearRenderTargetView(mHDRRTVHandle, ClearColor, 0, NULL);
	mPrimitiveCommandList->ClearDepthStencilView(mDepthStencialHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	mPrimitiveCommandList->OMSetRenderTargets(1, &mHDRRTVHandle, FALSE, &mDepthStencialHandle);
	mPrimitiveCommandList->IASetVertexBuffers(0, 1, &mPrimitiveVBView);
	mPrimitiveCommandList->IASetIndexBuffer(&mPrimitiveIBView);
	mPrimitiveCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mPrimitiveCommandList->SetGraphicsRootSignature(mPrimitiveRootSignature.Get());
	mPrimitiveCommandList->SetDescriptorHeaps(1, mPrimitiveDH.GetAddressOf());
	mPrimitiveCommandList->SetGraphicsRootDescriptorTable(0, mPrimitiveDH->GetGPUDescriptorHandleForHeapStart());
	mPrimitiveCommandList->RSSetScissorRects(1, &m_ScissorRect);
	mPrimitiveCommandList->RSSetViewports(1, &m_Viewport);

	mPrimitiveCommandList->DrawIndexedInstanced(mPrimitive.Indices.size(), 1, 0, 0, 0);

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Transition.pResource = mHDRRT.Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	mPrimitiveCommandList->ResourceBarrier(1, &ResourceBarrier);

	mPrimitiveCommandList->Close();

	m_CommandLists.push_back(mPrimitiveCommandList.Get());
}

void PBRShadingModelPrecomputeIBL::LoadGenPrefilterEnviPipelineState()
{
	D3D12_DESCRIPTOR_RANGE Ranges[2];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	Ranges[0].BaseShaderRegister = 0;
	Ranges[0].NumDescriptors = 1;
	Ranges[0].OffsetInDescriptorsFromTableStart = 0;
	Ranges[1].RegisterSpace = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	Ranges[1].BaseShaderRegister = 0;
	Ranges[1].NumDescriptors = 1;
	Ranges[1].OffsetInDescriptorsFromTableStart = 1;

	D3D12_ROOT_PARAMETER RootParameters[2];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	RootParameters[0].DescriptorTable.pDescriptorRanges = Ranges;
	RootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(Ranges);
	RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParameters[1].Constants.Num32BitValues = 1;
	RootParameters[1].Constants.ShaderRegister = 2;

	D3D12_STATIC_SAMPLER_DESC StaticSampler;
	ZeroMemory(&StaticSampler,sizeof(StaticSampler));
	StaticSampler.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	StaticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	StaticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
	ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
	RootSignDesc.NumParameters = _countof(RootParameters);
	RootSignDesc.pParameters = RootParameters;
	RootSignDesc.NumStaticSamplers = 1;
	RootSignDesc.pStaticSamplers = &StaticSampler;
	RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> RootSign;
	ComPtr<ID3DBlob> Error;
	assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mGenPrefilterEnvironmentMapRootSignature.GetAddressOf()));

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> GS;
	ComPtr<ID3DBlob> PS;

	if (S_OK != D3DCompileFromFile(TEXT("./GenPrefilterEnvironment.hlsl"), NULL, &D3D12DemoInclude, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}
	if (S_OK != D3DCompileFromFile(TEXT("./GenPrefilterEnvironment.hlsl"), NULL, &D3D12DemoInclude, "GSMain", "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, GS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}
	if (S_OK != D3DCompileFromFile(TEXT("./GenPrefilterEnvironment.hlsl"), NULL, &D3D12DemoInclude, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}

	D3D12_INPUT_ELEMENT_DESC InputDesc[] = {
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
	ZeroMemory(&GPSDesc, sizeof(GPSDesc));
	GPSDesc.pRootSignature = mGenPrefilterEnvironmentMapRootSignature.Get();
	GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
	GPSDesc.GS = { GS->GetBufferPointer(),GS->GetBufferSize() };
	GPSDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
	GPSDesc.InputLayout.pInputElementDescs = InputDesc;
	GPSDesc.InputLayout.NumElements = _countof(InputDesc);
	GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	GPSDesc.RasterizerState.FrontCounterClockwise = FALSE;
	GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	GPSDesc.BlendState.IndependentBlendEnable = FALSE;
	GPSDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	GPSDesc.SampleMask = UINT_MAX;
	GPSDesc.NumRenderTargets = 1;
	GPSDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	GPSDesc.DepthStencilState.DepthEnable = FALSE;
	GPSDesc.DepthStencilState.StencilEnable = FALSE;
	GPSDesc.SampleDesc = { 1,0 };

	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mGenPrefilterEnvironmentMapPSO.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,m_D3D12CmdAllocator.Get(), mGenPrefilterEnvironmentMapPSO.Get(),__uuidof(ID3D12GraphicsCommandList),(void**)mGenPrefilterEnvironmentCmdList.GetAddressOf()));
	mGenPrefilterEnvironmentCmdList->Close();

	D3D12_DESCRIPTOR_HEAP_DESC DHDesc;
	ZeroMemory(&DHDesc, sizeof(DHDesc));
	DHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DHDesc.NumDescriptors = 2;
	DHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&DHDesc,__uuidof(ID3D12DescriptorHeap),(void**)mGenPrefilterEnvironmentDH.GetAddressOf()));
}

void PBRShadingModelPrecomputeIBL::LoadGenIntegratedBRDFPipelineState()
{
	D3D12_DESCRIPTOR_RANGE Ranges[2];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	Ranges[0].BaseShaderRegister = 0;
	Ranges[0].BaseShaderRegister = 0;

	D3D12_ROOT_PARAMETER RootParameters[1];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
	ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
	RootSignDesc.NumParameters = 0;
	RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ComPtr<ID3DBlob> RootSign;
	ComPtr<ID3DBlob> Error;
	assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mGenIntegratedBRDFRootSignature.GetAddressOf()));

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;
	if (S_OK != D3DCompileFromFile(TEXT("./GenIntegratedBRDF.hlsl"), NULL, &D3D12DemoInclude, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}
	if (S_OK != D3DCompileFromFile(TEXT("./GenIntegratedBRDF.hlsl"), NULL, &D3D12DemoInclude, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}

	D3D12_INPUT_ELEMENT_DESC InputDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
	ZeroMemory(&GPSDesc, sizeof(GPSDesc));
	GPSDesc.pRootSignature = mGenIntegratedBRDFRootSignature.Get();
	GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
	GPSDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
	GPSDesc.InputLayout.pInputElementDescs = InputDesc;
	GPSDesc.InputLayout.NumElements = _countof(InputDesc);
	GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	GPSDesc.RasterizerState.FrontCounterClockwise = FALSE;
	GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	GPSDesc.NumRenderTargets = 1;
	GPSDesc.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
	GPSDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	GPSDesc.DepthStencilState.DepthEnable = FALSE;
	GPSDesc.DepthStencilState.StencilEnable = FALSE;
	GPSDesc.SampleMask = UINT_MAX;
	GPSDesc.SampleDesc = { 1,0 };

	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mGenIntegratedBRDFPSO.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mGenIntegratedBRDFPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mGenIntegratedBRDFCmdList.GetAddressOf()));
	mGenIntegratedBRDFCmdList->Close();
}

void PBRShadingModelPrecomputeIBL::LoadPrimitiveBRDFPipelineState()
{
	D3D12_DESCRIPTOR_RANGE Ranges[2];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	Ranges[0].BaseShaderRegister = 0;
	Ranges[0].NumDescriptors = 3;
	Ranges[0].OffsetInDescriptorsFromTableStart = 0;
	Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	Ranges[1].BaseShaderRegister = 0;
	Ranges[1].NumDescriptors = 2;
	Ranges[1].OffsetInDescriptorsFromTableStart = 3;

	D3D12_ROOT_PARAMETER RootParameters[1];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	RootParameters[0].DescriptorTable.pDescriptorRanges = Ranges;
	RootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(Ranges);

	D3D12_STATIC_SAMPLER_DESC StaticSamplers[2];
	ZeroMemory(StaticSamplers, sizeof(StaticSamplers));
	StaticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	StaticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	StaticSamplers[0].ShaderRegister = 0;
	StaticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	StaticSamplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	StaticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	StaticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	StaticSamplers[1].ShaderRegister = 1;
	StaticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	ComPtr<ID3DBlob> RootSign;
	ComPtr<ID3DBlob> Error;
	D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
	ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
	RootSignDesc.pParameters = RootParameters;
	RootSignDesc.NumParameters = _countof(RootParameters);
	RootSignDesc.pStaticSamplers = StaticSamplers;
	RootSignDesc.NumStaticSamplers = _countof(StaticSamplers);
	RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc,D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(),Error.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mPrimitiveRootSignature.GetAddressOf()));

	D3D12_INPUT_ELEMENT_DESC InputDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL",	0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	ComPtr<ID3DBlob> VS;
	ComPtr<ID3DBlob> PS;

	if (S_OK != D3DCompileFromFile(TEXT("./PrecomputeIBL.hlsl"), NULL, &D3D12DemoInclude, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return; 
	}
	if (S_OK != D3DCompileFromFile(TEXT("./PrecomputeIBL.hlsl"), NULL, &D3D12DemoInclude, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, PS.GetAddressOf(), Error.GetAddressOf()))
	{
		LOGA("%s\n", Error->GetBufferPointer());
		return;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
	ZeroMemory(&GPSDesc, sizeof(GPSDesc));
	GPSDesc.pRootSignature = mPrimitiveRootSignature.Get();
	GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
	GPSDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
	GPSDesc.InputLayout.pInputElementDescs = InputDesc;
	GPSDesc.InputLayout.NumElements = _countof(InputDesc);

	GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
	GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	GPSDesc.NumRenderTargets = 1;
	GPSDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	GPSDesc.BlendState.IndependentBlendEnable = TRUE;
	GPSDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	GPSDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	GPSDesc.SampleMask = UINT_MAX;
	GPSDesc.SampleDesc = { 1,0 };
	GPSDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	GPSDesc.DepthStencilState.DepthEnable = TRUE;
	GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mPrimitivePSO.GetAddressOf()));
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mPrimitivePSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mPrimitiveCommandList.GetAddressOf()));
	mPrimitiveCommandList->Close();

	{
		D3D12_DESCRIPTOR_HEAP_DESC DHDesc;
		ZeroMemory(&DHDesc, sizeof(DHDesc));
		DHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		DHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DHDesc.NumDescriptors = 5;
		m_D3D12Device->CreateDescriptorHeap(&DHDesc, __uuidof(ID3D12DescriptorHeap),(void**)mPrimitiveDH.GetAddressOf());
	}
}

void PBRShadingModelPrecomputeIBL::LoadGenPrefilterEnvironmentAssets()
{
	mGenPrefilterEnvironmentCmdList->Reset(m_D3D12CmdAllocator.Get(), mGenPrefilterEnvironmentMapPSO.Get());
	{
		//0-1
		//2-3
		float TriangleStripQuad[] =
		{
			-1.0f,1.0f, -1.f,
			1.0f, 1.0f, -1.f,
			-1.0f,-1.0f, -1.f,
			1.0f,-1.0f, -1.f,
		};

		const UINT VertexBufferSize = sizeof(TriangleStripQuad);

		D3D12_HEAP_PROPERTIES HeapProperites;
		ZeroMemory(&HeapProperites, sizeof(HeapProperites));
		HeapProperites.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = VertexBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mGenPrefilterEnviromentVBUpload.GetAddressOf()));
		mGenPrefilterEnviromentVBUpload->SetName(TEXT("mGenPrefilterEnviromentVBUpload"));

		void* pData;
		mGenPrefilterEnviromentVBUpload->Map(0, 0, &pData);
		memcpy(pData, TriangleStripQuad, VertexBufferSize);
		mGenPrefilterEnviromentVBUpload->Unmap(0, 0);

		HeapProperites.Type = D3D12_HEAP_TYPE_DEFAULT;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)mGenPrefilterEnviromentVB.GetAddressOf()));
		mGenPrefilterEnviromentVB->SetName(TEXT("mGenPrefilterEnviromentVB"));

		mGenPrefilterEnviromentVBView.BufferLocation = mGenPrefilterEnviromentVB->GetGPUVirtualAddress();
		mGenPrefilterEnviromentVBView.SizeInBytes = VertexBufferSize;
		mGenPrefilterEnviromentVBView.StrideInBytes = 3 * sizeof(float);

		mGenPrefilterEnvironmentCmdList->CopyResource(mGenPrefilterEnviromentVB.Get(), mGenPrefilterEnviromentVBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mGenPrefilterEnviromentVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		mGenPrefilterEnvironmentCmdList->ResourceBarrier(1, &ResourceBarrier);
	}

	{
		struct GSView
		{
			XMFLOAT4X4 FaceTransform[6];
			XMFLOAT4X4 Projection;
			char padding[512-sizeof(XMFLOAT4X4)*7];
		};

		GSView View;
		XMStoreFloat4x4(&View.FaceTransform[0], XMMatrixRotationY(PI / 2.f));//+X
		XMStoreFloat4x4(&View.FaceTransform[1], XMMatrixRotationY(-PI / 2.f));//-X
		XMStoreFloat4x4(&View.FaceTransform[2], XMMatrixRotationX(PI / 2.f)* XMMatrixRotationY(PI));//+Y
		XMStoreFloat4x4(&View.FaceTransform[3], XMMatrixRotationX(-PI / 2.f)* XMMatrixRotationY(PI));//-Y
		XMStoreFloat4x4(&View.FaceTransform[4], XMMatrixRotationY(PI));//+Z
		XMStoreFloat4x4(&View.FaceTransform[5], XMMatrixIdentity());//-Z
		XMStoreFloat4x4(&View.Projection, XMMatrixIdentity());

		D3D12_HEAP_PROPERTIES HeapProperites;
		ZeroMemory(&HeapProperites, sizeof(HeapProperites));
		HeapProperites.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = sizeof(GSView);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mGenPrefilterEnvironmentCBView.GetAddressOf()));
		mGenPrefilterEnvironmentCBView->SetName(TEXT("mGenPrefilterEnvironmentCBView"));

		void* pData;
		mGenPrefilterEnvironmentCBView->Map(0, 0, &pData);
		memcpy(pData, &View, sizeof(GSView));
		mGenPrefilterEnvironmentCBView->Unmap(0, 0);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mGenPrefilterEnvironmentCBView->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(GSView);
		D3D12_CPU_DESCRIPTOR_HANDLE Handle = mGenPrefilterEnvironmentDH->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, Handle);
	}

	{
		D3D12_HEAP_PROPERTIES HeapProperites;
		ZeroMemory(&HeapProperites, sizeof(HeapProperites));
		HeapProperites.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		ResourceDesc.Width = 512;
		ResourceDesc.Height = 512;
		ResourceDesc.DepthOrArraySize = 6;
		ResourceDesc.MipLevels = 9;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		//https://stackoverflow.com/questions/24975473/directx11-texture2d-formats
		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(ClearValue));
		ClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), (void**)mPrefilterEnvironmentMap.GetAddressOf()));
		mPrefilterEnvironmentMap->SetName(TEXT("mPrefilterEnvironmentMap"));

		for (UINT32 i = 1; i < 9; ++i)
		{
			D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
			ZeroMemory(&RTVDesc, sizeof(RTVDesc));
			RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.MipSlice = i;
			RTVDesc.Texture2DArray.ArraySize = 6;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
			Handle.ptr += (3 + i) * m_RTVDescriptorSize;
			m_D3D12Device->CreateRenderTargetView(mPrefilterEnvironmentMap.Get(), &RTVDesc, Handle);
		}

		{
			D3D12_RESOURCE_BARRIER ResourceBarriers[2];
			ZeroMemory(ResourceBarriers, sizeof(ResourceBarriers));
			ResourceBarriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			ResourceBarriers[0].Transition.pResource = mEnvironmentMap.Get();
			ResourceBarriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			ResourceBarriers[1].Transition.pResource = mPrefilterEnvironmentMap.Get();

			for (UINT32 i = 0; i < 6; ++i)
			{
				ResourceBarriers[0].Transition.Subresource = i;
				ResourceBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				ResourceBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
				ResourceBarriers[1].Transition.Subresource = i * 9;
				ResourceBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				ResourceBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
				mGenPrefilterEnvironmentCmdList->ResourceBarrier(_countof(ResourceBarriers), ResourceBarriers);

				D3D12_TEXTURE_COPY_LOCATION DestLocation;
				ZeroMemory(&DestLocation, sizeof(DestLocation));
				DestLocation.pResource = mPrefilterEnvironmentMap.Get();
				DestLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				DestLocation.SubresourceIndex = i*9;
				D3D12_TEXTURE_COPY_LOCATION SrcLocation;
				ZeroMemory(&SrcLocation, sizeof(SrcLocation));
				SrcLocation.pResource = mEnvironmentMap.Get();
				SrcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				SrcLocation.SubresourceIndex = i;
				D3D12_BOX CopyRegion;
				ZeroMemory(&CopyRegion, sizeof(CopyRegion));
				CopyRegion.left = 0;
				CopyRegion.right = 512;
				CopyRegion.top = 0;
				CopyRegion.bottom = 512;
				CopyRegion.front = 0;
				CopyRegion.back = 1;
				mGenPrefilterEnvironmentCmdList->CopyTextureRegion(&DestLocation, 0, 0, 0, &SrcLocation, &CopyRegion);

				ResourceBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
				ResourceBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				ResourceBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				ResourceBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				mGenPrefilterEnvironmentCmdList->ResourceBarrier(_countof(ResourceBarriers), ResourceBarriers);
			}

		}
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.ResourceMinLODClamp = 0;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		D3D12_CPU_DESCRIPTOR_HANDLE Handle = mGenPrefilterEnvironmentDH->GetCPUDescriptorHandleForHeapStart();
		Handle.ptr += m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mPrefilterEnvironmentMap.Get(), &SRVDesc, Handle);
	}

	mGenPrefilterEnvironmentCmdList->Close();

	m_CommandLists.push_back(mGenPrefilterEnvironmentCmdList.Get());
	WaitForPreviousFrame();
	ExecuteDirectCommandList();
}

void PBRShadingModelPrecomputeIBL::LoadGenIntegratedBRDFAssets()
{
	mGenIntegratedBRDFCmdList->Reset(m_D3D12CmdAllocator.Get(), mGenIntegratedBRDFPSO.Get());

	{
		float Square[] = {
			-1.0f,1.0f,
			1.0f,1.0f,
			-1.f,-1.f,
			1.0f,-1.0f,
		};

		const UINT VertexBufferSize = sizeof(Square);

		D3D12_HEAP_PROPERTIES HeapProperites;
		ZeroMemory(&HeapProperites, sizeof(HeapProperites));
		HeapProperites.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = VertexBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mGenIntegratedBRDFVBUpload.GetAddressOf()));

		void* pData;
		mGenIntegratedBRDFVBUpload->Map(0, 0, &pData);
		memcpy(pData, Square, VertexBufferSize);
		mGenIntegratedBRDFVBUpload->Unmap(0, 0);

		HeapProperites.Type = D3D12_HEAP_TYPE_DEFAULT;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)mGenIntegratedBRDFVB.GetAddressOf()));

		mGenIntegratedBRDFCmdList->CopyResource(mGenIntegratedBRDFVB.Get(), mGenIntegratedBRDFVBUpload.Get());

		mGenIntegratedBRDFVBView.BufferLocation = mGenIntegratedBRDFVB->GetGPUVirtualAddress();
		mGenIntegratedBRDFVBView.SizeInBytes = VertexBufferSize;
		mGenIntegratedBRDFVBView.StrideInBytes = 2 * sizeof(float);

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mGenIntegratedBRDFVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		mGenIntegratedBRDFCmdList->ResourceBarrier(1, &ResourceBarrier);
	}

	{
		D3D12_HEAP_PROPERTIES HeapProperites;
		ZeroMemory(&HeapProperites, sizeof(HeapProperites));
		HeapProperites.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		ResourceDesc.Width = 512;
		ResourceDesc.Height = 512;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(ClearValue));
		ClearValue.Format = DXGI_FORMAT_R32G32_FLOAT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperites, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), (void**)mIntegratedBRDF.GetAddressOf()));
	}

	{
		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));
		RTVDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		Handle.ptr += (12) * m_RTVDescriptorSize;
		m_D3D12Device->CreateRenderTargetView(mIntegratedBRDF.Get(), &RTVDesc, Handle);
	}

	{
		mGenIntegratedBRDFCmdList->Close();
		m_CommandLists.push_back(mGenIntegratedBRDFCmdList.Get());

		WaitForPreviousFrame();
		ExecuteDirectCommandList();
	}
}

void PBRShadingModelPrecomputeIBL::LoadPrimitiveAssets()
{
	//Mesh
	mPrimitive.LoadFBX("./Primitives/Sphere.fbx");
	mPrimitiveCommandList->Reset(m_D3D12CmdAllocator.Get(), mPrimitivePSO.Get());
	//VB
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Width = mPrimitive.Vertices.size() * sizeof(MeshVertex);
		ResourceDesc.Height = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveVBUpload.GetAddressOf()));
		mPrimitiveVBUpload->SetName(TEXT("PrimitiveVBUpload"));

		void* pData;
		mPrimitiveVBUpload->Map(0, NULL, &pData);
		memcpy(pData, mPrimitive.Vertices.data(), mPrimitive.Vertices.size() * sizeof(MeshVertex));
		mPrimitiveVBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveVB.GetAddressOf()));
		mPrimitiveVB->SetName(TEXT("mPrimitiveVB"));

		mPrimitiveCommandList->CopyResource(mPrimitiveVB.Get(), mPrimitiveVBUpload.Get());
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPrimitiveVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		mPrimitiveCommandList->ResourceBarrier(1, &ResourceBarrier);

		mPrimitiveVBView.BufferLocation = mPrimitiveVB->GetGPUVirtualAddress();
		mPrimitiveVBView.StrideInBytes = sizeof(MeshVertex);
		mPrimitiveVBView.SizeInBytes = mPrimitive.Vertices.size() * sizeof(MeshVertex);

	}
	//IB
	{
		D3D12_HEAP_PROPERTIES HeapProps;
		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = mPrimitive.Indices.size() * sizeof(int);
		ResourceDesc.Height = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveIBUpload.GetAddressOf()));
		mPrimitiveIBUpload->SetName(TEXT("PrimitiveIBUpload"));

		void* pData;
		mPrimitiveIBUpload->Map(0, NULL, &pData);
		memcpy(pData, mPrimitive.Indices.data(), mPrimitive.Indices.size() * sizeof(int));
		mPrimitiveIBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProps, sizeof(HeapProps));
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveIB.GetAddressOf()));
		mPrimitiveIB->SetName(TEXT("mPrimitiveIB"));

		mPrimitiveCommandList->CopyResource(mPrimitiveIB.Get(), mPrimitiveIBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPrimitiveIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		mPrimitiveCommandList->ResourceBarrier(1, &ResourceBarrier);

		mPrimitiveIBView.BufferLocation = mPrimitiveIB->GetGPUVirtualAddress();
		mPrimitiveIBView.Format = DXGI_FORMAT_R32_UINT;
		mPrimitiveIBView.SizeInBytes = mPrimitive.Indices.size() * sizeof(int);

	}
	//CB
	{
		//primitive
		{
			PrimitiveUniform Primitive;
			XMStoreFloat4x4(&Primitive.LocalToWorld, XMMatrixIdentity());
			D3D12_HEAP_PROPERTIES HeapProps;
			ZeroMemory(&HeapProps, sizeof(HeapProps));
			HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.Width = sizeof(Primitive);
			ResourceDesc.Height = 1;
			ResourceDesc.DepthOrArraySize = 1.0f;
			ResourceDesc.MipLevels = 1.0f;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mPrimitiveCB.GetAddressOf()));
			mPrimitiveCB->SetName(TEXT("mPrimitiveCB"));

			void* pData;
			mPrimitiveCB->Map(0, nullptr, &pData);
			memcpy(pData, &Primitive, sizeof(Primitive));
			mPrimitiveCB->Unmap(0, nullptr);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, 0);
			ConstViewDesc.BufferLocation = mPrimitiveCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(Primitive);
			mPrimitiveCBV = mPrimitiveDH->GetCPUDescriptorHandleForHeapStart();
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveCBV);
		}
		//View
		{
			ViewUniform View;
			View.ViewOrigin = { 60,0,0,0 };
			XMFLOAT4 Target = { 0,0,0,0 };
			XMFLOAT4 UpVector = { 0,1,0,0 };
			XMMATRIX WorldToView = XMMatrixLookAtLH(XMLoadFloat4(&View.ViewOrigin), XMLoadFloat4(&Target), XMLoadFloat4(&UpVector));
			XMMATRIX ViewToClip = XMMatrixPerspectiveFovLH(90.f, 1920.f / 1080.f, 1.0f, 10000.f);
			XMStoreFloat4x4(&View.WorldToClip, XMMatrixMultiply(WorldToView, ViewToClip));

			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(HeapProperties));
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = sizeof(View);
			ResourceDesc.Height = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveViewCB.GetAddressOf()));
			mPrimitiveViewCB->SetName(TEXT("mPrimitiveViewCB"));

			void* pData;
			assert(S_OK == mPrimitiveViewCB->Map(0, NULL, &pData));
			memcpy(pData, &View, sizeof(View));
			mPrimitiveViewCB->Unmap(0, NULL);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, sizeof(ConstViewDesc));
			ConstViewDesc.BufferLocation = mPrimitiveViewCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(View);
			mPrimitiveViewCBV = mPrimitiveDH->GetCPUDescriptorHandleForHeapStart();
			mPrimitiveViewCBV.ptr += 1 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveViewCBV);
		}

		{
			PBRMaterialUniform Material;
			Material.BaseColor = { 1.0f,1.0f, 1.0f, 1.0f, };
			Material.SpecularColor = { 1.0f,1.0f, 1.0f, 1.0f, };
			Material.fRoughtness = 0.1f;

			D3D12_HEAP_PROPERTIES HeapProperties;
			ZeroMemory(&HeapProperties, sizeof(HeapProperties));
			HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC ResourceDesc;
			ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = sizeof(Material);
			ResourceDesc.Height = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.SampleDesc = { 1,0 };
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPrimitiveMaterialCB.GetAddressOf()));
			mPrimitiveMaterialCB->SetName(TEXT("mPrimitiveMaterialCB"));

			void* pData;
			mPrimitiveMaterialCB->Map(0, NULL, &pData);
			memcpy(pData, &Material, sizeof(Material));
			mPrimitiveMaterialCB->Unmap(0, NULL);

			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstViewDesc;
			ZeroMemory(&ConstViewDesc, sizeof(ConstViewDesc));
			ConstViewDesc.BufferLocation = mPrimitiveMaterialCB->GetGPUVirtualAddress();
			ConstViewDesc.SizeInBytes = sizeof(Material);
			mPrimitiveMaterialCBV = mPrimitiveDH->GetCPUDescriptorHandleForHeapStart();
			mPrimitiveMaterialCBV.ptr += 2 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&ConstViewDesc, mPrimitiveMaterialCBV);
		}

		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(SRVDesc));
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			SRVDesc.TextureCube.MipLevels = 9;
			SRVDesc.TextureCube.MostDetailedMip = 0;
			SRVDesc.TextureCube.ResourceMinLODClamp = 0;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = mPrimitiveDH->GetCPUDescriptorHandleForHeapStart();
			Handle.ptr += 3 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateShaderResourceView(mPrefilterEnvironmentMap.Get(), &SRVDesc, Handle);
		}

		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(SRVDesc));
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.ResourceMinLODClamp = 0;
			SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = mPrimitiveDH->GetCPUDescriptorHandleForHeapStart();
			Handle.ptr += 4 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateShaderResourceView(mIntegratedBRDF.Get(), &SRVDesc, Handle);
		}
	}

	{
		mPrimitiveCommandList->Close();
		m_CommandLists.push_back(mPrimitiveCommandList.Get());
		WaitForPreviousFrame();
		ExecuteDirectCommandList();
	}
}

