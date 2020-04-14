#pragma once

#include "fbxsdk.h"

#include "X5.h"

#include <vector>
#include <memory>

#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Color.h"

#include "MeshDescription/MeshDescription.h"
#include "StaticMeshResources.h"

struct FStaticMeshSourceModel
{
	/*
	 * The staticmesh owner of this source model. We need the SM to be able to convert between MeshDesription and RawMesh.
	 * RawMesh use int32 material index and MeshDescription use FName material slot name.
	 * This memeber is fill in the PostLoad of the static mesh.
	 * TODO: Remove this member when FRawMesh will be remove.
	 */
	class UStaticMesh* StaticMeshOwner;
	/*
	 * Accessor to Load and save the raw mesh or the mesh description depending on the editor settings.
	 * Temporary until we deprecate the RawMesh.
	 */
	bool IsRawMeshEmpty() const;
	void LoadRawMesh(struct FRawMesh& OutRawMesh) const;
	void SaveRawMesh(struct FRawMesh& InRawMesh, bool bConvertToMeshdescription = true);

	/* Original Imported mesh description data. Optional for all but the first LOD. Autogenerate LOD do not have original mesh description*/
	FMeshDescription* OriginalMeshDescription;

	/**
	 * ScreenSize to display this LOD.
	 * The screen size is based around the projected diameter of the bounding
	 * sphere of the model. i.e. 0.5 means half the screen's maximum dimension.
	 */
	float ScreenSize;

	/** Default constructor. */
	FStaticMeshSourceModel();

	/** Destructor. */
	~FStaticMeshSourceModel();
};

class UStaticMeshDescriptions
{
public:
	// Define these in a concrete compilation unit so the TUniquePtr deleter works
	UStaticMeshDescriptions();
	~UStaticMeshDescriptions();

	/** Empties the mesh description container */
	void Empty();

	/** Returns the number of mesh descriptions in the container. This should always equal the number of LODs */
	int32 Num() const;

	/** Set the number of mesh descriptions in the container */
	void SetNum(const int32 Num);

	/** Gets mesh description for the given LOD */
	FMeshDescription* Get(int32 Index) const;

	/** Creates an empty mesh description for the given LOD */
	FMeshDescription* Create(int32 Index);

	/** Clears the mesh description for the given LOD */
	void Reset(int32 Index);

	/** Inserts new LODs at the given index */
	void InsertAt(int32 Index, int32 Count = 1);

	/** Deletes LODs at the given index */
	void RemoveAt(int32 Index, int32 Count = 1);

private:
	std::vector<std::unique_ptr<FMeshDescription>> MeshDescriptions;
};
/**
 * Per-section settings.
 */
struct FMeshSectionInfo
{
		/** Index in to the Materials array on UStaticMesh. */
	int32 MaterialIndex;

	/** If true, collision is enabled for this section. */
	bool bEnableCollision;

	/** If true, this section will cast shadows. */
	bool bCastShadow;

	/** Default values. */
	FMeshSectionInfo()
		: MaterialIndex(0)
		, bEnableCollision(true)
		, bCastShadow(true)
	{
	}

	/** Default values with an explicit material index. */
	explicit FMeshSectionInfo(int32 InMaterialIndex)
		: MaterialIndex(InMaterialIndex)
		, bEnableCollision(true)
		, bCastShadow(true)
	{
	}
};

struct FMeshSectionInfoMap
{
	/** Maps an LOD+Section to the material it should render with. */
	std::map<uint32, FMeshSectionInfo> Map;

	/** Clears all entries in the map resetting everything to default. */
	void Clear();

	/** Get the number of section for a LOD. */
	int32 GetSectionNumber(int32 LODIndex) const;

	/** Return true if the section exist, false otherwise. */
	bool IsValidSection(int32 LODIndex, int32 SectionIndex) const;

	/** Gets per-section settings for the specified LOD + section. */
	FMeshSectionInfo Get(int32 LODIndex, int32 SectionIndex) const;

	/** Sets per-section settings for the specified LOD + section. */
	void Set(int32 LODIndex, int32 SectionIndex, FMeshSectionInfo Info);

	/** Resets per-section settings for the specified LOD + section to defaults. */
	void Remove(int32 LODIndex, int32 SectionIndex);

	/** Copies per-section settings from the specified section info map. */
	void CopyFrom(const FMeshSectionInfoMap& Other);

	/** Returns true if any section of the specified LOD has collision enabled. */
	bool AnySectionHasCollision(int32 LodIndex) const;
};



class UStaticMesh
{
	friend class FFbxImporter;
	friend class FStaticMeshBuilder;
public:
	std::unique_ptr<class FStaticMeshRenderData> RenderData;

	static void RegisterMeshAttributes(FMeshDescription& MeshDescription);

	void LoadMeshDescriptions();
	void UnloadMeshDescriptions();

	FMeshDescription* GetOriginalMeshDescription(int32 LodIndex);
	FMeshDescription* CreateOriginalMeshDescription(int32 LodIndex);
	void CommitOriginalMeshDescription(int32 LodIndex);
	void ClearOriginalMeshDescription(int32 LodIndex);

	//SourceModels API
	FStaticMeshSourceModel& AddSourceModel();
	void SetNumSourceModels(int32 Num);
	void RemoveSourceModel(int32 Index);

	X5_API void Build();

	X5_API virtual void InitResources();

	X5_API void CacheDerivedData();

private:
	std::vector<FStaticMeshSourceModel> SourceModels;
	UStaticMeshDescriptions* MeshDescriptions;
	FMeshSectionInfoMap SectionInfoMap;
};



class FObjImporter
{
public:
	static void Import(const char* InFileName, UStaticMesh* OutMesh);
};
