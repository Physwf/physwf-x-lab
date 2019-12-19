#include "StaticMesh.h"
#include "Meshdescription/MeshAttributes.h"

bool FStaticMeshSourceModel::IsRawMeshEmpty() const
{
	return false;
}

void FStaticMeshSourceModel::LoadRawMesh(struct FRawMesh& OutRawMesh) const
{

}

void FStaticMeshSourceModel::SaveRawMesh(struct FRawMesh& InRawMesh, bool bConvertToMeshdescription /*= true*/)
{

}

FStaticMeshSourceModel::FStaticMeshSourceModel()
{

}

FStaticMeshSourceModel::~FStaticMeshSourceModel()
{

}

UStaticMeshDescriptions::UStaticMeshDescriptions()
{

}

UStaticMeshDescriptions::~UStaticMeshDescriptions()
{

}

void UStaticMeshDescriptions::Empty()
{
	MeshDescriptions.clear();
}

int32 UStaticMeshDescriptions::Num() const
{
	return (int32)MeshDescriptions.size();
}

void UStaticMeshDescriptions::SetNum(const int32 Num)
{
	MeshDescriptions.resize(Num);
}

FMeshDescription* UStaticMeshDescriptions::Get(int32 Index) const
{
	return MeshDescriptions[Index].get();
}

FMeshDescription* UStaticMeshDescriptions::Create(int32 Index)
{
	return (MeshDescriptions[Index] = std::make_unique<FMeshDescription>()).get();
}

void UStaticMeshDescriptions::Reset(int32 Index)
{
	MeshDescriptions[Index].reset();
}

void UStaticMeshDescriptions::InsertAt(int32 Index, int32 Count /*= 1*/)
{
	while (Count--)
	{
		MeshDescriptions.insert(MeshDescriptions.begin() + Index, std::make_unique<FMeshDescription>());
	}
}

void UStaticMeshDescriptions::RemoveAt(int32 Index, int32 Count /*= 1*/)
{
	MeshDescriptions.erase(MeshDescriptions.begin() + Index, MeshDescriptions.begin() + Index + Count);
}

static uint32 GetMeshMaterialKey(int32 LODIndex, int32 SectionIndex)
{
	return ((LODIndex & 0xffff) << 16) | (SectionIndex & 0xffff);
}

void FMeshSectionInfoMap::Clear()
{
	Map.clear();
}

int32 FMeshSectionInfoMap::GetSectionNumber(int32 LODIndex) const
{
	int32 SectionCount = 0;
	for (auto kvp : Map)
	{
		if (((kvp.first & 0xffff0000) >> 16) == LODIndex)
		{
			SectionCount++;
		}
	}
	return SectionCount;
}

bool FMeshSectionInfoMap::IsValidSection(int32 LODIndex, int32 SectionIndex) const
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	return (Map.find(Key) != Map.end());
}

FMeshSectionInfo FMeshSectionInfoMap::Get(int32 LODIndex, int32 SectionIndex) const
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	auto It = Map.find(Key);
	if (It == Map.end())
	{
		Key = GetMeshMaterialKey(0, SectionIndex);
		It = Map.find(Key);
	}
	if (It != Map.end())
	{
		return It->second;
	}
	return FMeshSectionInfo(SectionIndex);
}

void FMeshSectionInfoMap::Set(int32 LODIndex, int32 SectionIndex, FMeshSectionInfo Info)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.insert(std::make_pair(Key, Info));
}

void FMeshSectionInfoMap::Remove(int32 LODIndex, int32 SectionIndex)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.erase(Key);
}

void FMeshSectionInfoMap::CopyFrom(const FMeshSectionInfoMap& Other)
{
	for (auto It = Other.Map.begin(); It!= Other.Map.end(); ++It)
	{
		Map[It->first] = It->second;
	}
}

bool FMeshSectionInfoMap::AnySectionHasCollision(int32 LodIndex) const
{
	return false;
}

FMeshDescription* UStaticMesh::GetOriginalMeshDescription(int32 LodIndex)
{
	LoadMeshDescriptions();

	if ((int32)SourceModels.size() > LodIndex)
	{
		//check(MeshDescriptions->Num() == SourceModels.Num());
		//check(MeshDescriptions->Get(LodIndex) == SourceModels[LodIndex].OriginalMeshDescription);

		if (MeshDescriptions->Get(LodIndex) == nullptr && !SourceModels[LodIndex].IsRawMeshEmpty())
		{
			// If the MeshDescriptions are out of sync with the SourceModels RawMesh, perform a conversion here.
			// @todo: once all tools are ported, we can replace this with the disabled check() above.
			FMeshDescription* MeshDescription = MeshDescriptions->Create(LodIndex);
			SourceModels[LodIndex].OriginalMeshDescription = MeshDescription;
			RegisterMeshAttributes(*MeshDescription);

// 			FRawMesh LodRawMesh;
// 			SourceModels[LodIndex].LoadRawMesh(LodRawMesh);
// 			TMap<int32, FName> MaterialMap;
// 			FillMaterialName(StaticMaterials, MaterialMap);
// 			FMeshDescriptionOperations::ConvertFromRawMesh(LodRawMesh, *MeshDescription, MaterialMap);
		}

		return MeshDescriptions->Get(LodIndex);
	}

	return nullptr;
}

FMeshDescription* UStaticMesh::CreateOriginalMeshDescription(int32 LodIndex)
{
	LoadMeshDescriptions();

	if ((int32)SourceModels.size() > LodIndex)
	{
		// @todo: mark package dirty once MeshDescriptions is packaged with the static mesh

		//check(MeshDescriptions->Num() == SourceModels.Num());
		FMeshDescription* MeshDescription = MeshDescriptions->Create(LodIndex);
		SourceModels[LodIndex].OriginalMeshDescription = MeshDescription;
		return MeshDescription;
	}

	return nullptr;
}

void UStaticMesh::CommitOriginalMeshDescription(int32 LodIndex)
{

}

void UStaticMesh::ClearOriginalMeshDescription(int32 LodIndex)
{

}

FStaticMeshSourceModel& UStaticMesh::AddSourceModel()
{
	LoadMeshDescriptions();
	//check(MeshDescriptions->Num() == SourceModels.Num());
	SourceModels.push_back(FStaticMeshSourceModel());
	int32 LodModelIndex = SourceModels.size();
	MeshDescriptions->SetNum(SourceModels.size());
	SourceModels[LodModelIndex-1].StaticMeshOwner = this;
	return SourceModels[LodModelIndex - 1];
}

void UStaticMesh::SetNumSourceModels(int32 Num)
{

}

void UStaticMesh::RemoveSourceModel(int32 Index)
{

}

void UStaticMesh::RegisterMeshAttributes(FMeshDescription& MeshDescription)
{
	MeshDescription.VertexAttributes().RegisterAttribute<FVector>(MeshAttribute::Vertex::Position,1,FVector::ZeroVector, EMeshAttributeFlags::Lerpable);
	MeshDescription.VertexAttributes().RegisterAttribute<float>(MeshAttribute::Vertex::CornerSharpness, 1, 0.0f, EMeshAttributeFlags::Lerpable);

	// Add basic vertex instance attributes
	MeshDescription.VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 1, FVector2D::ZeroVector, EMeshAttributeFlags::Lerpable);
	MeshDescription.VertexInstanceAttributes().RegisterAttribute<FVector>(MeshAttribute::VertexInstance::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.VertexInstanceAttributes().RegisterAttribute<FVector>(MeshAttribute::VertexInstance::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.VertexInstanceAttributes().RegisterAttribute<float>(MeshAttribute::VertexInstance::BinormalSign, 1, 0.0f, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.VertexInstanceAttributes().RegisterAttribute<FVector4>(MeshAttribute::VertexInstance::Color, 1, FVector4(1.0f), EMeshAttributeFlags::Lerpable);

	// Add basic edge attributes
	MeshDescription.EdgeAttributes().RegisterAttribute<bool>(MeshAttribute::Edge::IsHard, 1, false);
	MeshDescription.EdgeAttributes().RegisterAttribute<bool>(MeshAttribute::Edge::IsUVSeam, 1, false);
	MeshDescription.EdgeAttributes().RegisterAttribute<float>(MeshAttribute::Edge::CreaseSharpness, 1, 0.0f, EMeshAttributeFlags::Lerpable);

	// Add basic polygon attributes
	MeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Binormal, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);
	MeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Center, 1, FVector::ZeroVector, EMeshAttributeFlags::AutoGenerated);

	// Add basic polygon group attributes
	MeshDescription.PolygonGroupAttributes().RegisterAttribute<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName); //The unique key to match the mesh material slot
}

void UStaticMesh::LoadMeshDescriptions()
{
	if (MeshDescriptions)
	{
		//Sync the already loaded MeshDescription
		MeshDescriptions->SetNum(SourceModels.size());
		for (int32 LodIndex = 0; LodIndex < MeshDescriptions->Num(); ++LodIndex)
		{
			//Get the missing MeshDescription to create them from the FRawMesh
			if (MeshDescriptions->Get(LodIndex) == nullptr && !SourceModels[LodIndex].IsRawMeshEmpty())
			{
				// If the MeshDescriptions are out of sync with the SourceModels RawMesh, perform a conversion here.
				// @todo: once all tools are ported, we can replace this with a check() instead.
				FMeshDescription* MeshDescription = MeshDescriptions->Create(LodIndex);
				SourceModels[LodIndex].OriginalMeshDescription = MeshDescription;
				RegisterMeshAttributes(*MeshDescription);

// 				FRawMesh LodRawMesh;
// 				SourceModels[LodIndex].LoadRawMesh(LodRawMesh);
// 				TMap<int32, FName> MaterialMap;
// 				FillMaterialName(StaticMaterials, MaterialMap);
// 				FMeshDescriptionOperations::ConvertFromRawMesh(LodRawMesh, *MeshDescription, MaterialMap);
			}
		}
	}
	else
	{
		MeshDescriptions = new UStaticMeshDescriptions();

		//check(MeshDescriptions->Num() == SourceModels.Num());
		for (int32 Index = 0; Index < (int32)SourceModels.size(); ++Index)
		{
			SourceModels[Index].OriginalMeshDescription = MeshDescriptions->Get(Index);
		}
	}
}

void UStaticMesh::UnloadMeshDescriptions()
{

}

void FObjImporter::Import(const char* InFileName, UStaticMesh* OutMesh)
{

}
