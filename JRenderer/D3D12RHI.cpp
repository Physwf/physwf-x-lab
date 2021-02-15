#include "D3D12RHI.h"
#include <cassert>

void JD3D12RHI::CreateVertexBuffer(void* Data, UINT Size, UINT Stride, ID3D12Resource* OutVertexBuffer, D3D12_VERTEX_BUFFER_VIEW* OutVertexBufferView)
{
	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = Size;
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
		(void**)&OutVertexBuffer));

	UINT8* pVertexDataBegin;
	D3D12_RANGE readRange;
	readRange.Begin = readRange.End = 0;
	OutVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, Data, Size);
	OutVertexBuffer->Unmap(0, nullptr);

	OutVertexBufferView->BufferLocation = OutVertexBuffer->GetGPUVirtualAddress();
	OutVertexBufferView->StrideInBytes = Stride;
	OutVertexBufferView->SizeInBytes = Size;
}

void JD3D12RHI::CreateIndexBuffer(void* Data, UINT Size, BOOL Use16Bit, ID3D12Resource* OutIndexBuffer, D3D12_INDEX_BUFFER_VIEW* OutIndexBufferView)
{
	D3D12_HEAP_PROPERTIES HeapProps;
	ZeroMemory(&HeapProps, sizeof(HeapProps));
	HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProps.CreationNodeMask = 1;
	HeapProps.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = Size;
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
		(void**)&OutIndexBuffer
	));

	UINT8* pVertexDataBegin;
	D3D12_RANGE ReadRange;
	ReadRange.Begin = ReadRange.End = 0;
	OutIndexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, Data, Size);
	OutIndexBuffer->Unmap(0, nullptr);

	OutIndexBufferView->BufferLocation = OutIndexBuffer->GetGPUVirtualAddress();
	OutIndexBufferView->Format = DXGI_FORMAT_R16_UINT;
	OutIndexBufferView->SizeInBytes = Size;
}

void JD3D12RHI::CreateConstantBuffer(void* Data,UINT Size, ID3D12Resource* OutConstantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferHandle)
{
	D3D12_HEAP_PROPERTIES HeapProperties;
	ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC ResourceDesc;
	ZeroMemory(&ResourceDesc, sizeof(ResourceDesc));
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = Size;
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
		(void**)&OutConstantBuffer));

	UINT8* pDataBegin;
	D3D12_RANGE ReadRange;
	ReadRange.Begin = ReadRange.End = 0;
	OutConstantBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pDataBegin));
	memcpy(pDataBegin, Data, Size);
	OutConstantBuffer->Unmap(0, nullptr);

	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.BufferLocation = OutConstantBuffer->GetGPUVirtualAddress();
	Desc.SizeInBytes = Size;
	m_D3D12Device->CreateConstantBufferView(&Desc, ConstantBufferHandle);
}

void JD3D12RHI::CreateRenderTarget2D(UINT Width, UINT Height, UINT ArraySize, UINT MipLevels, UINT SampleCount, UINT SampleQuality, DXGI_FORMAT Format, ID3D12Resource* OutTexture, FLOAT ClearColor[])
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
	ResourceDesc.Width = Width;
	ResourceDesc.Height = Height;
	ResourceDesc.DepthOrArraySize = ArraySize;
	ResourceDesc.MipLevels = MipLevels;
	ResourceDesc.Format = Format;
	ResourceDesc.SampleDesc.Count = SampleCount;
	ResourceDesc.SampleDesc.Quality = SampleQuality;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE ClearValue;
	ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
	memcpy(&ClearValue.Color, ClearColor, sizeof(ClearColor));
	ClearValue.Format = Format;
	assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearValue, __uuidof(ID3D12Resource), (void**)&OutTexture));
}

void JD3D12RHI::CreateRenderTargetView2D(ID3D12Resource* RTVTexture, DXGI_FORMAT Format,D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT MipSlice, UINT PlaneSlice)
{
	D3D12_RENDER_TARGET_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2D.MipSlice = MipSlice;
	Desc.Texture2D.PlaneSlice = PlaneSlice;
	Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	m_D3D12Device->CreateRenderTargetView(RTVTexture, &Desc, RenderTargetHandle);
}

void JD3D12RHI::CreateRenderTargetView2DMS(ID3D12Resource* RTVTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle)
{
	D3D12_RENDER_TARGET_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	m_D3D12Device->CreateRenderTargetView(RTVTexture, &Desc, RenderTargetHandle);
}

void JD3D12RHI::CreateRenderTargetView2DArray(ID3D12Resource* RTVTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT MipSlice /*= 0*/, UINT FirstArraySlice /*= 0*/, UINT PlaneSlice /*= 0*/)
{
	D3D12_RENDER_TARGET_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2DArray.MipSlice = MipSlice;
	Desc.Texture2DArray.FirstArraySlice = FirstArraySlice;
	Desc.Texture2DArray.ArraySize = ArraySize;
	Desc.Texture2DArray.PlaneSlice = PlaneSlice;
	Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	m_D3D12Device->CreateRenderTargetView(RTVTexture, &Desc, RenderTargetHandle);
}

void JD3D12RHI::CreateRenderTargetView2DArrayMS(ID3D12Resource* RTVTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT FirstArraySlice /*= 0*/)
{
	D3D12_RENDER_TARGET_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2DMSArray.FirstArraySlice = FirstArraySlice;
	Desc.Texture2DMSArray.ArraySize = ArraySize;
	Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
	m_D3D12Device->CreateRenderTargetView(RTVTexture, &Desc, RenderTargetHandle);
}

void JD3D12RHI::CreateDepthStencialTarget(UINT Width, UINT Height, UINT ArraySize, UINT MipLevels, UINT SampleCount, UINT SampleQuality, DXGI_FORMAT Format, ID3D12Resource* OutTexture, FLOAT ClearDepth, UINT8 ClearStencil, BOOL bWriteEnable)
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
	ResourceDesc.Width = Width;
	ResourceDesc.Height = Height;
	ResourceDesc.DepthOrArraySize = ArraySize;
	ResourceDesc.MipLevels = MipLevels;
	ResourceDesc.Format = Format;
	ResourceDesc.SampleDesc.Count = SampleCount;
	ResourceDesc.SampleDesc.Quality = SampleQuality;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE ClearValue;
	ZeroMemory(&ClearValue, sizeof(D3D12_CLEAR_VALUE));
	ClearValue.DepthStencil.Depth = ClearDepth;
	ClearValue.DepthStencil.Stencil = ClearStencil;
	ClearValue.Format = Format;

	D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATE_DEPTH_READ;
	if (bWriteEnable)
	{
		InitialResourceState |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}
	assert(S_OK == m_D3D12Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, InitialResourceState, &ClearValue, __uuidof(ID3D12Resource), (void**)&OutTexture));
}

void JD3D12RHI::CreateDepthStencialView(ID3D12Resource* OutTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle, UINT MipSlice /*= 0*/)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2D.MipSlice = MipSlice;
	Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	m_D3D12Device->CreateDepthStencilView(OutTexture, &Desc, DSTHandle);
}

void JD3D12RHI::CreateDepthStencialViewMS(ID3D12Resource* OutTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	m_D3D12Device->CreateDepthStencilView(OutTexture, &Desc, DSTHandle);
}

void JD3D12RHI::CreateDepthStencialViewArray(ID3D12Resource* OutTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle, UINT MipSlice /*= 0*/, UINT FirstArraySlice /*= 0*/)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2DArray.MipSlice = MipSlice;
	Desc.Texture2DArray.FirstArraySlice = FirstArraySlice;
	Desc.Texture2DArray.ArraySize = ArraySize;
	Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	m_D3D12Device->CreateDepthStencilView(OutTexture, &Desc, DSTHandle);
}

void JD3D12RHI::CreateDepthStencialViewArrayMS(ID3D12Resource* OutTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle,  UINT FirstArraySlice /*= 0*/)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = Format;
	Desc.Texture2DMSArray.FirstArraySlice = FirstArraySlice;
	Desc.Texture2DMSArray.ArraySize = ArraySize;
	Desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
	m_D3D12Device->CreateDepthStencilView(OutTexture, &Desc, DSTHandle);
}

