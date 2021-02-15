#pragma once

#include <D3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <debugapi.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
using namespace Microsoft::WRL;

class JD3D12RHI
{
public:
	void CreateVertexBuffer(void* Data, UINT Size, UINT Stride, ID3D12Resource* OutTriangleVertexBuffer, D3D12_VERTEX_BUFFER_VIEW* OutTriangleVertexBufferView);
	void CreateIndexBuffer(void* Data, UINT Size, BOOL Use16Bit, ID3D12Resource* OutTriangleVertexBuffer, D3D12_INDEX_BUFFER_VIEW* OutTriangleIndexBufferView);
	void CreateConstantBuffer(void* Data, UINT Size, ID3D12Resource* OutConstantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE ConstantBufferHandle);
	void CreateRenderTarget2D(UINT Width, UINT Height, UINT ArraySize, UINT MipLevels, UINT SampleCount, UINT SampleQuality, DXGI_FORMAT Format, ID3D12Resource* OutTexture, FLOAT ClearColor[]);
	void CreateRenderTargetView2D(ID3D12Resource* RTVTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT MipSlice, UINT PlaneSlice);
	void CreateRenderTargetView2DMS(ID3D12Resource* RTVTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle);
	void CreateRenderTargetView2DArray(ID3D12Resource* RTVTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT MipSlice = 0, UINT FirstArraySlice = 0, UINT PlaneSlice = 0);
	void CreateRenderTargetView2DArrayMS(ID3D12Resource* RTVTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetHandle, UINT FirstArraySlice = 0);
	void CreateDepthStencialTarget(UINT Width, UINT Height, UINT ArraySize, UINT MipLevels, UINT SampleCount, UINT SampleQuality, DXGI_FORMAT Format, ID3D12Resource* OutTexture, FLOAT ClearDepth, UINT8 ClearStencil, BOOL bWriteEnable);
	void CreateDepthStencialView(ID3D12Resource* OutTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle, UINT MipSlice = 0);
	void CreateDepthStencialViewMS(ID3D12Resource* OutTexture, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle);
	void CreateDepthStencialViewArray(ID3D12Resource* OutTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle, UINT MipSlice = 0, UINT FirstArraySlice = 0);
	void CreateDepthStencialViewArrayMS(ID3D12Resource* OutTexture, UINT ArraySize, DXGI_FORMAT Format, D3D12_CPU_DESCRIPTOR_HANDLE DSTHandle, UINT FirstArraySlice = 0);
private:
	ComPtr<IDXGIFactory>				m_DXGIFactory;
	ComPtr<IDXGIAdapter>				m_DXGIAdapter;
	ComPtr<ID3D12Device>				m_D3D12Device;
	ComPtr<ID3D12CommandAllocator>		m_D3D12CmdAllocator;
	ComPtr<ID3D12CommandQueue>			m_D3D12CmdQueue;
	ComPtr<IDXGISwapChain3>				m_DXGISwapChain;
	ComPtr<ID3D12Resource>				m_BackBuffer[FrameCount];
	DXGI_FORMAT							m_BackBufferFormat;


	ComPtr<ID3D12DescriptorHeap>		m_CBVSRVUAVDescHeap;
	ComPtr<ID3D12DescriptorHeap>		m_SamplerHeap;
	ComPtr<ID3D12DescriptorHeap>		m_RTVDescHeap;
	ComPtr<ID3D12DescriptorHeap>		m_DSVDescHeap;
};