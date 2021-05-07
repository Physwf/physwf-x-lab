#include "d3d12demo.h"
#include <fstream>

void D3D12Demo::Initialize()
{
	if (EnumAdapter())
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}

		assert(S_OK == D3D12CreateDevice(m_DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), &m_D3D12Device));

		D3D12_COMMAND_QUEUE_DESC CmdQueueDesc;
		ZeroMemory(&CmdQueueDesc, sizeof(CmdQueueDesc));
		CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		assert(S_OK == m_D3D12Device->CreateCommandQueue(&CmdQueueDesc, __uuidof(ID3D12CommandQueue), &m_D3D12CmdQueue));
		CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		assert(S_OK == m_D3D12Device->CreateCommandQueue(&CmdQueueDesc, __uuidof(ID3D12CommandQueue), &m_D3D12ComputeCmdQueue));

		m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
		SwapChainDesc.BufferDesc.Width = 1920;
		SwapChainDesc.BufferDesc.Height = 1080;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		SwapChainDesc.BufferDesc.Format = m_BackBufferFormat;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = FrameCount;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.Flags = 0;
		ComPtr<IDXGISwapChain> SwapChain;
		assert(S_OK == m_DXGIFactory->CreateSwapChain(m_D3D12CmdQueue.Get(), &SwapChainDesc, &SwapChain));
		SwapChain.As(&m_DXGISwapChain);

		m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

		

		if(m_NumCBVSRVUAVDescriptors >0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
			ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			rtvHeapDesc.NumDescriptors = m_NumCBVSRVUAVDescriptors;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//Only shader visible descriptor heaps can be bound to command lists
			assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_CBVSRVUAVDescHeap));
		}
		if (m_NumSamplerDescriptors > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
			ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			rtvHeapDesc.NumDescriptors = m_NumSamplerDescriptors;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_SamplerHeap));
		}
		if (m_NumRTVDescriptors > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
			ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.NumDescriptors = m_NumRTVDescriptors;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_RTVDescHeap));
		}
		if (m_NumDSVDescriptors > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
			ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			rtvHeapDesc.NumDescriptors = m_NumRTVDescriptors;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			assert(S_OK == m_D3D12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_DSVDescHeap));
		}

		m_CBVSRVUAVDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		m_RTVDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DSVDescriptorSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT n = 0; n < FrameCount; ++n)
		{
			assert(S_OK == m_DXGISwapChain->GetBuffer(n, __uuidof(ID3D12Resource), &m_BackBuffer[n]));
			m_D3D12Device->CreateRenderTargetView(m_BackBuffer[n].Get(), nullptr, RTVHandle);
			//offset
			RTVHandle.ptr += m_RTVDescriptorSize;
		}
		assert(S_OK == m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), &m_D3D12CmdAllocator));
		assert(S_OK == m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, __uuidof(ID3D12CommandAllocator), &m_D3D12ComputeCmdAllocator));
		assert(S_OK == m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator), &m_D3D12CopyCmdAllocator));

		{
			m_D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), &m_Fence);
			m_FenceValue = 1;

			m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_FenceEvent == nullptr)
			{
				
			}

			WaitForPreviousFrame();
		}

		InitPipelineStates();
	}
}

void D3D12Demo::Render()
{
	Draw();
	
	m_DXGISwapChain->Present(1, 0);

	WaitForPreviousFrame();
}

bool D3D12Demo::EnumAdapter()
{
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory4), &m_DXGIFactory);
	if (FAILED(hr))
	{
		return false;
	}
	UINT iIndex = 0;
	while (true)
	{
		ComPtr<IDXGIAdapter> Adpater;
		hr = m_DXGIFactory->EnumAdapters(iIndex, Adpater.GetAddressOf());
		if (hr == DXGI_ERROR_NOT_FOUND) break;

		if (iIndex == 0)
		{
			m_DXGIAdapter = Adpater;
		}
		DXGI_ADAPTER_DESC Desc;
		Adpater->GetDesc(&Desc);
		LOG(TEXT("Adpater %d:\n  \tDescription:%s\n  \tVendorId:%u\n  \tDeviceId:%u\n  \tSubSysId:%u\n  \tRevision:%u\n  \tDedicatedVideoMemory:%u\n  \tDedicatedSystemMemory:%u\n \tSharedSystemMemory:%u\n"), 
			iIndex,
			Desc.Description, 
			Desc.VendorId,
			Desc.DeviceId,
			Desc.SubSysId,
			Desc.Revision,
			Desc.DedicatedVideoMemory, 
			Desc.DedicatedSystemMemory, 
			Desc.SharedSystemMemory);
		++iIndex;
	}
	return true;
}


void D3D12Demo::ExecuteDirectCommandList()
{
	m_D3D12CmdQueue->ExecuteCommandLists(m_CommandLists.size(), &m_CommandLists[0]);
	m_CommandLists.clear();
}


void D3D12Demo::ExecuteComputeCommandList()
{
	if (m_ComputeCommandList.size() > 0)
	{
		m_D3D12ComputeCmdQueue->ExecuteCommandLists(m_ComputeCommandList.size(), &m_ComputeCommandList[0]);
		m_ComputeCommandList.clear();
	}
}

void D3D12Demo::WaitForPreviousFrame()
{
	const UINT64 fence = m_FenceValue;
	m_D3D12CmdQueue->Signal(m_Fence.Get(), fence);
	m_FenceValue++;

	if (m_Fence->GetCompletedValue() < fence)
	{
		m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();
}

void D3D12Demo::WaitForComputeFrame()
{
	const UINT64 fence = m_FenceValue;
	m_D3D12ComputeCmdQueue->Signal(m_Fence.Get(), fence);
	m_FenceValue++;

	if (m_Fence->GetCompletedValue() < fence)
	{
		m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();
}

void GetShaderParameterAllocations(ID3DBlob* Code)
{
	ID3D12ShaderReflection* pReflector = NULL;
	D3DReflect(Code->GetBufferPointer(), Code->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
	D3D12_SHADER_DESC ShaderDesc;
	pReflector->GetDesc(&ShaderDesc);
	for (UINT ResourceIndex = 0; ResourceIndex < ShaderDesc.BoundResources; ResourceIndex++)
	{
		D3D12_SHADER_INPUT_BIND_DESC BindDesc;
		pReflector->GetResourceBindingDesc(ResourceIndex, &BindDesc);
		if (BindDesc.Type == D3D10_SIT_CBUFFER || BindDesc.Type == D3D10_SIT_TBUFFER)
		{
			const UINT CBIndex = BindDesc.BindPoint;
			ID3D12ShaderReflectionConstantBuffer* ConstantBuffer = pReflector->GetConstantBufferByName(BindDesc.Name);
			D3D12_SHADER_BUFFER_DESC CBDesc;
			ConstantBuffer->GetDesc(&CBDesc);
			bool bGlobalCB = (strcmp(CBDesc.Name, "$Globals") == 0);
			if (bGlobalCB)
			{
				for (UINT ContantIndex = 0; ContantIndex < CBDesc.Variables; ContantIndex++)
				{
					ID3D12ShaderReflectionVariable* Variable = ConstantBuffer->GetVariableByIndex(ContantIndex);
					D3D12_SHADER_VARIABLE_DESC VariableDesc;
					Variable->GetDesc(&VariableDesc);
					if (VariableDesc.uFlags & D3D10_SVF_USED)
					{
						//OutParams.insert(std::make_pair<std::string, ParameterAllocation>(VariableDesc.Name, { CBIndex ,VariableDesc.StartOffset,VariableDesc.Size }));
					}
				}
			}
			else
			{
				//OutParams.insert(std::make_pair<std::string, ParameterAllocation>(CBDesc.Name, { CBIndex,0,0 }));
			}
		}
		else if (BindDesc.Type == D3D10_SIT_TEXTURE || BindDesc.Type == D3D10_SIT_SAMPLER)
		{
			UINT BindCount = BindDesc.BindCount;

			//OutParams.insert(std::make_pair<std::string, ParameterAllocation>(BindDesc.Name, { 0,BindDesc.BindPoint,BindCount }));
		}
		else if (BindDesc.Type == D3D11_SIT_UAV_RWTYPED || BindDesc.Type == D3D11_SIT_UAV_RWSTRUCTURED ||
			BindDesc.Type == D3D11_SIT_UAV_RWBYTEADDRESS || BindDesc.Type == D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER ||
			BindDesc.Type == D3D11_SIT_UAV_APPEND_STRUCTURED)
		{
		}
	}
}

SimpleInclude::SimpleInclude(LPCSTR WorkingDir, LPCSTR SystemDir)
{
	mWorkdingDir = WorkingDir;
	mSystemDir = SystemDir;
}

HRESULT __stdcall SimpleInclude::Open(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
	std::string FilePath;

	switch (IncludeType)
	{
	case D3D_INCLUDE_LOCAL:
	{
		FilePath = mWorkdingDir + "\\" + pFileName;
		break;
	}
	case D3D_INCLUDE_SYSTEM:
	{
		FilePath = mSystemDir + "\\" + pFileName;
		break;
	}
	}
	std::ifstream IncludeFile(FilePath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	if (IncludeFile.is_open()) 
	{
		long long fileSize = IncludeFile.tellg();
		char* buf = new char[fileSize];
		IncludeFile.seekg(0, std::ios::beg);
		IncludeFile.read(buf, fileSize);
		IncludeFile.close();
		*ppData = buf;
		*pBytes = fileSize;
	}
	else 
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT __stdcall SimpleInclude::Close(THIS_ LPCVOID pData)
{
	char* buf = (char*)pData;
	delete[] buf;
	return S_OK;
}

SimpleInclude D3D12DemoInclude("", "");