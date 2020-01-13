#pragma once

#include <unordered_map>
#include <type_traits>

template <typename ElementType, typename ElementIDType>
class TMeshElementArray
{
public:

	/** Resets the container, optionally reserving space for elements to be added */
	inline void Reset(const int32 Elements = 0)
	{
		Container.Reset();
		Container.Reserve(Elements);
	}

	/** Reserves space for the specified total number of elements */
	inline void Reserve(const int32 Elements) { /*Container.Reserve(Elements);*/ }

	/** Add a new element at the next available index, and return the new ID */
	inline ElementIDType Add() 
	{ 
		Container.insert({ MaxID, ElementType() });
		return ElementIDType(MaxID++);
	}

	/** Add the provided element at the next available index, and return the new ID */
	inline ElementIDType Add(const typename std::remove_reference<ElementType>::type& Element) 
	{ 
		Container.insert(MaxID, Element);
		return ElementIDType(MaxID++);
	}

	/** Add the provided element at the next available index, and return the ID */
	inline ElementIDType Add(ElementType&& Element) 
	{ 
		Container.Add(MaxID, std::forward<ElementType>(Element));
		return ElementIDType(MaxID++);
	}

	/** Inserts a new element with the given ID */
	inline ElementType& Insert(const ElementIDType ID)
	{
		Container.insert(ID.GetValue(), ElementType());
		return Container[ID.GetValue()];
	}

	/** Inserts the provided element with the given ID */
	inline ElementType& Insert(const ElementIDType ID, const typename std::remove_reference<ElementType>::type& Element)
	{
		Container.insert(ID.GetValue(), Element);
		return Container[ID.GetValue()];
	}

	/** Inserts the provided element with the given ID */
	inline ElementType& Insert(const ElementIDType ID, ElementType&& Element)
	{
		Container.insert(ID.GetValue(), std::forward<ElementType>(Element));
		return Container[ID.GetValue()];
	}

	/** Removes the element with the given ID */
	inline void Remove(const ElementIDType ID)
	{
		//checkSlow(Container.IsAllocated(ID.GetValue()));
		Container.RemoveAt(ID.GetValue());
	}

	/** Returns the element with the given ID */
	inline ElementType& operator[](const ElementIDType ID)
	{
		//checkSlow(Container.IsAllocated(ID.GetValue()));
		return Container[ID.GetValue()];
	}

	inline const ElementType& operator[](const ElementIDType ID) const
	{
		//checkSlow(Container.IsAllocated(ID.GetValue()));
		return Container.at(ID.GetValue());
	}

	/** Returns the number of elements in the container */
	inline int32 Num() const { return (int)Container.size(); }

	/** Returns the index after the last valid element */
	inline int32 GetArraySize() const { return MaxID; }

	/** Returns the first valid ID */
	inline ElementIDType GetFirstValidID() const
	{
		return MaxID > 0 ? ElementIDType(Container.begin()->first) : ElementIDType::Invalid;
	}

	/** Returns whether the given ID is valid or not */
	inline bool IsValid(const ElementIDType ID) const
	{
		return ID.GetValue() >= 0 && ID.GetValue() < MaxID && Container.find(ID.GetValue()) != Container.end();
	}

	std::vector<ElementIDType> GetElementIDs() const
	{
		std::vector<ElementIDType> Keys;
		for (auto kv : Container)
		{
			Keys.push_back(ElementIDType(kv.first));
		}
		return Keys;
	}
private:
	int MaxID{0};
	std::unordered_map<int, ElementType> Container;
};