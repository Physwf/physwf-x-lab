#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

struct JMeshLODResource
{
	ComPtr<ID3D12Resource> IndexBuffer;
	ComPtr<ID3D12Resource> VertexBuffer;
};

class JMesh
{

};