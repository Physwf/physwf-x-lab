#include "LocalVertexFactory.h"
#include "Misc/AssertionMacros.h"

void FLocalVertexFactory::SetData(const FDataType & InData)
{
	check((InData.ColorComponent.Type == VET_None)|| (InData.ColorComponent.Type == VET_Color));
	Data = InData;
	UpdateRHI();
}

void FLocalVertexFactory::Copy(const FLocalVertexFactory& Other)
{

}

void FLocalVertexFactory::InitRHI()
{

}
