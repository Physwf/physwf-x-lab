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
}

void PCSSDemo::InitPipelineStates()
{
	LoadCommonAssets();

}

void PCSSDemo::Draw()
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
		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mPCSSRootSignature.Get();
		GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mPCSSPSO.GetAddressOf());
	}
}

void PCSSDemo::LoadScenePipelineState()
{

}

void PCSSDemo::LoadPCSSAssets()
{

}

void PCSSDemo::LoadSceneAssets()
{

}
