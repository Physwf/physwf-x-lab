#include "Mesh.h"
#include "fbxsdk.h"
#include "log.h"
#include "D3D11RHI.h"
#include <d3dcompiler.h>
#include <vector>

extern std::vector<MeshBatch> AllBatches;

void MeshLODResources::InitResource()
{
	VertexBuffer = CreateVertexBuffer(Vertices.data(), sizeof(LocalVertex) * Vertices.size());
	PositionOnlyVertexBuffer = CreateVertexBuffer(PositionOnlyVertices.data(), sizeof(PositionOnlyLocalVertex) * Vertices.size());
	IndexBuffer = CreateIndexBuffer(Indices.data(), sizeof(unsigned int) * Indices.size());
}

void MeshLODResources::ReleaseResource()
{
	if (VertexBuffer)
	{
		VertexBuffer->Release();
	}
	if (PositionOnlyVertexBuffer)
	{
		PositionOnlyVertexBuffer->Release();
	}
	if (IndexBuffer)
	{
		IndexBuffer->Release();
	}
}

void FillFbxArray(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray)
{
	if (pNode->GetMesh())
	{
		pOutMeshArray.push_back(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		FillFbxArray(pNode->GetChild(i),pOutMeshArray);
	}
}

FbxAMatrix ComputeTotalMatrix(FbxScene* pScence, FbxNode* pNode, bool bTransformVertexToAbsolute,bool bBakePivotInVertex)
{
	FbxAMatrix Geometry;
	FbxVector4 Translation, Rotation, Scaling;
	Translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	Rotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	Scaling = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	FbxAMatrix& GlobalTransform = pScence->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode);
	if (!bTransformVertexToAbsolute)
	{
		if (bBakePivotInVertex)
		{
			FbxAMatrix PivotGeometry;
			FbxVector4 RotationPivot = pNode->GetRotationPivot(FbxNode::eSourcePivot);
			FbxVector4 FullPivot;
			FullPivot[0] = -RotationPivot[0];
			FullPivot[1] = -RotationPivot[1];
			FullPivot[2] = -RotationPivot[2];
			PivotGeometry.SetT(FullPivot);
			Geometry = Geometry * PivotGeometry;
		}
		else
		{
			Geometry.SetIdentity();
		}
	}

	FbxAMatrix TotalMatrix = bTransformVertexToAbsolute ? GlobalTransform * Geometry : Geometry;

	return TotalMatrix;
}

Vector ConvertPos(FbxVector4 pPos)
{
	Vector Result;
	Result.X = (float)pPos[0];
	Result.Y = (float)pPos[1];
	Result.Z = (float)pPos[2];
	return Result;
}

Vector ConvertDir(FbxVector4 Vec)
{
	Vector Result;
	Result.X = (float)Vec[0];
	Result.Y = (float)Vec[1];
	Result.Z = (float)Vec[2];
	return Result;
}

void RegisterMeshAttributes(MeshDescription& MD)
{
	// Add basic vertex attributes
	MD.VertexAttributes().RegisterAttribute<Vector>(MeshAttribute::Vertex::Position, 1, Vector());
	MD.VertexAttributes().RegisterAttribute<float>(MeshAttribute::Vertex::CornerSharpness, 1, 0.0f);

	// Add basic vertex instance attributes
	MD.VertexInstanceAttributes().RegisterAttribute<Vector2>(MeshAttribute::VertexInstance::TextureCoordinate, 1, Vector2());
	MD.VertexInstanceAttributes().RegisterAttribute<Vector>(MeshAttribute::VertexInstance::Normal, 1, Vector());
	MD.VertexInstanceAttributes().RegisterAttribute<Vector>(MeshAttribute::VertexInstance::Tangent, 1, Vector());
	MD.VertexInstanceAttributes().RegisterAttribute<float>(MeshAttribute::VertexInstance::BinormalSign, 1, 0.0f);
	MD.VertexInstanceAttributes().RegisterAttribute<Vector4>(MeshAttribute::VertexInstance::Color, 1, Vector4(1.0f));

	// Add basic edge attributes
	MD.EdgeAttributes().RegisterAttribute<bool>(MeshAttribute::Edge::IsHard, 1, false);
	MD.EdgeAttributes().RegisterAttribute<bool>(MeshAttribute::Edge::IsUVSeam, 1, false);
	MD.EdgeAttributes().RegisterAttribute<float>(MeshAttribute::Edge::CreaseSharpness, 1, 0.0f);

	// Add basic polygon attributes
	MD.PolygonAttributes().RegisterAttribute<Vector>(MeshAttribute::Polygon::Normal, 1, Vector());
	MD.PolygonAttributes().RegisterAttribute<Vector>(MeshAttribute::Polygon::Tangent, 1, Vector());
	MD.PolygonAttributes().RegisterAttribute<Vector>(MeshAttribute::Polygon::Binormal, 1, Vector());
	MD.PolygonAttributes().RegisterAttribute<Vector>(MeshAttribute::Polygon::Center, 1, Vector());

	// Add basic polygon group attributes
	MD.PolygonGroupAttributes().RegisterAttribute<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName,1,std::string()); //The unique key to match the mesh material slot
}

void Mesh::ImportFromFBX(const char* pFileName)
{
	FbxManager* lFbxManager = FbxManager::Create();

	FbxIOSettings* lIOSetting = FbxIOSettings::Create(lFbxManager, IOSROOT);
	lFbxManager->SetIOSettings(lIOSetting);

	FbxImporter* lImporter = FbxImporter::Create(lFbxManager, "MeshImporter");
	if (!lImporter->Initialize(pFileName,-1, lFbxManager->GetIOSettings()))
	{
		X_LOG("Call to FbxImporter::Initialize() failed.\n");
		X_LOG("Error returned: %s \n\n", lImporter->GetStatus().GetErrorString());
		return;
	}

	RegisterMeshAttributes(MD);

	FbxScene* lScene = FbxScene::Create(lFbxManager, "Mesh");
	lImporter->Import(lScene);
	lImporter->Destroy();

	std::vector<FbxNode*> lOutMeshArray;
	FillFbxArray(lScene->GetRootNode(), lOutMeshArray);

	FbxGeometryConverter* GeometryConverter = new FbxGeometryConverter(lFbxManager);

	std::vector<FbxMaterial> MeshMaterials;

	for (int Index = 0; Index < (int)lOutMeshArray.size(); ++Index)
	{
		FbxNode* lNode = lOutMeshArray[Index];

		int MaterialCount = lNode->GetMaterialCount();
		int MaterialIndexOffset = (int)MeshMaterials.size();
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
		{
			MeshMaterials.push_back(FbxMaterial());
			FbxMaterial* NewMaterial = &MeshMaterials.back();
			FbxSurfaceMaterial* fbxMaterial = lNode->GetMaterial(MaterialIndex);
			NewMaterial->fbxMaterial = fbxMaterial;
		}

		
		if (lNode->GetMesh())
		{
			FbxMesh* lMesh = lNode->GetMesh();

			lMesh->RemoveBadPolygons();

			if (!lMesh->IsTriangleMesh())
			{
				FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(lMesh,true);

				if(ConvertedNode!=NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					lMesh = ConvertedNode->GetNode()->GetMesh();
				}
				else
				{
					X_LOG("Cant triangulate!");
				}
			}

			FbxLayer* BaseLayer = lMesh->GetLayer(0);

			FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
			FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ? LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;
			
			bool bSmootingAvaliable = false;
			FbxLayerElementSmoothing* SmoothingInfo = BaseLayer->GetSmoothing();
			FbxLayerElement::EReferenceMode SmoothingReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode SmoothingMappingMode(FbxLayerElement::eByEdge);
			if (SmoothingInfo)
			{
				if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
				{
					GeometryConverter->ComputeEdgeSmoothingFromPolygonSmoothing(lMesh, 0);
				}

				if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByEdge)
				{
					bSmootingAvaliable = true;
				}

				SmoothingReferenceMode = SmoothingInfo->GetReferenceMode();
				SmoothingMappingMode = SmoothingInfo->GetMappingMode();
			}

			FbxLayerElementVertexColor* LayerElementVertexColor = BaseLayer->GetVertexColors();
			FbxLayerElement::EReferenceMode VertexColorReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementVertexColor)
			{
				VertexColorReferenceMode = LayerElementVertexColor->GetReferenceMode();
				VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();
			}

			FbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
			FbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
			FbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

			bool bHasNTBInfomation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

			FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementNormal)
			{
				NormalReferenceMode = LayerElementNormal->GetReferenceMode();
				NormalMappingMode = LayerElementNormal->GetMappingMode();
			}

			FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementTangent)
			{
				TangentReferenceMode = LayerElementTangent->GetReferenceMode();
				TangentMappingMode = LayerElementTangent->GetMappingMode();
			}

			FbxLayerElement::EReferenceMode BinormalReferenceMode(FbxLayerElement::eDirect);
			FbxLayerElement::EMappingMode BinormalMappingMode(FbxLayerElement::eByControlPoint);
			if (LayerElementBinormal)
			{
				BinormalReferenceMode = LayerElementBinormal->GetReferenceMode();
				BinormalMappingMode = LayerElementBinormal->GetMappingMode();
			}

			FbxAMatrix TotalMatrix;
			FbxAMatrix TotalMatrixForNormal;
			TotalMatrix = ComputeTotalMatrix(lScene, lNode, true, false);
			TotalMatrixForNormal = TotalMatrix.Inverse();
			TotalMatrixForNormal = TotalMatrixForNormal.Transpose();

			std::vector<Vector>& VertexPositions =  MD.VertexAttributes().GetAttributes<Vector>(MeshAttribute::Vertex::Position);

			std::vector<Vector>& VertexInstanceNormals = MD.VertexInstanceAttributes().GetAttributes<Vector>(MeshAttribute::VertexInstance::Normal);
			std::vector<Vector>& VertexInstanceTangents = MD.VertexInstanceAttributes().GetAttributes<Vector>(MeshAttribute::VertexInstance::Tangent);
			std::vector<float>& VertexInstanceBinormalSigns = MD.VertexInstanceAttributes().GetAttributes<float>(MeshAttribute::VertexInstance::BinormalSign);
			std::vector<Vector4>& VertexInstanceColors = MD.VertexInstanceAttributes().GetAttributes<Vector4>(MeshAttribute::VertexInstance::Color);
			std::vector<Vector2>& VertexInstanceUVs = MD.VertexInstanceAttributes().GetAttributes<Vector2>(MeshAttribute::VertexInstance::TextureCoordinate);

			std::vector<bool>& EdgeHardnesses = MD.EdgeAttributes().GetAttributes<bool>(MeshAttribute::Edge::IsHard);
			std::vector<float>& EdgeCreaseSharpnesses = MD.EdgeAttributes().GetAttributes<float>(MeshAttribute::Edge::CreaseSharpness);

			std::vector<std::string>& PolygonGroupImportedMaterialSlotNames = MD.PolygonGroupAttributes().GetAttributes<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName);

			int VertexCount = lMesh->GetControlPointsCount();
			int VertexOffset = MD.Vertices().Num();
			int VertexInstanceOffset = MD.VertexInstances().Num();
			int PolygonOffset = MD.Polygons().Num();

			std::map<int, int> PolygonGroupMapping;

			for (auto VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
			{
				int RealVertexIndex = VertexOffset + VertexIndex;
				FbxVector4 FbxPosition = lMesh->GetControlPoints()[VertexIndex];
				FbxPosition = TotalMatrix.MultT(FbxPosition);
				Vector const VectorPositon = ConvertPos(FbxPosition);
				int VertexID = MD.CreateVertex();
				VertexPositions[VertexID] = VectorPositon;
			}

			int PolygonCount = lMesh->GetPolygonCount();
			int CurrentVertexInstanceIndex = 0;
			int SkippedVertexInstance = 0;

			lMesh->BeginGetMeshEdgeIndexForPolygon();
			for (auto PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
			{
				int PolygonVertexCount = lMesh->GetPolygonSize(PolygonIndex);
				std::vector<Vector> P;
				P.resize(PolygonVertexCount, Vector());
				for (int32 CornerIndex = 0; CornerIndex < PolygonVertexCount; CornerIndex++)
				{
					const int ControlPointIndex = lMesh->GetPolygonVertex(PolygonIndex, CornerIndex);
					const int VertexID = VertexOffset + ControlPointIndex;
					P[CornerIndex] = VertexPositions[VertexID];
				}

				int RealPolygonIndex = PolygonOffset + PolygonIndex;
				std::vector<int> CornerInstanceIDs;
				CornerInstanceIDs.resize(PolygonVertexCount);
				std::vector<int> CornerVerticesIDs;
				CornerVerticesIDs.resize(PolygonVertexCount);

				for (auto CornerIndex = 0; CornerIndex < PolygonVertexCount; ++CornerIndex, CurrentVertexInstanceIndex++)
				{
					int VertexInstanceIndex = VertexInstanceOffset + CurrentVertexInstanceIndex;
					int RealFbxVertexIndex = SkippedVertexInstance + CurrentVertexInstanceIndex;
					const int VertexInstanceID = VertexInstanceIndex;
					CornerInstanceIDs[CornerIndex] = VertexInstanceID;
					const int ControlPointIndex = lMesh->GetPolygonVertex(PolygonIndex, CornerIndex);
					const int VertexID = VertexOffset + ControlPointIndex;
					const Vector VertexPostion = VertexPositions[VertexID];
					CornerVerticesIDs[CornerIndex] = VertexID;

					int AddedVertexInstanceId = MD.CreateVertexInstance(VertexID);

					if (LayerElementVertexColor)
					{
						int VertexColorMappingIndex = (VertexColorMappingMode == FbxLayerElement::eByControlPoint) ? lMesh->GetPolygonVertex(PolygonIndex, CornerIndex) : RealFbxVertexIndex;
						int VertexColorIndex = (VertexColorReferenceMode == FbxLayerElement::eDirect) ? VertexColorMappingIndex : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIndex);
						FbxColor VertexColor = LayerElementVertexColor->GetDirectArray().GetAt(VertexColorIndex);
						//todo setcolor
					}
					
					if (LayerElementNormal)
					{
						int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
						int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);
						FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
						TempValue = TotalMatrixForNormal.MultT(TempValue);
						Vector TangentZ = ConvertDir(TempValue);
						VertexInstanceNormals[AddedVertexInstanceId] = TangentZ;

						if (bHasNTBInfomation)
						{
							int TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
							int TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ? TangentMapIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMapIndex);

							TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);
							TempValue = TotalMatrixForNormal.MultT(TempValue);
							Vector TangentX = ConvertDir(TempValue);
							VertexInstanceTangents[AddedVertexInstanceId] = TangentX;

							int BinormalMapIndex = (BinormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
							int BinormalValueIndex = (BinormalReferenceMode == FbxLayerElement::eDirect) ? BinormalMapIndex : LayerElementBinormal->GetIndexArray().GetAt(BinormalMapIndex);

							TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
							TempValue = TotalMatrixForNormal.MultT(TempValue);
							Vector TangentY = -ConvertDir(TempValue);
							VertexInstanceBinormalSigns[AddedVertexInstanceId] = 1.0f;// GetBasisDeterminantSign(TangentX.GetSafeNormal(), TangentY.GetSafeNormal(), TangentZ.GetSafeNormal());
						}
					}
				}

				int MaterialIndex = 0;
				if (MaterialCount > 0)
				{
					if (LayerElementMaterial)
					{
						switch (MaterialMappingMode)
						{
						case FbxLayerElement::eAllSame:
							MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(0);
							break;
						case FbxLayerElement::eByPolygon:
							MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(PolygonIndex);
							break;
						}
					}
				}

				int RealMaterialIndex = MaterialIndexOffset + MaterialIndex;
				if (PolygonGroupMapping.find(RealMaterialIndex) == PolygonGroupMapping.end())
				{
					std::string ImportedMaterialSlotName = RealMaterialIndex < (int)MeshMaterials.size() ? MeshMaterials[RealMaterialIndex].GetName() : "None";
					int ExistingPolygonGroup = -1;
					for (const int PolygonGroupID : MD.PolygonGroups().GetElementIDs())
					{
						if (PolygonGroupImportedMaterialSlotNames[PolygonGroupID] == ImportedMaterialSlotName)
						{
							ExistingPolygonGroup = PolygonGroupID;
							break;
						}
					}
					if (ExistingPolygonGroup == -1)
					{
						ExistingPolygonGroup = MD.CreatePolygonGroup();
						PolygonGroupImportedMaterialSlotNames[ExistingPolygonGroup] = ImportedMaterialSlotName;
					}
					PolygonGroupMapping.insert(std::make_pair(RealMaterialIndex, ExistingPolygonGroup));
				}

				std::vector<MeshDescription::ContourPoint> Contours;
				{
					for (uint32 PolygonEdgeNumber = 0; PolygonEdgeNumber < (uint32)PolygonVertexCount; ++PolygonEdgeNumber)
					{
						Contours.push_back(MeshDescription::ContourPoint());
						uint32 ContourPointIndex = Contours.size() - 1;
						MeshDescription::ContourPoint& CP = Contours[ContourPointIndex];
						uint32 CornerIndices[2];
						CornerIndices[0] = (PolygonEdgeNumber + 0) % PolygonVertexCount;
						CornerIndices[1] = (PolygonEdgeNumber + 1) % PolygonVertexCount;

						int EdgeVertexIDs[2];
						EdgeVertexIDs[0] = CornerVerticesIDs[CornerIndices[0]];
						EdgeVertexIDs[1] = CornerVerticesIDs[CornerIndices[1]];

						int MatchEdgeId = MD.GetVertexPairEdge(EdgeVertexIDs[0], EdgeVertexIDs[1]);
						if (MatchEdgeId == -1)
						{
							MatchEdgeId = MD.CreateEdge(EdgeVertexIDs[0], EdgeVertexIDs[1]);
						}
						CP.EdgeID = MatchEdgeId;
						CP.VertexInstanceID = CornerInstanceIDs[CornerIndices[0]];

						//int EdgeIndex = -1;


					}
				}

				int PolygonGroupID = PolygonGroupID = PolygonGroupMapping[RealMaterialIndex];
				const int NewPolygonID = MD.CreatePolygon(PolygonGroupID, Contours);
				MeshPolygon& Polygon = MD.GetPolygon(NewPolygonID);
				MD.ComputePolygonTriangulation(NewPolygonID, Polygon.Triangles);
			}
			lMesh->EndGetMeshEdgeIndexForPolygon();
		}
	}

	delete GeometryConverter;
	GeometryConverter = nullptr;

	Build();
}

void Mesh::GeneratePlane(float InWidth, float InHeight, int InNumSectionW, int InNumSectionH)
{
	LODResource.Vertices.clear();
	LODResource.Indices.clear();
	LODResource.Sections.clear();

	float IntervalX = InWidth / InNumSectionW;
	float IntervalY = InHeight / InNumSectionH;
	for (int i = 0; i <= InNumSectionW; ++i)
	{
		for (int j = 0; j <= InNumSectionH; ++j)
		{
			float X = IntervalX * i;
			float Y = IntervalY * j;
			LocalVertex V;
			V.Position = { X - InWidth * 0.5f, Y - InHeight * 0.5f, 0.0f };
			V.TangentZ = {0,0,1};
			LODResource.Vertices.push_back(V);

			PositionOnlyLocalVertex PLV;
			PLV.Position = { X - InWidth * 0.5f, Y - InHeight * 0.5f, 0.0f , 1.0f};
			LODResource.PositionOnlyVertices.push_back(PLV);
		}
	}

	int Count = InNumSectionW * InNumSectionH;
	for (int i = 0; i < Count;++i)
	{
		LODResource.Indices.push_back(i);
		LODResource.Indices.push_back(i + 1);
		LODResource.Indices.push_back(i + 1 + InNumSectionW + 1);
		LODResource.Indices.push_back(i);
		LODResource.Indices.push_back(i + 1 + InNumSectionW + 1);
		LODResource.Indices.push_back(i + InNumSectionW + 1);
	}

	StaticMeshSection Section;
	Section.FirstIndex = 0;
	Section.NumTriangles = Count * 2;
	LODResource.Sections.push_back(Section);

	LODResource.InitResource();
}

void Mesh::GnerateBox(float InSizeX, float InSizeY, float InSizeZ, int InNumSectionX, int InNumSectionY, int InNumSectionZ)
{

}

void Mesh::InitResource()
{
	LODResource.InitResource();
	PrimitiveUniform PU;
	PU.LocalToWorld = GetWorldMatrix();
	PrimitiveUniformBuffer = CreateConstantBuffer(&PU, sizeof(PU));
	
}

void Mesh::ReleaseResource()
{
	LODResource.ReleaseResource();
}

bool Mesh::GetMeshElement(int BatchIndex, int SectionIndex, MeshBatch& OutMeshBatch)
{
	const StaticMeshSection& Section = LODResource.Sections[SectionIndex];
	MeshElement Element;
	Element.PrimitiveUniformBuffer = PrimitiveUniformBuffer;
	Element.FirstIndex = Section.FirstIndex;
	Element.NumTriangles = Section.NumTriangles;
	Element.MaterialIndex = Section.MaterialIndex;
	OutMeshBatch.Elements.emplace_back(Element);
	return true;
}

void Mesh::Draw(ID3D11DeviceContext* Context, const MeshRenderState& RenderState)
{
	Context->IASetInputLayout(RenderState.InputLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT Stride = sizeof(StaticMeshBuildVertex);
	UINT Offset = 0;
	Context->IASetVertexBuffers(0, 1, &LODResource.VertexBuffer, &Stride, &Offset);
	Context->IASetIndexBuffer(LODResource.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	Context->VSSetShader(RenderState.VertexShader, 0, 0);
	Context->PSSetShader(RenderState.PixelShader, 0, 0);

	Context->VSSetConstantBuffers(1, 1, &PrimitiveUniformBuffer);
	PrimitiveUniform PU;
	PU.LocalToWorld = GetWorldMatrix();
	Context->UpdateSubresource(PrimitiveUniformBuffer, 0, 0, &PU, 0, 0);


	for (const auto& Section : LODResource.Sections)
	{
		Context->DrawIndexed(Section.NumTriangles * 3, Section.FirstIndex, 0);
	}
}

void Mesh::DrawStaticElement()
{
	for (size_t SectionIndex = 0; SectionIndex < LODResource.Sections.size();++SectionIndex)
	{
		MeshBatch MB;
		MB.IndexBuffer = LODResource.IndexBuffer;
		MB.VertexBuffer = LODResource.VertexBuffer;
		MB.PositionOnlyVertexBuffer = LODResource.PositionOnlyVertexBuffer;

		for (int BatchIndex = 0; BatchIndex < GetNumberBatches(); ++BatchIndex)
		{
			if (GetMeshElement(BatchIndex, SectionIndex, MB))
			{
				AllBatches.emplace_back(MB);
			}
		}
	}
}

void Mesh::Build()
{
	std::vector<std::vector<uint32> > OutPerSectionIndices;
	OutPerSectionIndices.resize(MD.PolygonGroups().Num());
	std::vector<StaticMeshBuildVertex> StaticMeshBuildVertices;
	BuildVertexBuffer(OutPerSectionIndices, StaticMeshBuildVertices);

	std::vector<uint32> CombinedIndices;
	for (uint32 i = 0; i < LODResource.Sections.size(); ++i)
	{
		StaticMeshSection& Section = LODResource.Sections[i];
		std::vector<uint32> const& SectionIndices = OutPerSectionIndices[i];
		Section.FirstIndex = 0;
		Section.NumTriangles = 0;
		Section.MinVertexIndex = 0;
		Section.MaxVertexIndex = 0;

		if (SectionIndices.size())
		{
			Section.FirstIndex = CombinedIndices.size();
			Section.NumTriangles = SectionIndices.size() / 3;

			CombinedIndices.resize(CombinedIndices.size() + SectionIndices.size(), 0);
			uint32* DestPtr = &CombinedIndices[Section.FirstIndex];
			uint32 const* SrcPtr = SectionIndices.data();

			Section.MinVertexIndex = *SrcPtr;
			Section.MaxVertexIndex = *DestPtr;

			for (uint32 Index = 0; Index < SectionIndices.size(); Index++)
			{
				uint32 VertIndex = *SrcPtr++;
				Section.MinVertexIndex = Section.MinVertexIndex > VertIndex ? VertIndex : Section.MinVertexIndex;
				Section.MaxVertexIndex = Section.MaxVertexIndex < VertIndex ? VertIndex : Section.MaxVertexIndex;
				*DestPtr++ = VertIndex;
			}
		}
	}

	LODResource.Indices = std::move(CombinedIndices);
	for (const auto& V : StaticMeshBuildVertices)
	{
		LocalVertex LV;
		LV.Position = Vector4(V.Position,1.0f);
		LV.TangentX = V.TangentX;
		Vector Binormal = V.TangentZ ^ V.TangentX;
		LV.TangentZ = Vector4(V.TangentZ, Binormal == V.TangentY ? 1.f : 0);
		LODResource.Vertices.push_back(LV);

		PositionOnlyLocalVertex PLV;
		PLV.Position = Vector4(V.Position,1.0f);
		LODResource.PositionOnlyVertices.push_back(PLV);
	}
}

void Mesh::BuildVertexBuffer(std::vector<std::vector<uint32> >& OutPerSectionIndices, std::vector<StaticMeshBuildVertex>& StaticMeshBuildVertices)
{
	const TMeshElementArray<MeshVertex>& Vertices = MD.Vertices();
	const TMeshElementArray<MeshVertexInstance>& VertexInstances = MD.VertexInstances();
	const TMeshElementArray<MeshPolygonGroup>& PolygonGroupArray = MD.PolygonGroups();
	const TMeshElementArray<MeshPolygon>& PolygonArray = MD.Polygons();

	const std::vector<std::string>& PolygonGroupImportedMaterialSlotNames = MD.PolygonGroupAttributes().GetAttributes<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName);

	const std::vector<Vector>& VertexPositions = MD.VertexAttributes().GetAttributes<Vector>(MeshAttribute::Vertex::Position);
	const std::vector<Vector>& VertexInstanceNormals = MD.VertexInstanceAttributes().GetAttributes<Vector>(MeshAttribute::VertexInstance::Normal);
	const std::vector<Vector>& VertexInstanceTangents = MD.VertexInstanceAttributes().GetAttributes<Vector>(MeshAttribute::VertexInstance::Tangent);
	const std::vector<float>& VertexInstanceBinormalSigns = MD.VertexInstanceAttributes().GetAttributes<float>(MeshAttribute::VertexInstance::BinormalSign);
	const std::vector<Vector4>& VertexInstanceColors = MD.VertexInstanceAttributes().GetAttributes<Vector4>(MeshAttribute::VertexInstance::Color);
	const std::vector<std::vector<Vector2>>& VertexInstanceUVs = MD.VertexInstanceAttributes().GetAttributesSet<Vector2>(MeshAttribute::VertexInstance::TextureCoordinate);

	std::map<int, int> PolygonGroupToSectionIndex;
	for (const int PolgyonGroupID : MD.PolygonGroups().GetElementIDs())
	{
		int& SectionIndex = PolygonGroupToSectionIndex[PolgyonGroupID];
		LODResource.Sections.push_back(StaticMeshSection());
		SectionIndex = (int)LODResource.Sections.size() - 1;
		StaticMeshSection& Section = LODResource.Sections[SectionIndex];
		//if (Section.MaterialIndex == -1)
		{
			Section.MaterialIndex = PolgyonGroupID;
		}
	}

	int ReserveIndicesCount = 0;
	for (const int PolygonID : MD.Polygons().GetElementIDs())
	{
		const std::vector<MeshTriangle>& PolygonTriangles = MD.GetPolygonTriangles(PolygonID);
		ReserveIndicesCount += PolygonTriangles.size() * 3;
	}
	LODResource.Indices.reserve(ReserveIndicesCount);

	for (const int PolygonID : MD.Polygons().GetElementIDs())
	{
		const int PolygonGroupID = MD.GetPolygonPolygonGroup(PolygonID);
		const int SectionIndex = PolygonGroupToSectionIndex[PolygonGroupID];
		std::vector<uint32>& SectionIndices = OutPerSectionIndices[SectionIndex];
		const std::vector<MeshTriangle>& PolygonTriangles = MD.GetPolygonTriangles(PolygonID);
		uint32 MinIndex = 0;
		uint32 MaxIndex = 0xFFFFFFFF;
		for (int TriangleIndex = 0; TriangleIndex < (int)PolygonTriangles.size(); ++TriangleIndex)
		{
			const MeshTriangle& Triangle = PolygonTriangles[TriangleIndex];
			Vector CornerPositions[3];
			for (int TriVert = 0; TriVert < 3; ++TriVert)
			{
				const int VertexInstanceID = Triangle.GetVertexInstanceID(TriVert);
				const int VertexID = MD.GetVertexInstanceVertex(VertexInstanceID);
				CornerPositions[TriVert] = VertexPositions[VertexID];
			}

			for (int TriVert = 0; TriVert < 3; ++TriVert)
			{
				const int VertexInstanceID = Triangle.GetVertexInstanceID(TriVert);
				const int VertexInstanceValue = VertexInstanceID;
				const Vector& VertexPosition = CornerPositions[TriVert];
				const Vector& VertexNormal = VertexInstanceNormals[VertexInstanceID];
				const Vector& VertexTangent = VertexInstanceTangents[VertexInstanceID];
				const float VertexInstanceBinormalSign = VertexInstanceBinormalSigns[VertexInstanceID];

				StaticMeshBuildVertex StaticMeshVertex;
				StaticMeshVertex.Position = VertexPosition;
				StaticMeshVertex.TangentX = VertexTangent;
				StaticMeshVertex.TangentY = VertexNormal ^ VertexTangent * VertexInstanceBinormalSign;
				StaticMeshVertex.TangentZ = VertexNormal;

				StaticMeshBuildVertices.push_back(StaticMeshVertex);
				SectionIndices.push_back(StaticMeshBuildVertices.size() - 1) ;
			}
		}

	}
}
