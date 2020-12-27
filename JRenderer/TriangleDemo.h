#pragma once

#include "d3d12demo.h"

class TriangleDemo : public D3D12Demo
{
public:
	TriangleDemo(HWND hWnd) 
		: D3D12Demo(hWnd)
	{
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;

	void InitTriangleResources();
protected:
	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;
	DXGI_FORMAT		m_RTFormat;
	UINT m_SampleCount;
	UINT m_SampleQuality;
	ComPtr<ID3D12RootSignature>			m_TriangleRootSign;
	ComPtr<ID3D12PipelineState>			m_TrianglePipelineState;
	ComPtr<ID3D12GraphicsCommandList>	m_TriangleCmdList;
	ComPtr<ID3D12Resource>				m_TriangleVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_TriangleVertexBufferView;
};

class TriangleDemoMSAA : public TriangleDemo
{
public:
	TriangleDemoMSAA(HWND hWnd) 
		: TriangleDemo(hWnd) 
	{
		m_NumRTVDescriptors = FrameCount + 1;
	}
protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
	void InitSceneColorRT();
protected:
	ComPtr<ID3D12Resource> m_SceneColorRTMSAA;
private:
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	m_MSAALevels;
};

class TriangleDemoMSAA2 : public TriangleDemoMSAA
{
public:
	TriangleDemoMSAA2(HWND hWnd)
		: TriangleDemoMSAA(hWnd)
		
	{
		m_NumCBVSRVUAVDescriptors = 2;
	}

protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;

	void InitQuadResources();
private:
	ComPtr<ID3D12Resource>				m_QuadConstants;
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	m_MSAALevels;
	ComPtr<ID3D12RootSignature>			m_QuadRootSign;
	ComPtr<ID3D12PipelineState>			m_QuadPipelineState;
	ComPtr<ID3D12GraphicsCommandList>	m_QuadCmdList;
	ComPtr<ID3D12Resource>				m_QuadVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_QuadVertexBufferView;
};