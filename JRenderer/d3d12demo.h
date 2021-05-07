#pragma once

#include <D3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <debugapi.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <cassert>
#include <vector>
#include <string>

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
	D3D12Demo(HWND hWnd) 
		: m_hWnd(hWnd) 
		, m_NumCBVSRVUAVDescriptors(0)
		, m_NumSamplerDescriptors(0)
		, m_NumRTVDescriptors(FrameCount)
		, m_NumDSVDescriptors(0)
	{}

	void Initialize();
	void Render();

	virtual void OnMouseMove(float fScreenX, float fScreenY) {};
	virtual void OnKeyDown(unsigned char KeyCode) {};
private:
	bool EnumAdapter();
protected:
	virtual void InitPipelineStates() = 0;
	virtual void Draw() = 0;
	void ExecuteDirectCommandList();
	void ExecuteComputeCommandList();
	void WaitForPreviousFrame();
	void WaitForComputeFrame();
protected:
	ComPtr<IDXGIFactory>				m_DXGIFactory;
	ComPtr<IDXGIAdapter>				m_DXGIAdapter;
	ComPtr<ID3D12Device>				m_D3D12Device;
	ComPtr<ID3D12CommandAllocator>		m_D3D12CmdAllocator;
	ComPtr<ID3D12CommandAllocator>		m_D3D12ComputeCmdAllocator;
	ComPtr<ID3D12CommandAllocator>		m_D3D12CopyCmdAllocator;
	ComPtr<ID3D12CommandQueue>			m_D3D12CmdQueue;
	ComPtr<ID3D12CommandQueue>			m_D3D12ComputeCmdQueue;
	ComPtr<IDXGISwapChain3>				m_DXGISwapChain;
	ComPtr<ID3D12Resource>				m_BackBuffer[FrameCount];
	DXGI_FORMAT							m_BackBufferFormat;


	ComPtr<ID3D12DescriptorHeap>		m_CBVSRVUAVDescHeap;
	ComPtr<ID3D12DescriptorHeap>		m_SamplerHeap;
	ComPtr<ID3D12DescriptorHeap>		m_RTVDescHeap;
	ComPtr<ID3D12DescriptorHeap>		m_DSVDescHeap;
	UINT								m_CBVSRVUAVDescriptorSize;
	UINT								m_SamplerDescriptorSize;
	UINT								m_RTVDescriptorSize;
	UINT								m_DSVDescriptorSize;

	UINT								m_NumCBVSRVUAVDescriptors;
	UINT								m_NumSamplerDescriptors;
	UINT								m_NumRTVDescriptors;
	UINT								m_NumDSVDescriptors;

	std::vector<ID3D12CommandList*>		m_ComputeCommandList;
	std::vector<ID3D12CommandList*>		m_CommandLists;

	UINT								m_FrameIndex;
	HANDLE								m_FenceEvent;
	ComPtr<ID3D12Fence>					m_Fence;
	UINT64								m_FenceValue;

	HWND m_hWnd;
};

void GetShaderParameterAllocations(ID3DBlob* Code);

class SimpleInclude : public ID3DInclude
{
public: 
	SimpleInclude(LPCSTR WorkingDir, LPCSTR SystemDir);

	HRESULT __stdcall Open(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
	HRESULT __stdcall Close(THIS_ LPCVOID pData);
private:
	std::string mWorkdingDir;
	std::string mSystemDir;
};

extern SimpleInclude D3D12DemoInclude;