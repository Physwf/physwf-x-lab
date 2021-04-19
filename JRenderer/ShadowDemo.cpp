#include "ShadowDemo.h"
#include "DirectXTex.h"
using namespace DirectX;
#include "d3dx12.h"

void ShadowDemo::LoadCommonAssets()
{
	mMarry.LoadObj("./assets/mary/Marray.obj");

	ComPtr<ID3D12GraphicsCommandList> CommandList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)CommandList.GetAddressOf()));
	CommandList->Close();
	//VB
	{
		D3D12_HEAP_PROPERTIES HeapProperty;
		ZeroMemory(&HeapProperty, sizeof(HeapProperty));
		HeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = mMarry.Vertices.size() * sizeof(MeshVertex);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperty, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, __uuidof(ID3D12Resource), (void**)mMarryVBUpload.GetAddressOf()));

		void* pData;
		mMarryVBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mMarry.Vertices.data(), mMarry.Vertices.size() * sizeof(MeshVertex));
		mMarryVBUpload->Unmap(0, nullptr);

		ZeroMemory(&HeapProperty, sizeof(HeapProperty));
		HeapProperty.Type = D3D12_HEAP_TYPE_DEFAULT;
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperty, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryVB.GetAddressOf()));

		mMarryVBView.BufferLocation = mMarryVB->GetGPUVirtualAddress();
		mMarryVBView.StrideInBytes = sizeof(MeshVertex);
		mMarryVBView.SizeInBytes = mMarry.Vertices.size() * sizeof(MeshVertex);

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
		ResourceDesc.Width = mMarry.Indices.size() * sizeof(int);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, __uuidof(ID3D12Resource), (void**)mMarryIB.GetAddressOf()));
		void* pData;
		mMarryIBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mMarry.Indices.data(), mMarry.Indices.size() * sizeof(int));
		mMarryIBUpload->Unmap(0, nullptr);

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryIB.GetAddressOf()));
		
		mMarryIBView.BufferLocation = mMarryIB->GetGPUVirtualAddress();
		mMarryIBView.SizeInBytes = mMarry.Indices.size() * sizeof(int);
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
	//Plane VB
	mPlane.LoadObj("../assets/floor/floor.obj");
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = mPlane.Vertices.size() * sizeof(MeshVertex);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mPlaneVBUpload.GetAddressOf()));

		void* pData;
		mPlaneVBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mPlane.Vertices.data(), mPlane.Vertices.size() * sizeof(MeshVertex));
		mPlaneVBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)mPlaneVB.GetAddressOf()));

		CommandList->CopyResource(mPlaneVB.Get(), mPlaneVBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPlaneVB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	//Plane IB
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = mPlane.Indices.size() * sizeof(int);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mPlaneIBUpload.GetAddressOf()));

		void* pData;
		mPlaneIBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mPlane.Indices.data(), mPlane.Indices.size() * sizeof(int));
		mPlaneIBUpload->Unmap(0, NULL);

		CommandList->CopyResource(mPlaneIB.Get(), mPlaneIBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mPlaneIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	//marry Primitive Uniform
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

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mMarryPrimitiveCB.GetAddressOf()));

		mMarryPrimitiveCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mMarryPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mMarryPrimitiveCBView);
	}
	//plane primitive uniform
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
		ResourceDesc.SampleDesc = { 1,0 };
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mPlanePrimitiveCB.GetAddressOf()));

		mPlanePrimitiveCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mPlanePrimitiveCBView.ptr += m_CBVSRVUAVDescriptorSize;
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mPlanePrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mPlanePrimitiveCBView);
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
		mPCSSViewCBViewHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mPCSSViewCBViewHandle);
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
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, __uuidof(ID3D12Resource), (void**)mPCSSDetph.GetAddressOf()));

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
		mPCSSDetphSRVHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		m_D3D12Device->CreateShaderResourceView(mPCSSDetph.Get(), &SRVDesc, mPCSSDetphSRVHandle);
	}
	

	{
		ID3D12CommandList* List[] = { CommandList.Get() };
		m_D3D12CmdQueue->ExecuteCommandLists(1, List);
		WaitForPreviousFrame();
	}
}

void PCSSDemo::InitPipelineStates()
{
	LoadPCSSPipleState();
	LoadMarryPipelineState();
	LoadPlanePipelineState();
	ShadowDemo::LoadCommonAssets();
	LoadMarryAssets();
	LoadPlaneAssets();
	LoadCommonAssets();
}

void PCSSDemo::Draw()
{
	DrawShadow();

	mMarryCommandList->Reset(m_D3D12CmdAllocator.Get(), mMarryPSO.Get());

	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_DSVDescHeap->GetCPUDescriptorHandleForHeapStart();
	RTVHandle.ptr += m_FrameIndex * m_DSVDescriptorSize;

	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	ResourceBarrier.Transition.pResource = m_BackBuffer[m_FrameIndex].Get();
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mMarryCommandList->ResourceBarrier(1,&ResourceBarrier);

	const FLOAT ClearColor[] = { 0.f,0.f,0.f,0.f };
	D3D12_RECT RTVRect = { 0,0,1920,1080 };
	mMarryCommandList->ClearRenderTargetView(RTVHandle, ClearColor, 1, &RTVRect);
	mMarryCommandList->ClearDepthStencilView(mSceneDepthViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &RTVRect);

	mMarryCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, &mSceneDepthViewHandle);
	mMarryCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	mMarryCommandList->IASetIndexBuffer(&mMarryIBView);
	mMarryCommandList->IASetVertexBuffers(0, 1, &mMarryVBView);
	mMarryCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VIEWPORT Viewport = { 0.f,0.f,1920.f,1080.f,0.f,1.0f };
	mMarryCommandList->RSSetViewports(1, &Viewport);
	mMarryCommandList->RSSetScissorRects(1, &RTVRect);
	mMarryCommandList->DrawIndexedInstanced(mMarry.Indices.size(), 1, 0, 0, 0);

	mMarryCommandList->Close();

	mPlaneCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, &mSceneDepthViewHandle);
	mPlaneCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	mPlaneCommandList->IASetIndexBuffer(&mPlaneIBView);
	mPlaneCommandList->IASetVertexBuffers(0, 1, &mPlaneVBView);
	mPlaneCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mPlaneCommandList->RSSetViewports(1, &Viewport);
	mPlaneCommandList->RSSetScissorRects(1, &RTVRect);
	mPlaneCommandList->DrawIndexedInstanced(mPlane.Indices.size(), 1, 0, 0, 0);

	mPlaneCommandList->Close();

	m_CommandLists.push_back(mMarryCommandList.Get());
	m_CommandLists.push_back(mPlaneCommandList.Get());
}

void PCSSDemo::DrawShadow()
{
	mPCSSCommandList->Reset(m_D3D12CmdAllocator.Get(), mPCSSPSO.Get());
	mPCSSCommandList->OMSetRenderTargets(0, NULL, FALSE, &mPCSSDetphViewHandle);
	D3D12_RECT ClearRect = { 0, 0, 1920, 1080};
	mPCSSCommandList->ClearDepthStencilView(mPCSSDetphViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &ClearRect);
	mPCSSCommandList->SetPipelineState(mPCSSPSO.Get());
	D3D12_RECT RTRect = { 0,0,1024,1024 };
	D3D12_VIEWPORT RTViewport = { 0,0,1024,1024,0.f,1.0f };
	mPCSSCommandList->RSSetScissorRects(1, &RTRect);
	mPCSSCommandList->RSSetViewports(1, &RTViewport);
	mPCSSCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());

	mPCSSCommandList->IASetIndexBuffer(&mMarryIBView);
	mPCSSCommandList->IASetVertexBuffers(0, 1, &mMarryVBView);
	mPCSSCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mPCSSCommandList->DrawIndexedInstanced(mMarry.Indices.size(), 1, 0, 0, 0);

	mPCSSCommandList->IASetIndexBuffer(&mPlaneIBView);
	mPCSSCommandList->IASetVertexBuffers(0, 1, &mPlaneVBView);
	mPCSSCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mPCSSCommandList->DrawIndexedInstanced(mPlane.Indices.size(), 1, 0, 0, 0);

	mPCSSCommandList->Close();

	m_CommandLists.push_back(mPCSSCommandList.Get());

}

void PCSSDemo::LoadPCSSPipleState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[1];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 2;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;

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

void PCSSDemo::LoadMarryPipelineState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 4;//Primitive View Material Light
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
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mMarryPSO.GetAddressOf()));

		D3D12_INPUT_ELEMENT_DESC InuptDesc[] = 
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"UV",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		if (S_OK != D3DCompileFromFile(TEXT("Marry"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		if (S_OK != D3DCompileFromFile(TEXT("Marry"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mMarryRootSignature.Get();
		GPSDesc.InputLayout.pInputElementDescs = InuptDesc;
		GPSDesc.InputLayout.NumElements = _countof(InuptDesc);
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		GPSDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		GPSDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		GPSDesc.BlendState.IndependentBlendEnable = TRUE;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		GPSDesc.DepthStencilState.DepthEnable = TRUE;
		GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		GPSDesc.DepthStencilState.StencilEnable = FALSE;
		GPSDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mMarryPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mMarryPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mMarryCommandList.GetAddressOf()));
		mMarryCommandList->Close();
	}
}


void PCSSDemo::LoadPlanePipelineState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[1];
		ZeroMemory(Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 4;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;

		D3D12_ROOT_PARAMETER RootParameter;
		ZeroMemory(&RootParameter, sizeof(RootParameter));
		RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParameter.DescriptorTable.pDescriptorRanges = Ranges;
		RootParameter.DescriptorTable.NumDescriptorRanges = _countof(Ranges);

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.pParameters = &RootParameter;
		RootSignDesc.NumParameters = 1;
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mPlaneRootSignature.GetAddressOf()));

		D3D12_INPUT_ELEMENT_DESC InputDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"UV",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		if (S_OK != D3DCompileFromFile(TEXT("Marry.hlsl"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		if (S_OK != D3DCompileFromFile(TEXT("Marry.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mPlaneRootSignature.Get();
		GPSDesc.InputLayout.pInputElementDescs = InputDesc;
		GPSDesc.InputLayout.NumElements = _countof(InputDesc);
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		GPSDesc.VS = { VS->GetBufferPointer(),VS->GetBufferSize() };
		GPSDesc.PS = { PS->GetBufferPointer(),PS->GetBufferSize() };
		GPSDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		GPSDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		GPSDesc.RasterizerState.FrontCounterClockwise = TRUE;
		GPSDesc.NumRenderTargets = 1;
		GPSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		GPSDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		GPSDesc.BlendState.IndependentBlendEnable = TRUE;
		GPSDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		GPSDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		GPSDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		GPSDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		GPSDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		GPSDesc.DepthStencilState.DepthEnable = TRUE;
		GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		GPSDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mPlanePSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mPlanePSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mPlaneCommandList.GetAddressOf()));
		mPlaneCommandList->Close();
	}
}

void PCSSDemo::LoadCommonAssets()
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
		
		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mSceneViewCB.GetAddressOf()));

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mSceneViewCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(ViewUniform);
		mSceneViewCBHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mSceneViewCBHandle.ptr += 3 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mSceneViewCBHandle);

	}
	//Light Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(LightUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mLightCB.GetAddressOf()));

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mLightCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(LightUniform);
		mLightCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mLightCBView.ptr += 4 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mLightCBView);
	}
}

void PCSSDemo::LoadMarryAssets()
{
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)CommandList.GetAddressOf()));

	//Material Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ObjMaterialUniform);
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
		CBVDesc.SizeInBytes = sizeof(ObjMaterialUniform);

		mMarryMaterialCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mMarryMaterialCBView.ptr += 5 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mMarryMaterialCBView);
	}
	//diffuse map
	{
		TexMetadata MetaData;
		ScratchImage Scrach;
		LoadFromWICFile(TEXT("./marry/Marry.png"), WIC_FLAGS_NONE, &MetaData,Scrach);

		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = MetaData.format;
		ResourceDesc.Width = MetaData.width;
		ResourceDesc.Height = MetaData.height;
		ResourceDesc.DepthOrArraySize = (UINT16)MetaData.arraySize;
		ResourceDesc.MipLevels = (UINT16)MetaData.mipLevels;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)mMarrayDiffuseColorSR.GetAddressOf()));

		std::vector<D3D12_SUBRESOURCE_DATA> SubResources;
		PrepareUpload(m_D3D12Device.Get(), Scrach.GetImages(), Scrach.GetImageCount(), MetaData, SubResources);
		const UINT64 UploadBufferSize = GetRequiredIntermediateSize(mMarrayDiffuseColorSR.Get(), 0, SubResources.size());

		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Width = UploadBufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mMarrayDiffuseColorSRUpload.GetAddressOf()));

		UpdateSubresources(CommandList.Get(), mMarrayDiffuseColorSR.Get(), mMarrayDiffuseColorSRUpload.Get(), 0, 0, SubResources.size(), SubResources.data());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mMarrayDiffuseColorSR.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		CommandList->ResourceBarrier(1, &ResourceBarrier);

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = MetaData.format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = MetaData.mipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		mMarrayDiffuseColorSRV = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mMarrayDiffuseColorSRV.ptr += 6 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mMarrayDiffuseColorSR.Get(), &SRVDesc, mMarrayDiffuseColorSRV);
	}

	{
		ID3D12CommandList* List[] = { CommandList.Get() };
		m_D3D12CmdQueue->ExecuteCommandLists(1, List);
		WaitForPreviousFrame();
	}
}

void PCSSDemo::LoadPlaneAssets()
{
	//Material Uniform
	{
		D3D12_HEAP_PROPERTIES HeapProperties;
		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC ResourceDesc;
		ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = sizeof(ObjMaterialUniform);
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc = { 1,0 };

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mPlaneMaterialCB.GetAddressOf()));

		mPlaneMaterialCBView = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		mPlaneMaterialCBView.ptr += 7 * m_CBVSRVUAVDescriptorSize;
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, mPlaneMaterialCBView);
	}
}
