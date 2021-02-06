#include "d3d12demo.h"
#include "DirectXTex.h"
#include "DirectXMath.h"

using namespace DirectX;

//Device requires SizeInBytes be a multiple of 256.
struct CubeMapViewUniform
{
	XMFLOAT4X4 FaceTransform[6];
	XMFLOAT4X4 Projection;
	float padding[16];
};

struct SkyBoxViewUniform
{
	XMFLOAT4X4 WorldToProj;
	float Pading00[48];
};

class SkyBoxDemo : public D3D12Demo
{
public:
	SkyBoxDemo(HWND hWnd):D3D12Demo(hWnd)
	{
		m_NumSamplerDescriptors = 1;
		m_NumRTVDescriptors = FrameCount + 1;
		m_NumCBVSRVUAVDescriptors = 2 + 2;
	}
protected:
	virtual void InitPipelineStates();
	virtual void Draw();
private:
	void InitCubemapPass();
	void UpdateCubeMap();
	void InitSceneColorPass();
private:
	//Cubemap Pass
	D3D12_VIEWPORT m_CubeMapViewport;
	D3D12_RECT m_CubeMapScissorRect;
	DXGI_FORMAT m_CubeMapRTFormat;
	ComPtr<ID3D12Resource> m_CubeMapRT;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CubeMapRTHandle;
	ComPtr<ID3D12RootSignature> m_CubeMapRootSignature;
	ComPtr<ID3D12PipelineState> m_CubeMapPipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_CubeMapCmdList;
	ComPtr<ID3D12Resource> m_QuadVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_QuadVertexBufferView;
	ComPtr<ID3D12Resource> m_HDRI;
	D3D12_CPU_DESCRIPTOR_HANDLE m_HDRISRVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_HDRISRVSamplerHandle;
	ComPtr<ID3D12Resource> m_CubeViewUniformBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CubeViewBufferHandle;
	//Scene Color Pass
	D3D12_VIEWPORT	m_Viewport;
	D3D12_RECT		m_ScissorRect;
	DXGI_FORMAT		m_RTFormat;
	ComPtr<ID3D12RootSignature> m_SkyBoxRootSignature;
	ComPtr<ID3D12PipelineState> m_SkyBoxPipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_SkyBoxCmdList;
	ComPtr<ID3D12Resource> m_SkyBoxVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_SkyBoxVertexBufferView;
	ComPtr<ID3D12Resource> m_SkyBoxIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_SkyBoxIndexBufferView;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SkyBoxSRVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SkyBoxSRVSamplerHandle;
	ComPtr<ID3D12Resource> m_SkyBoxViewUniformBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SkyBoxViewBufferHandle;
};