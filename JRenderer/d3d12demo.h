#pragma once

#include <D3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <debugapi.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <cassert>
#define FrameCount 2

#define LOG(format,...)									\
	do {												\
		wchar_t info[1024] = {0};						\
		wsprintf(info, format, __VA_ARGS__);			\
		OutputDebugString(info);						\
	} while (0);

#define LOGA(format,...)									\
	do {													\
		char info[1024] = {0};								\
		wsprintfA(info, format, __VA_ARGS__);					\
		OutputDebugStringA(info);							\
	} while (0);



using namespace Microsoft::WRL;

class D3D12Demo
{
public:
	D3D12Demo(HWND hWnd) : m_hWnd(hWnd) {}

	void Initialize();
	void Render();
private:
	bool EnumAdapter();
protected:
	virtual void InitPipelineStates() = 0;
	virtual void Draw() = 0;
	void WaitForPreviousFrame();
protected:
	ComPtr<IDXGIFactory>				m_DXGIFactory;
	ComPtr<IDXGIAdapter>				m_DXGIAdapter;
	ComPtr<ID3D12Device>				m_D3D12Device;
	ComPtr<ID3D12CommandAllocator>		m_D3D12CmdAllocator;
	ComPtr<ID3D12CommandQueue>			m_D3D12CmdQueue;
	ComPtr<IDXGISwapChain3>				m_DXGISwapChain;
	ComPtr<ID3D12Resource>				m_BackBuffer[FrameCount];

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	m_MSAALevels;

	ComPtr<ID3D12RootSignature>			m_D3D12RootSign;
	ComPtr<ID3D12DescriptorHeap>		m_D3D12DescHeap;
	ComPtr<ID3D12GraphicsCommandList>	m_D3D12CmdList;
	UINT								m_RTVDescriptorSize;

	ComPtr<ID3D12PipelineState>			m_D3D12PipelineState;

	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;

	ComPtr<ID3D12Resource>				m_D3D12VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;

	UINT								m_FrameIndex;
	HANDLE								m_FenceEvent;
	ComPtr<ID3D12Fence>					m_Fence;
	UINT64								m_FenceValue;

	HWND m_hWnd;
};