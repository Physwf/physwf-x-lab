#include "primitives.h"
#include "fbxsdk.h"

#include <cassert>
#include <fstream>

using namespace DirectX;

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

void Mesh::LoadObj(const char* Filename)
{
	ObjLoader Loader;
	if (Loader.Load(Filename))
	{

	}
}

std::string get_filename(const std::string& fullfilename, std::string& DirName)
{
	std::string::size_type pos1 = fullfilename.rfind('/');
	if (pos1 != std::string::npos)
	{
		DirName = fullfilename.substr(0, pos1);
		return fullfilename.substr(pos1);
	}
	else
	{
		DirName = ".";
		return fullfilename;
	}
}

bool string_startwith(const std::string& str, const char* start)
{
	auto start_len = strlen(start);
	if (str.length() < start_len) return false;
	for (std::string::size_type i = 0; i < start_len; ++i)
	{
		if (str[i] == start[i]) continue;
		return false;
	}
	return true;
}

bool string_split(const std::string& str, const std::string& dilimer, std::vector<std::string>& split_result)
{
	std::string::size_type pos1, pos2;
	pos2 = str.find(dilimer);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		split_result.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + dilimer.size();
		pos2 = str.find(dilimer, pos1);
	}
	if (pos1 != str.length())
		split_result.push_back(str.substr(pos1));
	return split_result.size() > 0;
}

bool ObjLoader::Load(const char* filename)
{
	get_filename(filename, ResourceDir);

	std::string line;
	std::ifstream fs(filename);
	if (fs)
	{
		while (std::getline(fs, line))
		{
			if (line.length() == 0) continue;
			if (!ParseLine(line)) return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool ObjLoader::ParseMaterial(const char* filename)
{
	std::string line;
	std::ifstream fs(ResourceDir + "/" + filename);
	if (fs)
	{
		while (std::getline(fs, line))
		{
			if (line.length() == 0) continue;
			if (!ParseLine(line)) return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool ObjLoader::ParseLine(const std::string& line)
{
	if (line.length() <= 0) return false;
	if (string_startwith(line, "#"))
	{
	}
	else if (string_startwith(line, "mtllib "))
	{
		std::string filename;
		if (!ParseName(line, filename)) return false;
		if (!ParseMaterial(filename.c_str())) return false;
	}
	else if (string_startwith(line, "g "))
	{
		std::string GroupName;
		if (!ParseName(line, GroupName)) return false;
		if (GroupName != "default")
		{
			SubMeshes[GroupName] = SubMesh();
			CurrentSubMesh = &SubMeshes[GroupName];
		}
	}
	else if (string_startwith(line, "usemtl"))
	{
		std::string MaterialName;
		if (!ParseName(line, MaterialName)) return false;
		CurrentSubMesh->MaterialName = MaterialName;
	}
	else if (string_startwith(line, "v "))
	{
		XMFLOAT3 Position;
		if (!ParsePosition(line, Position)) return false;
		Positions.push_back(Position);
	}
	else if (string_startwith(line, "vn "))
	{
		XMFLOAT3 Normal;
		if (!ParseNormal(line, Normal)) return false;
		Normals.push_back(Normal);
	}
	else if (string_startwith(line, "vt "))
	{
		XMFLOAT2 TexCoord;
		if (!ParseTextCoord2D(line, TexCoord)) return false;
		TexCoords.push_back(TexCoord);
	}
	else if (string_startwith(line, "f "))
	{
		Face F;
		if (!ParseFace(line, F)) return false;
		AssembleSubMesh(CurrentSubMesh, F);
	}
	else if (string_startwith(line, "newmtl "))
	{
		std::string MaterialName;
		if (!ParseName(line, MaterialName)) return false;
		Materials[MaterialName] = ObjMaterial();
		CurrentMaterial = &Materials[MaterialName];
	}
	else if (string_startwith(line, "Kd "))
	{
		XMFLOAT3 Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Kd = Color;
	}
	else if (string_startwith(line, "Ka "))
	{
		XMFLOAT3 Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Ka = Color;
	}
	else if (string_startwith(line, "Tf "))
	{
		XMFLOAT3 Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Tf = Color;
	}
	else if (string_startwith(line, "Ni "))
	{
		float Value;
		if (!ParseScalar(line, Value)) return false;
		CurrentMaterial->Ni = Value;
	}
	else if (string_startwith(line, "Ks "))
	{
		XMFLOAT3 Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Ks = Color;
	}
	else if (string_startwith(line, "Ns "))
	{
		float Value;
		if (!ParseScalar(line, Value)) return false;
		CurrentMaterial->Ns = Value;
	}
	else if (string_startwith(line, "map_Kd "))
	{
		std::string FileName;
		if (!ParseName(line, FileName)) return false;
		//CurrentMaterial->map_Kd = ResourceDir + "/" + FileName;
		//Texture2D* tex = new Texture2D();
		//tex->LoadFromJpg(CurrentMaterial->map_Kd.c_str());
		//tex->UpdateRHI();
		//Textures[CurrentMaterial->map_Kd] = tex;
	}
	else if (string_startwith(line, "map_Ks "))
	{
		std::string FileName;
		if (!ParseName(line, FileName)) return false;
		//CurrentMaterial->map_Ks = ResourceDir + "/" + FileName;
		//Texture2D* tex = new Texture2D();
		//tex->LoadFromJpg(CurrentMaterial->map_Ks.c_str());
		//tex->UpdateRHI();
		//Textures[CurrentMaterial->map_Ks] = tex;
	}
	return true;
}

bool ObjLoader::ParseName(const std::string& line, std::string& Name)
{
	std::string::size_type pos1 = line.find(' ');
	if (pos1 != std::string::npos)
	{
		Name = line.substr(pos1 + 1);
		return Name.size() > 0;
	}
	return false;
}

bool ObjLoader::ParsePosition(const std::string& line, XMFLOAT3& Position)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Position.x = (float)std::atof(result[1].c_str());
			Position.y = (float)std::atof(result[2].c_str());
			Position.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}


bool ObjLoader::ParseNormal(const std::string& line, XMFLOAT3& Normal)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Normal.x = (float)std::atof(result[1].c_str());
			Normal.y = (float)std::atof(result[2].c_str());
			Normal.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}

bool ObjLoader::ParseTextCoord2D(const std::string& line, XMFLOAT2& TexCoord)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 2)
		{
			TexCoord.x = (float)std::atof(result[1].c_str());
			TexCoord.y = (float)std::atof(result[2].c_str());
			return true;
		}
	}
	return false;
}

bool ObjLoader::ParseColor(const std::string& line, XMFLOAT3& Color)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Color.x = (float)std::atof(result[1].c_str());
			Color.y = (float)std::atof(result[2].c_str());
			Color.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}


bool ObjLoader::ParseScalar(const std::string& line, float& Value)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 1)
		{
			Value = (float)std::atof(result[1].c_str());
			return true;
		}
	}
	return false;
}

bool ObjLoader::ParseFace(const std::string& line, Face& F)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			VertexIndex Index;
			F.VerticesCount = 0;
			if (!ParseVertexIndex(result[1], Index)) return false;
			F.v1 = Index;
			++F.VerticesCount;
			if (!ParseVertexIndex(result[2], Index)) return false;
			F.v2 = Index;
			++F.VerticesCount;
			if (!ParseVertexIndex(result[3], Index)) return false;
			F.v3 = Index;
			++F.VerticesCount;
			if (result.size() > 4)
			{
				if (!ParseVertexIndex(result[4], Index)) return false;
				F.v4 = Index;
				++F.VerticesCount;
			}
			return true;
		}
	}
	return false;
}

bool ObjLoader::ParseVertexIndex(const std::string& line, VertexIndex& Index)
{
	std::vector<std::string> result;
	if (string_split(line, "/", result))
	{
		if (result.size() >= 3)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.TexCoordIndex = std::atoi(result[1].c_str());
			Index.NormalIndex = std::atoi(result[2].c_str());
			return true;
		}
		else if (result.size() >= 2)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.TexCoordIndex = std::atoi(result[1].c_str());
			return true;
		}
	}
	return false;
}

void ObjLoader::AssembleSubMesh(SubMesh* pSubMesh, const Face& F)
{
	if (pSubMesh)
	{
		MeshVertex V1, V2, V3, V4;
		V1.Position = Positions[F.v1.PositonIndex - 1];
		V2.Position = Positions[F.v2.PositonIndex - 1];
		V3.Position = Positions[F.v3.PositonIndex - 1];
		if (F.VerticesCount == 4)
		{
			V4.Position = Positions[F.v4.PositonIndex - 1];
		}

		if (TexCoords.size() > 0)
		{
			V1.UV = TexCoords[F.v1.TexCoordIndex - 1];
			V2.UV = TexCoords[F.v2.TexCoordIndex - 1];
			V3.UV = TexCoords[F.v3.TexCoordIndex - 1];
			if (F.VerticesCount == 4)
			{
				V4.UV = TexCoords[F.v4.TexCoordIndex - 1];
			}
		}
		if (Normals.size() > 0)
		{
			V1.Normal = Normals[F.v1.NormalIndex - 1];
			V2.Normal = Normals[F.v2.NormalIndex - 1];
			V3.Normal = Normals[F.v3.NormalIndex - 1];
			if (F.VerticesCount == 4)
			{
				V4.Normal = Normals[F.v4.NormalIndex - 1];
			}
		}
		else
		{
			//Normal = normalize(Normal);
			//XMLoadFloat3(&V3.Position), XMLoadFloat3(&V1.Position);
			//V1.Normal = Normal;
			//V2.Normal = Normal;
			//V3.Normal = Normal;

			XMVECTOR Normal = XMVectorMultiply(XMVectorSubtract(XMLoadFloat3(&V3.Position), XMLoadFloat3(&V1.Position)), XMVectorSubtract(XMLoadFloat3(&V2.Position), XMLoadFloat3(&V1.Position)));
			Normal = XMVector3Normalize(Normal);
			XMStoreFloat3(&V1.Normal, Normal);
			XMStoreFloat3(&V2.Normal, Normal);
			XMStoreFloat3(&V3.Normal, Normal);

			if (F.VerticesCount == 4)
			{
				XMStoreFloat3(&V4.Normal, Normal);
				//V4.Normal = Normal;
			}
		}

		pSubMesh->Vertices.push_back(V1);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		pSubMesh->Vertices.push_back(V2);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		pSubMesh->Vertices.push_back(V3);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);

		if (F.VerticesCount == 4)
		{
			pSubMesh->Vertices.push_back(V1);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
			pSubMesh->Vertices.push_back(V3);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
			pSubMesh->Vertices.push_back(V4);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		}
	}
}

