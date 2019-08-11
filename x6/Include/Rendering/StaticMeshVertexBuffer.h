#pragma once

#include "RenderResource.h"
#include "StaticMeshVertexData.h"

class XStaticMeshVertexBuffer : public XRenderResource
{
public:

	XStaticMeshVertexBuffer();

	virtual ~XStaticMeshVertexBuffer();

	void CleanUp();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords, bool bNeedCPUAcess = true);

private:
	XStaticMeshVertexDataInterface* TangentsData;
	

};