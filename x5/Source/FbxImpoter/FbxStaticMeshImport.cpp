#include "FbxImporter/FbxImporter.h"
#include "MeshDescription/MeshAttributes.h"
#include "Misc/AssertionMacros.h"
#include "RenderCore/RenderUtils.h"

FVector FFbxDataConverter::ConvertPos(FbxVector4 Vector)
{
	FVector Out;
	Out[0] = (float)Vector[0];
	// flip Y, then the right-handed axis system is converted to LHS
	Out[1] = -(float)Vector[1];
	Out[2] = (float)Vector[2];
	return Out;
}

FVector FFbxDataConverter::ConvertDir(FbxVector4 Vector)
{
	FVector Out;
	Out[0] = (float)Vector[0];
	Out[1] = -(float)Vector[1];
	Out[2] = (float)Vector[2];
	return Out;
}

FFbxImporter::FFbxImporter()
{
	SdkManager = FbxManager::Create();

	FbxIOSettings* lIOSetting = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(lIOSetting);

	ImportOptions = new FBXImportOptions();
}

void FFbxImporter::ReadAllMeshNodes(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray)
{
	if (pNode->GetMesh())
	{
		pOutMeshArray.push_back(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		ReadAllMeshNodes(pNode->GetChild(i), pOutMeshArray);
	}
}

UStaticMesh* FFbxImporter::ImportStaticMesh(const char* InFileName, int LODIndex/* = 0*/)
{
	
	FbxImporter* lImporter = FbxImporter::Create(SdkManager, "MeshImporter");
	if (!lImporter->Initialize(InFileName, -1, SdkManager->GetIOSettings()))
	{
		//X_LOG("Call to FbxImporter::Initialize() failed.\n");
		//X_LOG("Error returned: %s \n\n", lImporter->GetStatus().GetErrorString());
		std::string error = lImporter->GetStatus().GetErrorString();
		return nullptr;
	}
	Scene = FbxScene::Create(SdkManager, "Mesh");
	lImporter->Import(Scene);

	GeometryConverter = new FbxGeometryConverter(SdkManager);

	//读出所有Mesh节点
	std::vector<FbxNode*> lOutMeshArray;
	ReadAllMeshNodes(Scene->GetRootNode(), lOutMeshArray);


	UStaticMesh* StaticMesh = new UStaticMesh();

	if ((int32)StaticMesh->SourceModels.size() < LODIndex + 1)
	{
		// Add one LOD 
		StaticMesh->AddSourceModel();

		if ((int32)StaticMesh->SourceModels.size() < LODIndex + 1)
		{
			LODIndex = StaticMesh->SourceModels.size() - 1;
		}
	}

	FMeshDescription* MeshDescription = StaticMesh->GetOriginalMeshDescription(LODIndex);
	FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
	std::vector<FFbxMaterial> MeshMaterials;
	for (int32 MeshIndex = 0; MeshIndex < (int)lOutMeshArray.size(); MeshIndex++)
	{
		FbxNode* Node = lOutMeshArray[MeshIndex];

		if (Node->GetMesh())
		{
			if (!BuildStaticMeshFromGeometry(Node, StaticMesh, MeshMaterials, LODIndex))
			{
				break;
			}
		}
	}
	return StaticMesh;
}



bool FFbxImporter::BuildStaticMeshFromGeometry(FbxNode* Node, UStaticMesh* StaticMesh, std::vector<FFbxMaterial>& MeshMaterials, int LODIndex)
{
	FbxMesh* Mesh = Node->GetMesh();
	FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];

	FMeshDescription* MeshDescription = StaticMesh->GetOriginalMeshDescription(LODIndex);
	//The mesh description should have been created before calling BuildStaticMeshFromGeometry
	//check(MeshDescription);
	//remove the bad polygons before getting any data from mesh
	Mesh->RemoveBadPolygons();

	//Get the base layer of the mesh
	FbxLayer* BaseLayer = Mesh->GetLayer(0);

	int32 MaterialCount = 0;

	MaterialCount = Node->GetMaterialCount();

	int32 MaterialIndexOffset = (int32)MeshMaterials.size();

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
	{
		MeshMaterials.push_back(FFbxMaterial());
		FFbxMaterial* NewMaterial = &MeshMaterials.back();
		FbxSurfaceMaterial *FbxMaterial = Node->GetMaterial(MaterialIndex);
		NewMaterial->FbxMaterial = FbxMaterial;
	}
	int32 LayerSmoothingCount = Mesh->GetLayerCount(FbxLayerElement::eSmoothing);
	for (int32 i = 0; i < LayerSmoothingCount; i++)
	{
		FbxLayerElementSmoothing const* SmoothingInfo = Mesh->GetLayer(0)->GetSmoothing();
		if (SmoothingInfo && SmoothingInfo->GetMappingMode() != FbxLayerElement::eByPolygon)
		{
			GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh, i);
		}
	}

	if (!Mesh->IsTriangleMesh())
	{
		const bool bReplace = true;
		FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(Mesh, bReplace);

		if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Mesh = (FbxMesh*)ConvertedNode;
		}
	}

	BaseLayer = Mesh->GetLayer(0);

	//
	//	get the "material index" layer.  Do this AFTER the triangulation step as that may reorder material indices
	//
	FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
	FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ? LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;

	bool bSmoothingAvailable = false;

	FbxLayerElementSmoothing* SmoothingInfo = BaseLayer->GetSmoothing();
	FbxLayerElement::EReferenceMode SmoothingReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode SmoothingMappingMode(FbxLayerElement::eByEdge);
	if (SmoothingInfo)
	{
		if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
		{
			//Convert the base layer to edge smoothing
			GeometryConverter->ComputeEdgeSmoothingFromPolygonSmoothing(Mesh, 0);
			BaseLayer = Mesh->GetLayer(0);
			SmoothingInfo = BaseLayer->GetSmoothing();
		}

		if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByEdge)
		{
			bSmoothingAvailable = true;
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

	//
	// get the first normal layer
	//
	FbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
	FbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
	FbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

	//whether there is normal, tangent and binormal data in this mesh
	bool bHasNTBInformation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

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

	//bool bImportedCollision = ImportCollisionModels(StaticMesh, GetNodeNameWithoutNamespace(Node));

	//If we import a collision or we "generate one and remove the degenerates triangles" we will automatically set the section collision boolean.
	//bool bEnableCollision = bImportedCollision || (GBuildStaticMeshCollision && LODIndex == 0 && ImportOptions->bRemoveDegenerates);
	for (int32 SectionIndex = MaterialIndexOffset; SectionIndex < MaterialIndexOffset + MaterialCount; SectionIndex++)
	{
		FMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, SectionIndex);

		Info.bEnableCollision = false;// bEnableCollision;
		//Make sure LOD greater then 0 copy the LOD 0 sections collision flags
		if (LODIndex != 0)
		{
			//Match the material slot index
			for (int32 LodZeroSectionIndex = 0; LodZeroSectionIndex < StaticMesh->SectionInfoMap.GetSectionNumber(0); ++LodZeroSectionIndex)
			{
				FMeshSectionInfo InfoLodZero = StaticMesh->SectionInfoMap.Get(0, LodZeroSectionIndex);
				if (InfoLodZero.MaterialIndex == Info.MaterialIndex)
				{
					Info.bEnableCollision = InfoLodZero.bEnableCollision;
					Info.bCastShadow = InfoLodZero.bCastShadow;
					break;
				}
			}
		}
		StaticMesh->SectionInfoMap.Set(LODIndex, SectionIndex, Info);
	}

	FbxAMatrix TotalMatrix;
	FbxAMatrix TotalMatrixForNormal;
	TotalMatrix = ComputeTotalMatrix(Node);
	TotalMatrixForNormal = TotalMatrix.Inverse();
	TotalMatrixForNormal = TotalMatrixForNormal.Transpose();
	int32 PolygonCount = Mesh->GetPolygonCount();

	int32 VertexCount = Mesh->GetControlPointsCount();
	bool OddNegativeScale = IsOddNegativeScale(TotalMatrix);
	bool bHasNonDegeneratePolygons = false;
	TVertexAttributeArray<FVector>& VertexPositions = MeshDescription->VertexAttributes().GetAttributes<FVector>(MeshAttribute::Vertex::Position);

	TVertexInstanceAttributeArray<FVector>& VertexInstanceNormals = MeshDescription->VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Normal);
	TVertexInstanceAttributeArray<FVector>& VertexInstanceTangents = MeshDescription->VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Tangent);
	TVertexInstanceAttributeArray<float>& VertexInstanceBinormalSigns = MeshDescription->VertexInstanceAttributes().GetAttributes<float>(MeshAttribute::VertexInstance::BinormalSign);
	TVertexInstanceAttributeArray<FVector4>& VertexInstanceColors = MeshDescription->VertexInstanceAttributes().GetAttributes<FVector4>(MeshAttribute::VertexInstance::Color);
	TVertexInstanceAttributeIndicesArray<FVector2D>& VertexInstanceUVs = MeshDescription->VertexInstanceAttributes().GetAttributesSet<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate);

	TEdgeAttributeArray<bool>& EdgeHardnesses = MeshDescription->EdgeAttributes().GetAttributes<bool>(MeshAttribute::Edge::IsHard);
	TEdgeAttributeArray<float>& EdgeCreaseSharpnesses = MeshDescription->EdgeAttributes().GetAttributes<float>(MeshAttribute::Edge::CreaseSharpness);

	TPolygonGroupAttributeArray<std::string>& PolygonGroupImportedMaterialSlotNames = MeshDescription->PolygonGroupAttributes().GetAttributes<std::string>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName);

	int32 VertexOffset = MeshDescription->Vertices().Num();
	int32 VertexInstanceOffset = MeshDescription->VertexInstances().Num();
	int32 PolygonOffset = MeshDescription->Polygons().Num();

	for (int32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
	{
		int32 RealVertexIndex = VertexOffset + VertexIndex;
		FbxVector4 FbxPosition = Mesh->GetControlPoints()[VertexIndex];
		FbxPosition = TotalMatrix.MultT(FbxPosition);
		const FVector VertexPosition = Converter.ConvertPos(FbxPosition);

		FVertexID AddedVertexId = MeshDescription->CreateVertex();
		VertexPositions[AddedVertexId] = VertexPosition;
		if (AddedVertexId.GetValue() != RealVertexIndex)
		{
			//AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_CannotCreateVertex", "Cannot create valid vertex for mesh '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::StaticMesh_BuildError);
			return false;
		}
	}
	Mesh->BeginGetMeshEdgeIndexForPolygon();
	int32 CurrentVertexInstanceIndex = 0;
	int32 SkippedVertexInstance = 0;
	for (int32 PolygonIndex = 0; PolygonIndex < PolygonCount; PolygonIndex++)
	{
		int32 PolygonVertexCount = Mesh->GetPolygonSize(PolygonIndex);
		//Verify if the polygon is degenerate, in this case do not add them
		{
			float ComparisonThreshold = ImportOptions->bRemoveDegenerates ? SMALL_NUMBER : 0.0f;
			std::vector<FVector> P;
			P.resize(PolygonVertexCount);
			for (int32 CornerIndex = 0; CornerIndex < PolygonVertexCount; CornerIndex++)
			{
				const int32 ControlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, CornerIndex);
				const FVertexID VertexID(VertexOffset + ControlPointIndex);
				P[CornerIndex] = VertexPositions[VertexID];
			}
			//check(P.Num() > 2); //triangle is the smallest polygon we can have
			const FVector Normal = ((P[1] - P[2]) ^ (P[0] - P[2])).GetSafeNormal(ComparisonThreshold);
			//Check for degenerated polygons, avoid NAN
			if (Normal.IsNearlyZero(ComparisonThreshold))
			{
				SkippedVertexInstance += PolygonVertexCount;
				continue;
			}
		}

		int32 RealPolygonIndex = PolygonOffset + PolygonIndex;
		std::vector<FVertexInstanceID> CornerInstanceIDs;
		CornerInstanceIDs.resize(PolygonVertexCount);
		std::vector<FVertexID> CornerVerticesIDs;
		CornerVerticesIDs.resize(PolygonVertexCount);
		for (int32 CornerIndex = 0; CornerIndex < PolygonVertexCount; CornerIndex++, CurrentVertexInstanceIndex++)
		{
			int32 VertexInstanceIndex = VertexInstanceOffset + CurrentVertexInstanceIndex;
			int32 RealFbxVertexIndex = SkippedVertexInstance + CurrentVertexInstanceIndex;
			const FVertexInstanceID VertexInstanceID(VertexInstanceIndex);
			CornerInstanceIDs[CornerIndex] = VertexInstanceID;
			const int32 ControlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, CornerIndex);
			const FVertexID VertexID(VertexOffset + ControlPointIndex);
			const FVector VertexPosition = VertexPositions[VertexID];
			CornerVerticesIDs[CornerIndex] = VertexID;

			FVertexInstanceID AddedVertexInstanceId = MeshDescription->CreateVertexInstance(VertexID);

			check(AddedVertexInstanceId == VertexInstanceID);

			if (LayerElementNormal)
			{
				//normals may have different reference and mapping mode than tangents and binormals
				int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ?
					ControlPointIndex : RealFbxVertexIndex;
				int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ?
					NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);

				FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
				TempValue = TotalMatrixForNormal.MultT(TempValue);
				FVector TangentZ = Converter.ConvertDir(TempValue);
				VertexInstanceNormals[AddedVertexInstanceId] = TangentZ.GetSafeNormal();
				//tangents and binormals share the same reference, mapping mode and index array
				if (bHasNTBInformation)
				{
					int TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ?
						ControlPointIndex : RealFbxVertexIndex;
					int TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ?
						TangentMapIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMapIndex);

					TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentX = Converter.ConvertDir(TempValue);
					VertexInstanceTangents[AddedVertexInstanceId] = TangentX.GetSafeNormal();

					int BinormalMapIndex = (BinormalMappingMode == FbxLayerElement::eByControlPoint) ?
						ControlPointIndex : RealFbxVertexIndex;
					int BinormalValueIndex = (BinormalReferenceMode == FbxLayerElement::eDirect) ?
						BinormalMapIndex : LayerElementBinormal->GetIndexArray().GetAt(BinormalMapIndex);

					TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentY = -Converter.ConvertDir(TempValue);
					VertexInstanceBinormalSigns[AddedVertexInstanceId] = GetBasisDeterminantSign(TangentX.GetSafeNormal(), TangentY.GetSafeNormal(), TangentZ.GetSafeNormal());
				}
			}
		}
	}
	
	Mesh->EndGetMeshEdgeIndexForPolygon();
	return true;
}

FbxAMatrix FFbxImporter::ComputeTotalMatrix(FbxNode* Node)
{
	FbxAMatrix Geometry;
	FbxVector4 Translation, Rotation, Scaling;
	Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	//For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
	FbxAMatrix& GlobalTransform = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);

	//We can bake the pivot only if we don't transform the vertex to the absolute position
	if (!ImportOptions->bTransformVertexToAbsolute)
	{
		if (ImportOptions->bBakePivotInVertex)
		{
			FbxAMatrix PivotGeometry;
			FbxVector4 RotationPivot = Node->GetRotationPivot(FbxNode::eSourcePivot);
			FbxVector4 FullPivot;
			FullPivot[0] = -RotationPivot[0];
			FullPivot[1] = -RotationPivot[1];
			FullPivot[2] = -RotationPivot[2];
			PivotGeometry.SetT(FullPivot);
			Geometry = Geometry * PivotGeometry;
		}
		else
		{
			//No Vertex transform and no bake pivot, it will be the mesh as-is.
			Geometry.SetIdentity();
		}
	}
	//We must always add the geometric transform. Only Max use the geometric transform which is an offset to the local transform of the node
	FbxAMatrix TotalMatrix = ImportOptions->bTransformVertexToAbsolute ? GlobalTransform * Geometry : Geometry;

	return TotalMatrix;
}


bool FFbxImporter::IsOddNegativeScale(FbxAMatrix& TotalMatrix)
{
	FbxVector4 Scale = TotalMatrix.GetS();
	int32 NegativeNum = 0;

	if (Scale[0] < 0) NegativeNum++;
	if (Scale[1] < 0) NegativeNum++;
	if (Scale[2] < 0) NegativeNum++;

	return NegativeNum == 1 || NegativeNum == 3;
}

