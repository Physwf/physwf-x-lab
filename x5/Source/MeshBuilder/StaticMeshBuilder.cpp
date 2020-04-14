#include "MeshBuilder/StaticMeshBuilder.h"
#include "StaticMeshResources.h"
#include "StaticMesh.h"
#include "Components.h"
#include "MeshDescription/MeshAttributes.h"

void BuildVertexBuffer(
	UStaticMesh *StaticMesh
	, int32 LodIndex
	, const FMeshDescription& MeshDescription
	, FStaticMeshLODResources& StaticMeshLOD
	//, const FMeshBuildSettings& LODBuildSettings
	, std::vector< uint32 >& IndexBuffer
	, std::vector<int32>& OutWedgeMap
	, std::vector<std::vector<uint32> >& OutPerSectionIndices
	, std::vector< FStaticMeshBuildVertex >& StaticMeshBuildVertices
	//, const TMultiMap<int32, int32>& OverlappingCorners
	, float VertexComparisonThreshold
	, std::vector<int32>& RemapVerts);

FStaticMeshBuilder::FStaticMeshBuilder()
{

}

bool FStaticMeshBuilder::Build(class FStaticMeshRenderData& StaticMeshRenderData, class UStaticMesh* StaticMesh)
{
	StaticMeshRenderData.AllocateLODResources(StaticMesh->SourceModels.size());

	std::vector<FMeshDescription> MeshDescriptions;
	MeshDescriptions.resize(StaticMesh->SourceModels.size());

	for (int32 LodIndex = 0; LodIndex < (int32)StaticMesh->SourceModels.size(); ++LodIndex)
	{
		MeshDescriptions[LodIndex] = *StaticMesh->GetOriginalMeshDescription(LodIndex);
		const FPolygonGroupArray& PolygonGroups = MeshDescriptions[LodIndex].PolygonGroups();

		FStaticMeshLODResources& StaticMeshLOD = *StaticMeshRenderData.LODResources[LodIndex];

		std::vector< FStaticMeshBuildVertex > StaticMeshBuildVertices;

		std::vector<uint32> IndexBuffer;

		StaticMeshLOD.Sections.clear();
		std::vector<int32> RemapVerts;
		std::vector<int32> TempWedgeMap;

		std::vector<std::vector<uint32> > PerSectionIndices;
		PerSectionIndices.resize(MeshDescriptions[LodIndex].PolygonGroups().Num());


		BuildVertexBuffer(StaticMesh, LodIndex, MeshDescriptions[LodIndex], StaticMeshLOD, IndexBuffer, TempWedgeMap, PerSectionIndices, StaticMeshBuildVertices, 0.0f, RemapVerts);

		std::vector<uint32> CombinedIndices;
		bool bNeeds32BitIndices = false;
		for (int32 SectionIndex = 0; SectionIndex < (int32)StaticMeshLOD.Sections.size(); SectionIndex++)
		{
			FStaticMeshSection& Section = StaticMeshLOD.Sections[SectionIndex];
			std::vector<uint32> const& SectionIndices = PerSectionIndices[SectionIndex];
			Section.FirstIndex = 0;
			Section.NumTriangles = 0;
			Section.MinVertexIndex = 0;
			Section.MaxVertexIndex = 0;

			if (SectionIndices.size())
			{
				Section.FirstIndex = (int32)CombinedIndices.size();
				Section.NumTriangles = (int32)SectionIndices.size() / 3;

				CombinedIndices.resize(CombinedIndices.size() + SectionIndices.size());
				uint32* DestPtr = &CombinedIndices[Section.FirstIndex];
				uint32 const* SrcPtr = SectionIndices.data();

				Section.MinVertexIndex = *SrcPtr;
				Section.MaxVertexIndex = *SrcPtr;

				for (int32 Index = 0; Index < (int32)SectionIndices.size(); Index++)
				{
					uint32 VertIndex = *SrcPtr++;

					bNeeds32BitIndices |= (VertIndex > MAX_uint16);
					Section.MinVertexIndex = FMath::Min<uint32>(VertIndex, Section.MinVertexIndex);
					Section.MaxVertexIndex = FMath::Max<uint32>(VertIndex, Section.MaxVertexIndex);
					*DestPtr++ = VertIndex;
				}
			}
		}

		const EIndexBufferStride::Type IndexBufferStride = bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit;
		StaticMeshLOD.IndexBuffer.SetIndices(CombinedIndices, IndexBufferStride);
	}
	return true;
}


void BuildVertexBuffer(UStaticMesh *StaticMesh, int32 LodIndex, const FMeshDescription& MeshDescription, FStaticMeshLODResources& StaticMeshLOD /*, const FMeshBuildSettings& LODBuildSettings */, std::vector< uint32 >& IndexBuffer, std::vector<int32>& OutWedgeMap, std::vector<std::vector<uint32> >& OutPerSectionIndices, std::vector< FStaticMeshBuildVertex >& StaticMeshBuildVertices /*, const TMultiMap<int32, int32>& OverlappingCorners */, float VertexComparisonThreshold, std::vector<int32>& RemapVerts)
{
	const FVertexArray& Vertices = MeshDescription.Vertices();
	const FVertexInstanceArray& VertexInstances = MeshDescription.VertexInstances();
	const FPolygonGroupArray& PolygonGroupArray = MeshDescription.PolygonGroups();
	const FPolygonArray& PolygonArray = MeshDescription.Polygons();

	OutWedgeMap.clear();
	OutWedgeMap.resize(VertexInstances.Num(),0);

	std::vector<int32> RemapVertexInstanceID;
	// set up vertex buffer elements
	StaticMeshBuildVertices.reserve(VertexInstances.Num());
	bool bHasColor = false;
	//Fill the remap array
	RemapVerts.resize(VertexInstances.Num());
	for (int32& RemapIndex : RemapVerts)
	{
		RemapIndex = -1;
	}
	std::vector<int32> DupVerts;

	const uint32 NumTextureCoord = MeshDescription.VertexInstanceAttributes().GetAttributeIndexCount<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate);

	const TPolygonGroupAttributeArray<std::string>& PolygonGroupImportedMaterialSlotNames = MeshDescription.PolygonGroupAttributes().GetAttributes<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName);

	const TVertexAttributeArray<FVector>& VertexPositions = MeshDescription.VertexAttributes().GetAttributes<FVector>(MeshAttribute::Vertex::Position);
	const TVertexInstanceAttributeArray<FVector>& VertexInstanceNormals = MeshDescription.VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Normal);
	const TVertexInstanceAttributeArray<FVector>& VertexInstanceTangents = MeshDescription.VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Tangent);
	const TVertexInstanceAttributeArray<float>& VertexInstanceBinormalSigns = MeshDescription.VertexInstanceAttributes().GetAttributes<float>(MeshAttribute::VertexInstance::BinormalSign);
	const TVertexInstanceAttributeArray<FVector4>& VertexInstanceColors = MeshDescription.VertexInstanceAttributes().GetAttributes<FVector4>(MeshAttribute::VertexInstance::Color);
	const TVertexInstanceAttributeIndicesArray<FVector2D>& VertexInstanceUVs = MeshDescription.VertexInstanceAttributes().GetAttributesSet<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate);

	std::map<FPolygonGroupID, int32> PolygonGroupToSectionIndex;


	for (const FPolygonGroupID PolygonGroupID : MeshDescription.PolygonGroups().GetElementIDs())
	{
		int32& SectionIndex = PolygonGroupToSectionIndex[PolygonGroupID];
		StaticMeshLOD.Sections.push_back(FStaticMeshSection());
		SectionIndex = StaticMeshLOD.Sections.size() - 1;
		FStaticMeshSection& StaticMeshSection = StaticMeshLOD.Sections[SectionIndex];
		//StaticMeshSection.MaterialIndex = StaticMesh->GetMaterialIndexFromImportedMaterialSlotName(PolygonGroupImportedMaterialSlotNames[PolygonGroupID]);
		if (StaticMeshSection.MaterialIndex == -1)
		{
			StaticMeshSection.MaterialIndex = PolygonGroupID.GetValue();
		}
	}

	int32 ReserveIndicesCount = 0;
	for (const FPolygonID& PolygonID : MeshDescription.Polygons().GetElementIDs())
	{
		const std::vector<FMeshTriangle>& PolygonTriangles = MeshDescription.GetPolygonTriangles(PolygonID);
		ReserveIndicesCount += (int32)PolygonTriangles.size() * 3;
	}
	IndexBuffer.clear();
	IndexBuffer.reserve(ReserveIndicesCount);

	for (const FPolygonID& PolygonID : MeshDescription.Polygons().GetElementIDs())
	{
		const FPolygonGroupID PolygonGroupID = MeshDescription.GetPolygonPolygonGroup(PolygonID);
		const int32 SectionIndex = PolygonGroupToSectionIndex[PolygonGroupID];
		std::vector<uint32>& SectionIndices = OutPerSectionIndices[SectionIndex];

		const std::vector<FMeshTriangle>& PolygonTriangles = MeshDescription.GetPolygonTriangles(PolygonID);
		uint32 MinIndex = TNumericLimits< uint32 >::Max();
		uint32 MaxIndex = TNumericLimits< uint32 >::Min();
		for (int32 TriangleIndex = 0; TriangleIndex < (int32)PolygonTriangles.size(); ++TriangleIndex)
		{
			const FMeshTriangle& Triangle = PolygonTriangles[TriangleIndex];

			FVector CornerPositions[3];
			for (int32 TriVert = 0; TriVert < 3; ++TriVert)
			{
				const FVertexInstanceID VertexInstanceID = Triangle.GetVertexInstanceID(TriVert);
				const FVertexID VertexID = MeshDescription.GetVertexInstanceVertex(VertexInstanceID);
				CornerPositions[TriVert] = VertexPositions[VertexID];
			}

			for (int32 TriVert = 0; TriVert < 3; ++TriVert)
			{
				const FVertexInstanceID VertexInstanceID = Triangle.GetVertexInstanceID(TriVert);
				const int32 VertexInstanceValue = VertexInstanceID.GetValue();
				const FVector& VertexPosition = CornerPositions[TriVert];
				const FVector& VertexInstanceNormal = VertexInstanceNormals[VertexInstanceID];
				const FVector& VertexInstanceTangent = VertexInstanceTangents[VertexInstanceID];
				const float VertexInstanceBinormalSign = VertexInstanceBinormalSigns[VertexInstanceID];
				const FVector4& VertexInstanceColor = VertexInstanceColors[VertexInstanceID];

				const FLinearColor LinearColor(VertexInstanceColor);
				if (LinearColor != FLinearColor::White)
				{
					bHasColor = true;
				}

				FStaticMeshBuildVertex StaticMeshVertex;

				StaticMeshVertex.Position = VertexPosition;
				StaticMeshVertex.TangentX = VertexInstanceTangent.GetSafeNormal();
				StaticMeshVertex.TangentY = FVector::CrossProduct(VertexInstanceNormal, VertexInstanceTangent).GetSafeNormal() * VertexInstanceBinormalSign;
				StaticMeshVertex.TangentZ = VertexInstanceNormal.GetSafeNormal();
				//StaticMeshVertex.Color = LinearColor.ToFColor(true);
// 				const uint32 MaxNumTexCoords = FMath::Min<int32>(MAX_MESH_TEXTURE_COORDS_MD, MAX_STATIC_TEXCOORDS);
// 				for (uint32 UVIndex = 0; UVIndex < MaxNumTexCoords; ++UVIndex)
// 				{
// 					if (UVIndex < NumTextureCoord)
// 					{
// 						StaticMeshVertex.UVs[UVIndex] = VertexInstanceUVs.GetArrayForIndex(UVIndex)[VertexInstanceID];
// 					}
// 					else
// 					{
// 						StaticMeshVertex.UVs[UVIndex] = FVector2D(0.0f, 0.0f);
// 					}
// 				}


				//Never add duplicated vertex instance
				int32 Index = -1;
				if (Index == -1)
				{
					StaticMeshBuildVertices.push_back(StaticMeshVertex);
					Index = (int32)StaticMeshBuildVertices.size() - 1;
				}
				RemapVerts[VertexInstanceValue] = Index;
				const uint32 RenderingVertexIndex = RemapVerts[VertexInstanceValue];
				IndexBuffer.push_back(RenderingVertexIndex);
				OutWedgeMap[VertexInstanceValue] = RenderingVertexIndex;
				SectionIndices.push_back(RenderingVertexIndex);
			}
		}
	}




	StaticMeshLOD.VertexBuffers.StaticMeshVertexBuffer.SetUseHighPrecisionTangentBasis(true);
	StaticMeshLOD.VertexBuffers.StaticMeshVertexBuffer.SetUseFullPrecisionUVs(true);
	StaticMeshLOD.VertexBuffers.StaticMeshVertexBuffer.Init(StaticMeshBuildVertices, NumTextureCoord);
	StaticMeshLOD.VertexBuffers.PositionVertexBuffer.Init(StaticMeshBuildVertices);
	StaticMeshLOD.VertexBuffers.ColorVertexBuffer.Init(StaticMeshBuildVertices);
}
