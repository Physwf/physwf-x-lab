#include "primitives.h"
#include "fbxsdk.h"

#include <cassert>

void FillFbxArray(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray)
{
	if (pNode->GetMesh())
	{
		pOutMeshArray.push_back(pNode);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		FillFbxArray(pNode->GetChild(i), pOutMeshArray);
	}
}

FbxAMatrix ComputeTotalMatrix(FbxScene* pScence, FbxNode* pNode, bool bTransformVertexToAbsolute, bool bBakePivotInVertex)
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

XMFLOAT3 ConvertPos(FbxVector4 pPos)
{
	XMFLOAT3 Result;
	Result.x = (float)pPos[0];
	Result.y = -(float)pPos[1];
	Result.z = (float)pPos[2];
	return Result;
}

XMFLOAT3 ConvertDir(FbxVector4 Vec)
{
	XMFLOAT3 Result;
	Result.x = (float)Vec[0];
	Result.y = -(float)Vec[1];
	Result.z = (float)Vec[2];
	return Result;
}

void Mesh::LoadFBX(const char* pFileName)
{
	FbxManager* lFbxManager = FbxManager::Create();

	FbxIOSettings* lIOSetting = FbxIOSettings::Create(lFbxManager, IOSROOT);
	lFbxManager->SetIOSettings(lIOSetting);

	FbxImporter* lImporter = FbxImporter::Create(lFbxManager, "MeshImporter");
	if (!lImporter->Initialize(pFileName, -1, lFbxManager->GetIOSettings()))
	{
		assert("Call to FbxImporter::Initialize() failed.\n");
		//X_LOG("Error returned: %s \n\n", lImporter->GetStatus().GetErrorString());
		return;
	}

	FbxScene* lScene = FbxScene::Create(lFbxManager, "Mesh");
	lImporter->Import(lScene);
	lImporter->Destroy();

// 	FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eRightHanded;
// 	FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eZAxis;
// 	FbxAxisSystem::EFrontVector FrontVector = (FbxAxisSystem::EFrontVector) - FbxAxisSystem::eParityOdd;

	FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eLeftHanded;
	FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eYAxis;
	FbxAxisSystem::EFrontVector FrontVector = (FbxAxisSystem::EFrontVector) - FbxAxisSystem::eParityOdd;

	FbxAxisSystem ImportAxis(UpVector, FrontVector, CoordSystem);
	FbxAxisSystem SourceSetup = lScene->GetGlobalSettings().GetAxisSystem();

	if (SourceSetup != ImportAxis)
	{
		FbxRootNodeUtility::RemoveAllFbxRoots(lScene);
		ImportAxis.ConvertScene(lScene);
// 		FbxAMatrix JointOrientationMatrix;
// 		JointOrientationMatrix.SetIdentity();
// 		if (GetImportOptions()->bForceFrontXAxis)
// 		{
// 			JointOrientationMatrix.SetR(FbxVector4(-90.0, -90.0, 0.0));
// 		}
		//FFbxDataConverter::SetJointPostConversionMatrix(JointOrientationMatrix);
	}

	std::vector<FbxNode*> lOutMeshArray;
	FillFbxArray(lScene->GetRootNode(), lOutMeshArray);

	FbxGeometryConverter* GeometryConverter = new FbxGeometryConverter(lFbxManager);

	std::vector<FbxSurfaceMaterial*> MeshMaterials;

	std::vector<XMFLOAT3> VertexPositions;
	std::vector<MeshVertex>& VertexIntances = Vertices;
	std::vector<int>& SingleIndices = Indices;

	for (int Index = 0; Index < (int)lOutMeshArray.size(); ++Index)
	{
		FbxNode* lNode = lOutMeshArray[Index];

		int MaterialCount = lNode->GetMaterialCount();
		int MaterialIndexOffset = (int)MeshMaterials.size();
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
		{
			MeshMaterials.push_back(lNode->GetMaterial(MaterialIndex));
		}


		if (lNode->GetMesh())
		{
			FbxMesh* lMesh = lNode->GetMesh();

			lMesh->RemoveBadPolygons();

			if (!lMesh->IsTriangleMesh())
			{
				FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(lMesh, true);

				if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					lMesh = ConvertedNode->GetNode()->GetMesh();
				}
				else
				{
					assert("Cant triangulate!");
				}
			}

			FbxLayer* BaseLayer = lMesh->GetLayer(0);

			//FBXUVs fbxUVs(lMesh);

			FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
			FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ? LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;

			//fbxUVs.Phase2(lMesh);

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


			int VertexCount = lMesh->GetControlPointsCount();
			int VertexOffset = (int)VertexPositions.size();
			int VertexInstanceOffset = VertexIntances.size();
			//int PolygonOffset = MD.Polygons().Num();
			for (auto VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
			{
				int RealVertexIndex = VertexOffset + VertexIndex;
				FbxVector4 FbxPosition = lMesh->GetControlPoints()[VertexIndex];
				FbxPosition = TotalMatrix.MultT(FbxPosition);
				XMFLOAT3 const VectorPositon = ConvertPos(FbxPosition);
				int VertexID = (int)VertexPositions.size();
				VertexPositions.push_back(VectorPositon);
			}

			int PolygonCount = lMesh->GetPolygonCount();
			int CurrentVertexInstanceIndex = 0;
			int SkippedVertexInstance = 0;

			lMesh->BeginGetMeshEdgeIndexForPolygon();

			for (auto PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
			{
				int PolygonVertexCount = lMesh->GetPolygonSize(PolygonIndex);
				std::vector<XMFLOAT3> P;
				P.resize(PolygonVertexCount, XMFLOAT3());
				for (int CornerIndex = 0; CornerIndex < PolygonVertexCount; CornerIndex++)
				{
					const int ControlPointIndex = lMesh->GetPolygonVertex(PolygonIndex, CornerIndex);
					const int VertexID = VertexOffset + ControlPointIndex;
					P[CornerIndex] = VertexPositions[VertexID];
				}


				for (auto CornerIndex = 0; CornerIndex < PolygonVertexCount; ++CornerIndex, CurrentVertexInstanceIndex++)
				{
					int VertexInstanceIndex = VertexInstanceOffset + CurrentVertexInstanceIndex;
					int RealFbxVertexIndex = SkippedVertexInstance + CurrentVertexInstanceIndex;

					const int ControlPointIndex = lMesh->GetPolygonVertex(PolygonIndex, CornerIndex);
					const int VertexID = VertexOffset + ControlPointIndex;
					MeshVertex Vertex;
					Vertex.Position = VertexPositions[VertexID];


					if (LayerElementNormal)
					{
						int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
						int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);
						FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
						TempValue = TotalMatrixForNormal.MultT(TempValue);
						XMFLOAT3 TangentZ = ConvertDir(TempValue);
						Vertex.Normal = TangentZ;
						//VertexInstanceNormals[AddedVertexInstanceId] = TangentZ;

						if (bHasNTBInfomation)
						{
							int TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
							int TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ? TangentMapIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMapIndex);

							TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);
							TempValue = TotalMatrixForNormal.MultT(TempValue);
							XMFLOAT3 TangentX = ConvertDir(TempValue);
							//VertexInstanceTangents[AddedVertexInstanceId] = TangentX;
							Vertex.Tangent.x = TangentX.x;
							Vertex.Tangent.y = TangentX.y;
							Vertex.Tangent.z = TangentX.z;

							int BinormalMapIndex = (BinormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : RealFbxVertexIndex;
							int BinormalValueIndex = (BinormalReferenceMode == FbxLayerElement::eDirect) ? BinormalMapIndex : LayerElementBinormal->GetIndexArray().GetAt(BinormalMapIndex);

							TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
							TempValue = TotalMatrixForNormal.MultT(TempValue);
							XMFLOAT3 TangentY = ConvertDir(TempValue);
							TangentY.x *= -1;
							TangentY.y *= -1;
							TangentY.z *= -1;
							//VertexInstanceBinormalSigns[AddedVertexInstanceId] = GetBasisDeterminantSign(TangentX.GetSafeNormal(), TangentY.GetSafeNormal(), TangentZ.GetSafeNormal());
							XMFLOAT3X3 M(
								TangentX.x, TangentX.y, TangentX.z,
								TangentY.x, TangentY.y, TangentY.z,
								TangentZ.x, TangentZ.y, TangentZ.z
								);
							XMFLOAT3 Dest;
							XMStoreFloat3(&Dest, XMMatrixDeterminant(XMLoadFloat3x3(&M)));
							Vertex.Tangent.w = Dest.x > 0 ? 1.f : -1.f;
						}
					}


					VertexIntances.push_back(Vertex);
					SingleIndices.push_back(VertexIntances.size() - 1);
				}
			}

			
		}
	}
}

