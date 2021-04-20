#include "ShadowDemo.h"
#include "DirectXTex.h"
using namespace DirectX;
#include "d3dx12.h"

void ShadowDemo::LoadCommonAssets()
{
	mMarry.LoadObj("./assets/mary/Marry.obj");

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

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mFloorVBUpload.GetAddressOf()));

		void* pData;
		mFloorVBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mPlane.Vertices.data(), mPlane.Vertices.size() * sizeof(MeshVertex));
		mFloorVBUpload->Unmap(0, NULL);

		ZeroMemory(&HeapProperties, sizeof(HeapProperties));
		HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, __uuidof(ID3D12Resource), (void**)mFloorVB.GetAddressOf()));

		CommandList->CopyResource(mFloorVB.Get(), mFloorVBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mFloorVB.Get();
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

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mFloorIBUpload.GetAddressOf()));

		void* pData;
		mFloorIBUpload->Map(0, nullptr, &pData);
		memcpy(pData, mPlane.Indices.data(), mPlane.Indices.size() * sizeof(int));
		mFloorIBUpload->Unmap(0, NULL);

		CommandList->CopyResource(mFloorIB.Get(), mFloorIBUpload.Get());

		D3D12_RESOURCE_BARRIER ResourceBarrier;
		ZeroMemory(&ResourceBarrier, sizeof(ResourceBarrier));
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Transition.pResource = mFloorIB.Get();
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		CommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC DHDesc;
		ZeroMemory(&DHDesc, sizeof(DHDesc));
		DHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		DHDesc.NumDescriptors = 5;
		DHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&DHDesc, __uuidof(ID3D12DescriptorHeap), (void**)mMarryShadowPassDH.GetAddressOf()));
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC DHDesc;
		ZeroMemory(&DHDesc, sizeof(DHDesc));
		DHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		DHDesc.NumDescriptors = 5;
		DHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&DHDesc, __uuidof(ID3D12DescriptorHeap), (void**)mFloorShadowPassDH.GetAddressOf()));
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

		D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryShadowPassDH->GetCPUDescriptorHandleForHeapStart();
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mMarryPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
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

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)mFloorPrimitiveCB.GetAddressOf()));

		D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorShadowPassDH->GetCPUDescriptorHandleForHeapStart();
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mFloorPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
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
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryShadowPassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorShadowPassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
		
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
	LoadFloorPipelineState();
	ShadowDemo::LoadCommonAssets();
	LoadMarryAssets();
	LoadFloorAssets();
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

	mFloorCommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, &mSceneDepthViewHandle);
	mFloorCommandList->SetDescriptorHeaps(1, m_CBVSRVUAVDescHeap.GetAddressOf());
	mFloorCommandList->IASetIndexBuffer(&mFloorIBView);
	mFloorCommandList->IASetVertexBuffers(0, 1, &mFloorVBView);
	mFloorCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mFloorCommandList->RSSetViewports(1, &Viewport);
	mFloorCommandList->RSSetScissorRects(1, &RTVRect);
	mFloorCommandList->DrawIndexedInstanced(mPlane.Indices.size(), 1, 0, 0, 0);

	mFloorCommandList->Close();

	m_CommandLists.push_back(mMarryCommandList.Get());
	m_CommandLists.push_back(mFloorCommandList.Get());
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

	mPCSSCommandList->IASetIndexBuffer(&mFloorIBView);
	mPCSSCommandList->IASetVertexBuffers(0, 1, &mFloorVBView);
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
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mPCSSRootSignature.Get();
		GPSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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
		GPSDesc.DepthStencilState.DepthEnable = TRUE;
		GPSDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		GPSDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		GPSDesc.SampleDesc = { 1,0 };
		GPSDesc.SampleMask = 0xffffffff;
		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mPCSSPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mPCSSPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mPCSSCommandList.GetAddressOf()));
		mPCSSCommandList->Close();
	}
}

void PCSSDemo::LoadMarryPipelineState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(&Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//VertexShader Primitive View
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 4;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 2;
		Ranges[1].OffsetInDescriptorsFromTableStart = 4;
		D3D12_ROOT_PARAMETER Parameters[1];
		ZeroMemory(Parameters, sizeof(Parameters));
		Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		Parameters[0].DescriptorTable.pDescriptorRanges = Ranges;
		Parameters[0].DescriptorTable.NumDescriptorRanges = _countof(Ranges);

		D3D12_STATIC_SAMPLER_DESC StaticSampler[2];
		ZeroMemory(&StaticSampler, sizeof(StaticSampler));
		StaticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		StaticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		StaticSampler[0].ShaderRegister = 0;
		StaticSampler[1].Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		StaticSampler[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		StaticSampler[1].ShaderRegister = 1;
		D3D12_ROOT_SIGNATURE_DESC RSDesc;
		ZeroMemory(&RSDesc, sizeof(RSDesc));
		RSDesc.NumParameters = _countof(Parameters);
		RSDesc.pParameters = Parameters;
		RSDesc.NumStaticSamplers = _countof(StaticSampler);
		RSDesc.pStaticSamplers = StaticSampler;
		RSDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RSDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));
		LOGA("%s\n", RootSign->GetBufferPointer());
		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mMarryRootSignature.GetAddressOf()));

		D3D12_INPUT_ELEMENT_DESC InuptDesc[] = 
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		if (S_OK != D3DCompileFromFile(TEXT("Marry.hlsl"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		if (S_OK != D3DCompileFromFile(TEXT("Marry.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		GetShaderParameterAllocations(VS.Get());
		GetShaderParameterAllocations(PS.Get());
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
		GPSDesc.SampleMask = 0xfffffff;

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mMarryPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mMarryPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mMarryCommandList.GetAddressOf()));
		mMarryCommandList->Close();
	}
}


void PCSSDemo::LoadFloorPipelineState()
{
	{
		D3D12_DESCRIPTOR_RANGE Ranges[2];
		ZeroMemory(Ranges, sizeof(Ranges));
		Ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Ranges[0].BaseShaderRegister = 0;
		Ranges[0].NumDescriptors = 4;
		Ranges[0].OffsetInDescriptorsFromTableStart = 0;
		Ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Ranges[1].BaseShaderRegister = 0;
		Ranges[1].NumDescriptors = 1;
		Ranges[1].OffsetInDescriptorsFromTableStart = 4;

		D3D12_ROOT_PARAMETER RootParameter;
		ZeroMemory(&RootParameter, sizeof(RootParameter));
		RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParameter.DescriptorTable.pDescriptorRanges = Ranges;
		RootParameter.DescriptorTable.NumDescriptorRanges = _countof(Ranges);

		D3D12_STATIC_SAMPLER_DESC StaticSampler[1];
		ZeroMemory(&StaticSampler, sizeof(StaticSampler));
		StaticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		StaticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		StaticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		StaticSampler[0].ShaderRegister = 0;

		D3D12_ROOT_SIGNATURE_DESC RootSignDesc;
		ZeroMemory(&RootSignDesc, sizeof(RootSignDesc));
		RootSignDesc.pParameters = &RootParameter;
		RootSignDesc.NumParameters = 1;
		RootSignDesc.NumStaticSamplers = _countof(StaticSampler);
		RootSignDesc.pStaticSamplers = StaticSampler;
		RootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> RootSign;
		ComPtr<ID3DBlob> Error;
		assert(S_OK == D3D12SerializeRootSignature(&RootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSign.GetAddressOf(), Error.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateRootSignature(0, RootSign->GetBufferPointer(), RootSign->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)mFloorRootSignature.GetAddressOf()));

		D3D12_INPUT_ELEMENT_DESC InputDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,40,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		};

		ComPtr<ID3DBlob> VS;
		ComPtr<ID3DBlob> PS;
		if (S_OK != D3DCompileFromFile(TEXT("Floor.hlsl"), NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, VS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}
		if (S_OK != D3DCompileFromFile(TEXT("Floor.hlsl"), NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG, 0, PS.GetAddressOf(), Error.GetAddressOf()))
		{
			LOGA("%s\n", Error->GetBufferPointer());
			return;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSDesc;
		ZeroMemory(&GPSDesc, sizeof(GPSDesc));
		GPSDesc.pRootSignature = mFloorRootSignature.Get();
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
		GPSDesc.SampleMask = UINT_MAX;

		assert(S_OK == m_D3D12Device->CreateGraphicsPipelineState(&GPSDesc, __uuidof(ID3D12PipelineState), (void**)mFloorPSO.GetAddressOf()));

		assert(S_OK == m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CmdAllocator.Get(), mFloorPSO.Get(), __uuidof(ID3D12GraphicsCommandList), (void**)mFloorCommandList.GetAddressOf()));
		mFloorCommandList->Close();
	}
}

void PCSSDemo::LoadCommonAssets()
{
	//marry primitive
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mMarryPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);

		D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryScenePassDH->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
	}
	//floor primitive
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		ZeroMemory(&CBVDesc, sizeof(CBVDesc));
		CBVDesc.BufferLocation = mFloorPrimitiveCB->GetGPUVirtualAddress();
		CBVDesc.SizeInBytes = sizeof(PrimitiveUniform);

		D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorScenePassDH->GetCPUDescriptorHandleForHeapStart();
		m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
	}
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

		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mSceneViewCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(ViewUniform);
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mSceneViewCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(ViewUniform);
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 1 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
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

		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mLightCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(LightUniform);
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
		
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mLightCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(LightUniform);
			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 2 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle = mMarryScenePassDH->GetCPUDescriptorHandleForHeapStart();
		SRVHandle.ptr += 4 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mPCSSDetph.Get(), &SRVDesc, SRVHandle);
	}
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle = mFloorScenePassDH->GetCPUDescriptorHandleForHeapStart();
		SRVHandle.ptr += 4 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mPCSSDetph.Get(), &SRVDesc, SRVHandle);
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

		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mMarryMaterialCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(ObjMaterialUniform);

			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mMarryScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 4 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}

		
		
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

		D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle = m_CBVSRVUAVDescHeap->GetCPUDescriptorHandleForHeapStart();
		SRVHandle.ptr += 5 * m_CBVSRVUAVDescriptorSize;
		m_D3D12Device->CreateShaderResourceView(mMarrayDiffuseColorSR.Get(), &SRVDesc, SRVHandle);
	}

	{
		ID3D12CommandList* List[] = { CommandList.Get() };
		m_D3D12CmdQueue->ExecuteCommandLists(1, List);
		WaitForPreviousFrame();
	}
}

void PCSSDemo::LoadFloorAssets()
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

		assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)mFloorMaterialCB.GetAddressOf()));

		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			ZeroMemory(&CBVDesc, sizeof(CBVDesc));
			CBVDesc.BufferLocation = mMarryMaterialCB->GetGPUVirtualAddress();
			CBVDesc.SizeInBytes = sizeof(ObjMaterialUniform);

			D3D12_CPU_DESCRIPTOR_HANDLE CBVHandle = mFloorScenePassDH->GetCPUDescriptorHandleForHeapStart();
			CBVHandle.ptr += 4 * m_CBVSRVUAVDescriptorSize;
			m_D3D12Device->CreateConstantBufferView(&CBVDesc, CBVHandle);
		}
	}
}
