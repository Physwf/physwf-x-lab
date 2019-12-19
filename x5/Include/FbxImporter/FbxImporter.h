#pragma once

#include "StaticMesh.h"

struct FBXImportOptions
{
	// General options
	bool bCanShowDialog;
	bool bImportScene;
	bool bImportMaterials;
	bool bInvertNormalMap;
	bool bImportTextures;
	bool bImportLOD;
	bool bUsedAsFullName;
	bool bConvertScene;
	bool bForceFrontXAxis;
	bool bConvertSceneUnit;
	bool bRemoveNameSpace;
	FVector ImportTranslation;
	//FRotator ImportRotation;
	float ImportUniformScale;
	//EFBXNormalImportMethod NormalImportMethod;
	//EFBXNormalGenerationMethod::Type NormalGenerationMethod;
	bool bTransformVertexToAbsolute;
	bool bBakePivotInVertex;
	//EFBXImportType ImportType;
	// Static Mesh options
	bool bCombineToSingle;
	//EVertexColorImportOption::Type VertexColorImportOption;
	FColor VertexOverrideColor;
	bool bRemoveDegenerates;
	bool bBuildAdjacencyBuffer;
	bool bBuildReversedIndexBuffer;
	bool bGenerateLightmapUVs;
	bool bOneConvexHullPerUCX;
	bool bAutoGenerateCollision;
	//FName StaticMeshLODGroup;
	bool bImportStaticMeshLODs;
	bool bAutoComputeLodDistances;
	//TArray<float> LodDistances;
	int32 MinimumLodNumber;
	int32 LodNumber;
	// Material import options
	//class UMaterialInterface *BaseMaterial;
	//FString BaseColorName;
	//FString BaseDiffuseTextureName;
	//FString BaseEmissiveColorName;
	//FString BaseNormalTextureName;
	//FString BaseEmmisiveTextureName;
	//FString BaseSpecularTextureName;
	//EMaterialSearchLocation MaterialSearchLocation;
	// Skeletal Mesh options
	bool bImportMorph;
	bool bImportAnimations;
	bool bUpdateSkeletonReferencePose;
	bool bResample;
	bool bImportRigidMesh;
	bool bUseT0AsRefPose;
	bool bPreserveSmoothingGroups;
	//FOverlappingThresholds OverlappingThresholds;
	bool bImportMeshesInBoneHierarchy;
	bool bCreatePhysicsAsset;
	//UPhysicsAsset *PhysicsAsset;
	bool bImportSkeletalMeshLODs;
	// Animation option
	//USkeleton* SkeletonForAnimation;
	//EFBXAnimationLengthImportType AnimationLengthImportType;
	//struct FIntPoint AnimationRange;
	//FString AnimationName;
	bool	bPreserveLocalTransform;
	bool	bDeleteExistingMorphTargetCurves;
	bool	bImportCustomAttribute;
	bool	bImportBoneTracks;
	bool	bSetMaterialDriveParameterOnCustomAttribute;
	bool	bRemoveRedundantKeys;
	bool	bDoNotImportCurveWithZero;
	//TArray<FString> MaterialCurveSuffixes;

	/** This allow to add a prefix to the material name when unreal material get created.
	*   This prefix can just modify the name of the asset for materials (i.e. TEXT("Mat"))
	*   This prefix can modify the package path for materials (i.e. TEXT("/Materials/")).
	*   Or both (i.e. TEXT("/Materials/Mat"))
	*/
	//FName MaterialBasePath;

	//This data allow to override some fbx Material(point by the uint64 id) with existing unreal material asset
	//TMap<uint64, class UMaterialInterface*> OverrideMaterials;

	/*
	 * Temporary copy of the mesh we re-import, we use this copy to compare section shape when matching section
	*/
	//UObject* OriginalMeshCopy;

// 	bool ShouldImportNormals()
// 	{
// 		return NormalImportMethod == FBXNIM_ImportNormals || NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
// 	}
// 
// 	bool ShouldImportTangents()
// 	{
// 		return NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
// 	}
// 
// 	void ResetForReimportAnimation()
// 	{
// 		bImportMorph = true;
// 		AnimationLengthImportType = FBXALIT_ExportedTime;
// 	}
// 
// 	static void ResetOptions(FBXImportOptions *OptionsToReset)
// 	{
// 		check(OptionsToReset != nullptr);
// 		*OptionsToReset = FBXImportOptions();
// 	}
};

class FFbxDataConverter
{
public:
	static FVector ConvertPos(FbxVector4 Vector);
	static FVector ConvertDir(FbxVector4 Vector);
};
class FFbxImporter
{
public:
	struct FFbxMaterial
	{
		FbxSurfaceMaterial* FbxMaterial;
		//UMaterialInterface* Material;

		std::string GetName() const { return FbxMaterial ? FbxMaterial->GetName() : ("None"); }
	};

	FFbxImporter();

	void ReadAllMeshNodes(FbxNode* pNode, std::vector<FbxNode*>& pOutMeshArray);

	UStaticMesh*		ImportStaticMesh(const char* InFileName, int LODIndex = 0);
	bool				BuildStaticMeshFromGeometry(FbxNode* Node, UStaticMesh* StaticMesh, std::vector<FFbxMaterial>& MeshMaterials, int LODIndex);
protected:
	FFbxDataConverter Converter;
	FbxGeometryConverter* GeometryConverter;
	FBXImportOptions* ImportOptions;

	FbxManager* SdkManager;
	FbxImporter* Importer;

	FbxScene* Scene;

	FbxAMatrix ComputeTotalMatrix(FbxNode* Node);
	bool IsOddNegativeScale(FbxAMatrix& TotalMatrix);
};