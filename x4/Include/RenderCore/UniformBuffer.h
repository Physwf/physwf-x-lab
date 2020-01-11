#pragma once

#include "RenderResource.h"
#include "RHI/RHIDefinitions.h"

template<typename TBufferStruct>
class TUniformBuffer : public FRenderResource
{
public:

	TUniformBuffer()

	EUniformBufferUsage BufferUsage;
private:


}