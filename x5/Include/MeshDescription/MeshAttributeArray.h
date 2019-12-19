#pragma once

#include <vector>
#include <map>
#include <string>
#include <tuple>

using AttributeTypes = std::tuple
<
	FVector4,
	FVector,
	FVector2D,
	float,
	int,
	bool,
	std::string
>;

template <typename ElementType>
class TMeshAttributeArrayBase
{
public:
protected:

	friend class UMeshDescription;

	void Insert(const int32 Index, const ElementType& Default)
	{
		if (Index >= (int32)Container.size())
		{
			Container.resize(Index+1, ElementType(Default));
		}
	}

	void Initialize(const int32 ElementCount, const ElementType& Default)
	{
		Container.clear();
		Insert(ElementCount - 1, Default);
	}
protected:
	std::vector<ElementType> Container;
};

template <typename AttributeType, typename ElementIDType>
class TAttributeIndicesArray;

template <typename ElementType, typename ElementIDType>
class TMeshAttributeArray : private TMeshAttributeArrayBase<ElementType>
{
	using TMeshAttributeArrayBase<ElementType>::Container;
public:

	/** Element accessors */
	inline const ElementType& operator[](const ElementIDType ElementID) const { return Container[ElementID.GetValue()]; }
	inline ElementType& operator[](const ElementIDType ElementID) { return Container[ElementID.GetValue()]; }

	/** Return size of container */
	inline int32 Num() const { return Container.Num(); }

	/** Return base of data */
	inline const ElementType* GetData() const { return Container.data(); }

protected:
	friend class TAttributeIndicesArray <ElementType, ElementIDType>;

	inline void Insert(const ElementIDType Index, const ElementType& Default)
	{
		TMeshAttributeArrayBase<ElementType>::Insert(Index.GetValue(), Default);
	}

};

template <typename AttributeType> using TVertexAttributeArray = TMeshAttributeArray<AttributeType, FVertexID>;
template <typename AttributeType> using TVertexInstanceAttributeArray = TMeshAttributeArray<AttributeType, FVertexInstanceID>;
template <typename AttributeType> using TEdgeAttributeArray = TMeshAttributeArray<AttributeType, FEdgeID>;
template <typename AttributeType> using TPolygonAttributeArray = TMeshAttributeArray<AttributeType, FPolygonID>;
template <typename AttributeType> using TPolygonGroupAttributeArray = TMeshAttributeArray<AttributeType, FPolygonGroupID>;

enum class EMeshAttributeFlags : uint32
{
	None = 0,
	Lerpable = (1 << 0),
	AutoGenerated = (1 << 1),
	Mergeable = (1 << 2)
};

template <typename T, typename U>
class TAttributeIndicesArray
{
public:
	using AttributeType = T;
	using ElementIDType = U;

	/** Default constructor - required so that it builds correctly */
	TAttributeIndicesArray() = default;

	/** Constructor */
	TAttributeIndicesArray(const int32 NumberOfIndices, const AttributeType& InDefaultValue, const EMeshAttributeFlags InFlags, const int32 InNumberOfElements)
		: NumElements(InNumberOfElements),
		DefaultValue(InDefaultValue),
		Flags(InFlags)
	{
		SetNumIndices(NumberOfIndices);
	}

	inline void Insert(const ElementIDType ElementID)
	{
		for (TMeshAttributeArray<AttributeType, ElementIDType>& ArrayForIndex : ArrayForIndices)
		{
			ArrayForIndex.Insert(ElementID, DefaultValue);
		}

		NumElements = FMath::Max(NumElements, ElementID.GetValue() + 1);
	}

	/** Remove the element at the given index, replacing it with a default value */
	inline void Remove(const ElementIDType ElementID)
	{
		for (TMeshAttributeArray<AttributeType, ElementIDType>& ArrayForIndex : ArrayForIndices)
		{
			ArrayForIndex[ElementID] = DefaultValue;
		}
	}

	/** Return the TMeshAttributeArray corresponding to the given attribute index */
	inline const TMeshAttributeArray<AttributeType, ElementIDType>& GetArrayForIndex(const int32 Index) const { return ArrayForIndices[Index]; }
	inline TMeshAttributeArray<AttributeType, ElementIDType>& GetArrayForIndex(const int32 Index) { return ArrayForIndices[Index]; }

	/** Return flags for this attribute type */
	inline EMeshAttributeFlags GetFlags() const { return Flags; }

	/** Return default value for this attribute type */
	inline AttributeType GetDefaultValue() const { return DefaultValue; }

	/** Return number of indices this attribute has */
	inline int32 GetNumIndices() const { return ArrayForIndices.Num(); }

	/** Return number of elements each attribute index has */
	inline int32 GetNumElements() const { return NumElements; }

	void SetNumIndices(const int32 NumIndices)
	{
		//check(NumIndices > 0);
		const int32 OriginalNumIndices = ArrayForIndices.size();
		ArrayForIndices.resize(NumIndices);

		// If we have added new indices, ensure they are filled out with the correct number of elements
		for (int32 Index = OriginalNumIndices; Index < NumIndices; ++Index)
		{
			ArrayForIndices[Index].Initialize(NumElements, DefaultValue);
		}
	}
private:
	/** Number of elements in each index */
	int32 NumElements;

	/** An array of MeshAttributeArrays, one per attribute index */
	std::vector<TMeshAttributeArray<AttributeType, ElementIDType>> ArrayForIndices;

	/** The default value for an attribute of this name */
	AttributeType DefaultValue;

	/** Implementation-defined attribute name flags */
	EMeshAttributeFlags Flags;
};

template <typename AttributeType> using TVertexAttributeIndicesArray = TAttributeIndicesArray<AttributeType, FVertexID>;
template <typename AttributeType> using TVertexInstanceAttributeIndicesArray = TAttributeIndicesArray<AttributeType, FVertexInstanceID>;
template <typename AttributeType> using TEdgeAttributeIndicesArray = TAttributeIndicesArray<AttributeType, FEdgeID>;
template <typename AttributeType> using TPolygonAttributeIndicesArray = TAttributeIndicesArray<AttributeType, FPolygonID>;
template <typename AttributeType> using TPolygonGroupAttributeIndicesArray = TAttributeIndicesArray<AttributeType, FPolygonGroupID>;

template <typename T, typename U>
class TAttributesMap
{
public:
	using AttributeType = T;
	using ElementIDType = U;
	using AttributeIndicesArrayType = TAttributeIndicesArray<AttributeType, ElementIDType>;
	using MapType = std::map<std::string, AttributeIndicesArrayType>;

	inline TAttributesMap()
		: NumElements(0)
	{}

	inline void RegisterAttribute(const std::string& AttributeName, const int32 NumberOfIndices, const AttributeType& Default, const EMeshAttributeFlags Flags)
	{
		if (Map.find(AttributeName) == Map.end())
		{
			Map.emplace(AttributeName, TAttributeIndicesArray<AttributeType, ElementIDType>(NumberOfIndices, Default, Flags, NumElements));
		}
	}
	inline void UnregisterAttribute(const std::string& AttributeName)
	{
		Map.Remove(AttributeName);
	}

	/** Determines whether an attribute exists with the given name */
	inline bool HasAttribute(const std::string& AttributeName) const
	{
		return Map.Contains(AttributeName);
	}

	/** Get attribute array with the given name and index */
	inline TMeshAttributeArray<AttributeType, ElementIDType>& GetAttributes(const std::string& AttributeName, const int32 AttributeIndex = 0)
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return Map.at(AttributeName).GetArrayForIndex(AttributeIndex);
	}

	inline const TMeshAttributeArray<AttributeType, ElementIDType>& GetAttributes(const std::string& AttributeName, const int32 AttributeIndex = 0) const
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return Map.at(AttributeName).GetArrayForIndex(AttributeIndex);
	}

	inline TAttributeIndicesArray<AttributeType, ElementIDType>& GetAttributesSet(const std::string& AttributeName)
	{
		// @todo mesh description: should this handle non-existent attribute names gracefully?
		return Map.at(AttributeName);
	}

	inline const TAttributeIndicesArray<AttributeType, ElementIDType>& GetAttributesSet(const std::string& AttributeName) const
	{
		// @todo mesh description: should this handle non-existent attribute names gracefully?
		return Map.at(AttributeName);
	}

	/** Returns the number of indices for the attribute with the given name */
	inline int32 GetAttributeIndexCount(const std::string& AttributeName) const
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return Map.at(AttributeName).GetNumIndices();
	}

	/** Sets the number of indices for the attribute with the given name */
	inline void SetAttributeIndexCount(const std::string& AttributeName, const int32 NumIndices)
	{
		Map.at(AttributeName).SetNumIndices(NumIndices);
	}

	/** Returns an array of all the attribute names registered for this attribute type */
	template <typename Allocator>
	inline void GetAttributeNames(std::vector<std::string, Allocator>& OutAttributeNames) const
	{
		//Map.GetKeys(OutAttributeNames);
	}

	/** Gets a single attribute with the given ElementID, Name and Index */
	inline AttributeType GetAttribute(const ElementIDType ElementID, const std::string& AttributeName, const int32 AttributeIndex = 0) const
	{
		return Map.at(AttributeName).GetArrayForIndex(AttributeIndex)[ElementID];
	}

	/** Sets a single attribute with the given ElementID, Name and Index to the given value */
	inline void SetAttribute(const ElementIDType ElementID, const std::string& AttributeName, const int32 AttributeIndex, const AttributeType& AttributeValue)
	{
		Map.at(AttributeName).GetArrayForIndex(AttributeIndex)[ElementID] = AttributeValue;
	}

	/** Inserts a default-initialized value for all attributes of the given ID */
	void Insert(const ElementIDType ElementID)
	{
		NumElements = FMath::Max(NumElements, ElementID.GetValue() + 1);
		for (auto& AttributeNameAndIndicesArray : Map)
		{
			AttributeNameAndIndicesArray.second.Insert(ElementID);
			//check(AttributeNameAndIndicesArray.Value.GetNumElements() == this->NumElements);
		}
	}

	/** Removes all attributes with the given ID */
	void Remove(const ElementIDType ElementID)
	{
		for (auto& AttributeNameAndIndicesArray : Map)
		{
			AttributeNameAndIndicesArray.second.Remove(ElementID);
		}
	}

	/** Initializes all attributes to have the given number of elements with the default value */
	void Initialize(const int32 Count)
	{
		NumElements = Count;
		for (auto& AttributeNameAndIndicesArray : Map)
		{
			AttributeNameAndIndicesArray.Value.Initialize(Count);
		}
	}

	/** Returns the number of elements held by each attribute in this map */
	inline int32 GetNumElements() const
	{
		return NumElements;
	}

private:
	/** Number of elements for each attribute index */
	int32 NumElements;

	/** The actual container */
	MapType Map;
};

template <class T, class Tuple>
struct TTupleIndex;

template <class T, class... Types>
struct TTupleIndex<T, std::tuple<T, Types...>> {
	static const std::size_t Value = 0;
};

template <class T, class U, class... Types>
struct TTupleIndex<T, std::tuple<U, Types...>> {
	static const std::size_t Value = 1U + TTupleIndex<T, std::tuple<Types...>>::Value;
};

template <typename ElementIDType>
class TAttributesSet
{
public:
	template <typename AttributeType>
	void RegisterAttribute(const std::string& AttributeName, const int32 NumberOfIndices = 1, const AttributeType& Default = AttributeType(), const EMeshAttributeFlags Flags = EMeshAttributeFlags::None)
	{
		std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).RegisterAttribute(AttributeName, NumberOfIndices, Default, Flags);
	}
	/** Unregister an attribute name with the given type */
	template <typename AttributeType>
	void UnregisterAttribute(const std::string& AttributeName)
	{
		std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).UnregisterAttribute(AttributeName);
	}

	template <typename AttributeType>
	bool HasAttribute(const std::string& AttributeName) const
	{
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).HasAttribute(AttributeName);
	}

	template <typename AttributeType>
	TMeshAttributeArray<AttributeType, ElementIDType>& GetAttributes(const std::string& AttributeName, const int32 AttributeIndex = 0)
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributes(AttributeName, AttributeIndex);
	}

	template <typename AttributeType>
	const TMeshAttributeArray<AttributeType, ElementIDType>& GetAttributes(const std::string& AttributeName, const int32 AttributeIndex = 0) const
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributes(AttributeName, AttributeIndex);
	}

	/**
 * Get a set of attribute arrays with the given type and name.
 *
 * Example of use:
 *
 *		const TArray<TVertexInstanceAttributeArray<FVector2D>>& UVs = VertexInstanceAttributes().GetAttributesSet<FVector2D>( "UV" );
 *		for( const FVertexInstanceID VertexInstanceID : GetVertexInstances().GetElementIDs() )
 *		{
 *			const FVector2D UV0 = UVs[ 0 ][ VertexInstanceID ];
 *			const FVector2D UV1 = UVs[ 1 ][ VertexInstanceID ];
 *			DoSomethingWith( UV0, UV1 );
 *		}
 */
	template <typename AttributeType>
	TAttributeIndicesArray<AttributeType, ElementIDType>& GetAttributesSet(const std::string& AttributeName)
	{
		// @todo mesh description: should this handle non-existent attribute names gracefully?
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributesSet(AttributeName);
	}

	template <typename AttributeType>
	const TAttributeIndicesArray<AttributeType, ElementIDType>& GetAttributesSet(const std::string& AttributeName) const
	{
		// @todo mesh description: should this handle non-existent attribute names gracefully?
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributesSet(AttributeName);
	}

	/** Returns the number of indices for the attribute with the given name */
	template <typename AttributeType>
	int32 GetAttributeIndexCount(const std::string& AttributeName) const
	{
		// @todo mesh description: should this handle non-existent attribute names and indices gracefully?
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributeIndexCount(AttributeName);
	}

	/** Sets the number of indices for the attribute with the given name */
	template <typename AttributeType>
	void SetAttributeIndexCount(const std::string&, const int32 NumIndices)
	{
		std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).SetAttributeIndexCount(AttributeName, NumIndices);
	}

	/** Returns an array of all the attribute names registered for this attribute type */
	template <typename AttributeType, typename Allocator>
	void GetAttributeNames(std::vector<std::string, Allocator>& OutAttributeNames) const
	{
		std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttributeNames(OutAttributeNames);
	}

	template <typename AttributeType>
	AttributeType GetAttribute(const ElementIDType ElementID, const std::string& AttributeName, const int32 AttributeIndex = 0) const
	{
		return std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).GetAttribute(ElementID, AttributeName, AttributeIndex);
	}

	template <typename AttributeType>
	void SetAttribute(const ElementIDType ElementID, const std::string& AttributeName, const int32 AttributeIndex, const AttributeType& AttributeValue)
	{
		std::get<TTupleIndex<AttributeType, AttributeTypes>::Value>(Container).SetAttribute(ElementID, AttributeName, AttributeIndex, AttributeValue);
	}

	void Insert(const ElementIDType ElementID)
	{
		//VisitTupleElements([ElementID](auto& AttributesMap) { AttributesMap.Insert(ElementID); }, Container);
		std::get<0>(Container).Insert(ElementID);
		std::get<1>(Container).Insert(ElementID);
		std::get<2>(Container).Insert(ElementID);
		std::get<3>(Container).Insert(ElementID);
		std::get<4>(Container).Insert(ElementID);
		std::get<5>(Container).Insert(ElementID);
		std::get<6>(Container).Insert(ElementID);
	}

	/** Removes all attributes with the given ID */
	void Remove(const ElementIDType ElementID)
	{
		std::get<0>(Container).Remove(ElementID);
		std::get<1>(Container).Remove(ElementID);
		std::get<2>(Container).Remove(ElementID);
		std::get<3>(Container).Remove(ElementID);
		std::get<4>(Container).Remove(ElementID);
		std::get<5>(Container).Remove(ElementID);
		std::get<6>(Container).Remove(ElementID);
	}

	/** Initializes the attribute set with the given number of elements, all at the default value */
	void Initialize(const int32 NumElements)
	{
		std::get<0>(Container).Initialize(NumElements);
		std::get<1>(Container).Initialize(NumElements);
		std::get<2>(Container).Initialize(NumElements);
		std::get<3>(Container).Initialize(NumElements);
		std::get<4>(Container).Initialize(NumElements);
		std::get<5>(Container).Initialize(NumElements);
		std::get<6>(Container).Initialize(NumElements);
	}
private:
	using ContainerType = 
	std::tuple
	<
		TAttributesMap<FVector4,	ElementIDType>,
		TAttributesMap<FVector,		ElementIDType>,
		TAttributesMap<FVector2D,	ElementIDType>,
		TAttributesMap<float,		ElementIDType>,
		TAttributesMap<int,			ElementIDType>,
		TAttributesMap<bool,		ElementIDType>,
		TAttributesMap<std::string, ElementIDType>
	>;

	ContainerType Container;
};

