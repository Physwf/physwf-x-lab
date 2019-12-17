#include "StaticMesh.h"

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

FMeshDescription* UStaticMesh::GetOriginalMeshDescription(int32 LodIndex)
{
	return nullptr;
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

void FFbxImporter::ParseVertexPosition(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter)
{
	FbxVector4* ContorlPoints = pMesh->GetControlPoints();
	FbxVector4 Position = ContorlPoints[ControlPointIndex];
}

void FFbxImporter::ParseVertexColor(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter)
{
	int VertexColorCount = pMesh->GetElementVertexColorCount();
	if (VertexColorCount <= 0) return;

	FbxLayerElementVertexColor* VertexColors = pMesh->GetElementVertexColor();

	switch (VertexColors->GetMappingMode())
	{
	case FbxLayerElement::eByControlPoint:
	{
		switch (VertexColors->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxColor Color = VertexColors->GetDirectArray().GetAt(ControlPointIndex);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexColors->GetIndexArray().GetAt(ControlPointIndex);
			FbxColor Color = VertexColors->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	case FbxLayerElement::eByPolygonVertex:
	{
		switch (VertexColors->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxColor Color = VertexColors->GetDirectArray().GetAt(VertexCounter);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			 int Index = VertexColors->GetIndexArray().GetAt(VertexCounter);
			 FbxColor Color = VertexColors->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	}
}

void FFbxImporter::ParseVertexNormal(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter)
{
	if (pMesh->GetElementNormalCount() <= 0) return;

	FbxLayerElementNormal* VertexNormals = pMesh->GetElementNormal();

	switch (VertexNormals->GetMappingMode())
	{
	case FbxLayerElement::eByControlPoint:
	{
		switch (VertexNormals->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexNormals->GetDirectArray().GetAt(ControlPointIndex);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index =	VertexNormals->GetIndexArray().GetAt(ControlPointIndex);
			FbxVector4 Normal = VertexNormals->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	case FbxLayerElement::eByPolygonVertex:
	{
		switch (VertexNormals->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexNormals->GetDirectArray().GetAt(VertexCounter);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexNormals->GetIndexArray().GetAt(VertexCounter);
			FbxVector4 Normal = VertexNormals->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	}
}

void FFbxImporter::ParseVertexBinormal(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter)
{
	if (pMesh->GetElementBinormalCount() <= 0) return;

	FbxLayerElementBinormal* VertexBinormals = pMesh->GetElementBinormal();

	switch (VertexBinormals->GetMappingMode())
	{
	case FbxLayerElement::eByControlPoint:
	{
		switch (VertexBinormals->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexBinormals->GetDirectArray().GetAt(ControlPointIndex);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexBinormals->GetIndexArray().GetAt(ControlPointIndex);
			FbxVector4 Normal = VertexBinormals->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	case FbxLayerElement::eByPolygonVertex:
	{
		switch (VertexBinormals->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexBinormals->GetDirectArray().GetAt(VertexCounter);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexBinormals->GetIndexArray().GetAt(VertexCounter);
			FbxVector4 Normal = VertexBinormals->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	}
}

void FFbxImporter::ParseVertexTangent(FbxMesh* pMesh, int ControlPointIndex, int VertexCounter)
{
	if (pMesh->GetElementTangentCount() <= 0) return;

	FbxLayerElementTangent* VertexTangents = pMesh->GetElementTangent();

	switch (VertexTangents->GetMappingMode())
	{
	case FbxLayerElement::eByControlPoint:
	{
		switch (VertexTangents->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexTangents->GetDirectArray().GetAt(ControlPointIndex);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexTangents->GetIndexArray().GetAt(ControlPointIndex);
			FbxVector4 Normal = VertexTangents->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	case FbxLayerElement::eByPolygonVertex:
	{
		switch (VertexTangents->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:
		{
			FbxVector4 Normal = VertexTangents->GetDirectArray().GetAt(VertexCounter);
			break;
		}
		case FbxLayerElement::eIndex:
		{
			break;
		}
		case FbxLayerElement::eIndexToDirect:
		{
			int Index = VertexTangents->GetIndexArray().GetAt(VertexCounter);
			FbxVector4 Normal = VertexTangents->GetDirectArray().GetAt(Index);
			break;
		}
		}
		break;
	}
	}
}

void FFbxImporter::ImportStaticMeshAsSingle(const char* InFileName, UStaticMesh* OutMesh)
{
	FbxManager* lFbxManager = FbxManager::Create();

	FbxIOSettings* lIOSetting = FbxIOSettings::Create(lFbxManager, IOSROOT);
	lFbxManager->SetIOSettings(lIOSetting);

	FbxImporter* lImporter = FbxImporter::Create(lFbxManager, "MeshImporter");
	if (!lImporter->Initialize(InFileName, -1, lFbxManager->GetIOSettings()))
	{
		//X_LOG("Call to FbxImporter::Initialize() failed.\n");
		//X_LOG("Error returned: %s \n\n", lImporter->GetStatus().GetErrorString());
		std::string error = lImporter->GetStatus().GetErrorString();
		return;
	}
	FbxScene* lScene = FbxScene::Create(lFbxManager, "Mesh");
	lImporter->Import(lScene);

	//读出所有Mesh节点
	std::vector<FbxNode*> lOutMeshArray;
	ReadAllMeshNodes(lScene->GetRootNode(), lOutMeshArray);

	FbxGeometryConverter* GeometryConverter = new FbxGeometryConverter(lFbxManager);
	for (std::vector<FbxNode*>::size_type Index = 0; Index < lOutMeshArray.size(); ++Index)
	{
		FbxNode* lNode = lOutMeshArray[Index];
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
					//X_LOG("Cant triangulate!");
				}
			}


			int PolygonCount = lMesh->GetPolygonCount();
			int VertexCounter = 0;
			for (int i = 0; i < PolygonCount; ++i)
			{
				for (int j = 0; j < lMesh->GetPolygonSize(i); ++i)
				{
					int ContorlPointIndex = lMesh->GetPolygonVertex(i, j);
					ParseVertexPosition(lMesh, ContorlPointIndex, VertexCounter);
					ParseVertexColor(lMesh, ContorlPointIndex, VertexCounter);
					ParseVertexNormal(lMesh, ContorlPointIndex, VertexCounter);
					ParseVertexBinormal(lMesh, ContorlPointIndex, VertexCounter);
					ParseVertexTangent(lMesh, ContorlPointIndex, VertexCounter);
					++VertexCounter;
				}
			}
		}
	}
}



void FObjImporter::Import(const char* InFileName, UStaticMesh* OutMesh)
{

}
