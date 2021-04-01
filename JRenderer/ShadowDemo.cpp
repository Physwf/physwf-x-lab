#include "ShadowDemo.h"
#include "Primitives.h"

void ShadowDemo::LoadCommonAssets()
{
	Mesh M;
	M.LoadObj("Primitives/Marray.obj");

	ComPtr<ID3D12GraphicsCommandList> CommandList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_D3D12CmdAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)CommandList.GetAddressOf()));
	CommandList->Close();
	//VB
	{
		D3D12_HEAP_PROPERTIES HeapProperty;
		ZeroMemory(&HeapProperty, sizeof(HeapProperty));
		HeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = M.Vertices.size() * sizeof(MeshVertex);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperty, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, __uuidof(ID3D12Resource), (void**)mMarryVBUpload.GetAddressOf()));

		void* pData;
		mMarryVBUpload->Map(0, nullptr, &pData);
		memcpy(pData, M.Vertices.data(), M.Vertices.size() * sizeof(MeshVertex));
		mMarryVBUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProperty, sizeof(HeapProperty));
		HeapProperty.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperty, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryVB.GetAddressOf()));

		mMarryVBView.BufferLocation = mMarryVB->GetGPUVirtualAddress();
		mMarryVBView.StrideInBytes = sizeof(MeshVertex);
		mMarryVBView.SizeInBytes = M.Vertices.size() * sizeof(MeshVertex);

		CommandList->CopyResource(mMarryVB.Get(), mMarryVBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mMarryVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	//IB
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = M.Indices.size() * sizeof(int);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, __uuidof(ID3D12Resource), (void**)mMarryIB.GetAddressOf()));
		void* pData;
		mMarryIBUpload->Map(0, nullptr, &pData);
		memcpy(pData, M.Indices.data(), M.Indices.size() * sizeof(int));
		mMarryIBUpload->Unmap(0, nullptr);

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryIB.GetAddressOf()));
		
		mMarryIBView.BufferLocation = mMarryIB->GetGPUVirtualAddress();
		mMarryIBView.SizeInBytes = M.Indices.size() * sizeof(int);
		mMarryIBView.Format = DXGI_FORMAT_R32_UINT;

		CommandList->CopyResource(mMarryIB.Get(), mMarryIBUpload.Get());
		
		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mMarryIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	//Primitve Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(PrimitiveUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, __uuidof(ID3D12Resource), (void**)mMarryPrimitiveCB.GetAddressOf()));

		mMarryPrimitiveCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mMarryPrimitiveCBView.ptr += m_CBVSRVUAVDescriptorSize;
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mMarryPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mMarryPrimitiveCBView);
	}
	//Scene Depth
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Width = 1920;
		ResourceDesc.Height = 1080;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(ClearValue));
		ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ClearValue.DepthStencil.Depth = 1.0f;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void**)mSceneDepth.GetAddressOf()));
		
		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(DSVDesc));
		DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

		mSceneDepthViewHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateDepthStencilView(mSceneDepth.Get(), &DSVDesc, mSceneDepthViewHandle);
	}
}

void PCSSDemo::InitPipelineStates()
{
	LoadCommonAssets();
	LoadPCSSPipleState();
	LoadScenePipelineState();
	LoadPCSSAssets();
	LoadSceneAssets();
}

void PCSSDemo::Draw()
{

}

void PCSSDemo::DrawShadow()
{

}

void PCSSDemo::LoadPCSSPipleState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[1];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 2;

		D3D12_ROOT_PARAMETER Parameters[1];
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		Parameters[0].DescriptorTable.NumDescriptorRanges = 1;
		Parameters[0].DescriptorTable.pDescriptorRanges = Ranges;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.pParameters = Parameters;
		RootSignDesc.NumParameters = 1;

		ComPtr<ID3DBlob> Error;
		ComPtr<ID3DBlob> RootSign;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(),Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0,RootSign->GetBufferPointer(),RootSign->GetBufferSize(),__uuidof(ID3D12RootSignature), (void**)mPCSSRootSignature.GetAddressOf()));

		ComPtr<ID3DBlob> VS;
		if (S_OK != D3DCompileFromFile(TEXT("PCSS.hlsl"), nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
		}

		D3D12_INPUT_ELEMENT_DESC InputDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mPCSSRootSignature.Get();
		GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		GPSDesc.BlendState.AlphaToCoverageEnable = FALSE;
		GPSDesc.BlendState.IndependentBlendEnable = FALSE;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
		GPSDesc.InputLayout.NumElements = _countof(InputDesc);
		GPSDesc.InputLayout.pInputElementDescs = InputDesc;
		GPSDesc.NumRenderTargets = 0;
		GPSDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		GPSDesc.SampleDesc = { 1,0 };
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mPCSSPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mPCSSPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mPCSSCommandList.GetAddressOf()));
		mPCSSCommandList->Close();
	}
}

void PCSSDemo::LoadScenePipelineState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 2;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].OffsetInDescriptorsFromTableStart = 0;
		D3D12_ROOT_PARAMETER Parameters[1];
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		Parameters[0].DescriptorTable.pDescriptorRanges = Ranges;
		Parameters[0].DescriptorTable.NumDescriptorRanges = 2;

		D3D12_STATIC_SAMPLER_DESC StaticSampler;
		ZeroMemory(&StaticSampler, sizeof(StaticSampler));
		StaticSampler.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		StaticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		//StaticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		StaticSampler.ShaderRegister = 0;
		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.NumParameters = 1;
		RootSignDesc.pParameters = Parameters;
		RootSignDesc.NumStaticSamplers = 1;
		RootSignDesc.pStaticSamplers = &StaticSampler;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mMarrayPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mMarrayPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mMarrayCommandList.GetAddressOf()));
		mMarrayCommandList->Close();
	}
}

void PCSSDemo::LoadPCSSAssets()
{
	//Shadow Depth
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Width = 2048;
		ResourceDesc.Height = 2048;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
		ResourceDesc.SampleDesc = { 1,0 };

		D3D12_CLEAR_VALUE ClearValue;
		ZeroMemory(&ClearValue, sizeof(ClearValue));
		ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		ClearValue.DepthStencil.Depth = 1.0f;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,&ClearValue,__uuidof(ID3D12Resource),(void**)mPCSSDetph.GetAddressOf()));

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(DSVDesc));
		DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		DSVDesc.Texture2D.MipSlice = 0;

		mPCSSDetphViewHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mPCSSDetphViewHandle.ptr += m_DSVDescriptorSize;
		m_D3D12Device->CreateDepthStencilView(mPCSSDetph.Get(), &DSVDesc, mPCSSDetphViewHandle);

		mPCSSDetphSRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mPCSSDetphSRVHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		m_D3D12Device->CreateShaderResourceView(mPCSSDetph.Get(), &SRVDesc, mPCSSDetphSRVHandle);
	}
	//Shadow View
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ViewUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr, __uuidof(ID3D12Resource), (void**)mPCSSViewCB.GetAddressOf()));

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mPCSSViewCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(ViewUniform);
		mPCSSViewCBViewHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mPCSSViewCBViewHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mPCSSViewCBViewHandle);
	}
}

void PCSSDemo::LoadSceneAssets()
{
	//View Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ViewUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr, __uuidof(ID3D12Resource), (void**)mSceneViewCB.GetAddressOf()));

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mSceneViewCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(ViewUniform);
		mSceneViewCBHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mSceneViewCBHandle.ptr += 3 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mSceneViewCBHandle);

	}
	//Material Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(MaterialUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryMaterialCB.GetAddressOf()));

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mMarryMaterialCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(MaterialUniform);
		
		mMarryMaterialCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mMarryMaterialCBView.ptr += 4 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mMarryMaterialCBView);
	}
	//diffuse map
	{

	}
}
