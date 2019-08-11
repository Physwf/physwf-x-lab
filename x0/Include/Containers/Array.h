#pragma once

template <typename InElementType, typename InAllocator>
class TArray
{
	template <typename OtherElementType, typename OtherAllocator>
	friend class TArray;

public:
	typedef InElementType	ElementType;
	typedef InAllocator		Allocator;

	TArray()
		: ArrayNum(0)
		, ArrayMax(0)
	{}

	TArray(const ElementType* Ptr, int32 Count)
	{
		CopyToEmpty(Ptr, Count, 0, 0);
	}

	TArray(std::initializer_list<InElementType> InitList)
	{
		CopyToEmpty(InitList.begin(), (int32)InitList.size(), 0, 0);
	}

	template <typename OtherElementType, typename OtherAllocator>
	explicit TArray(const TArray<OtherElementType, OtherAllocator>& Other)
	{
		CopyToEmpty(Other.GetData(), Other.Num(), 0, 0);
	}

	TArray(const TArray& Other)
	{
		CopyToEmpty(Other.GetData(), Other.Num(), 0, 0);
	}

	TArray(const TArray& Other, int32 ExtraSlack)
	{
		CopyToEmpty(Other.GetData(), Other.Num(), 0, ExtraSlack);
	}

	TArray& operator=(std::initializer_list<InElementType> InitList)
	{
		return *this;
	}

	template<typename OtherAllocator>
	TArray& operator=(const TArray<ElementType, OtherAllocator>& Other)
	{
		return *this;
	}
private:
public:
	TArray(TArray&& Other)
	{

	}

	template <typename OtherElementType, typename OtherAllocator>
	explicit TArray(TArray<OtherElementType, OtherAllocator>&& Other)
	{
	}

	template <typename OtherElementType>
	TArray(TArray<OtherElementType, Allocator>&& Other, int32 ExtraSlack)
	{

	}

	TArray& operator=(TArray&& Other)
	{
		return *this;
	}

	~TArray()
	{
	}

	ElementType* GetData()
	{
		
	}

	const ElementType* GetData() const
	{

	}

	uint32 GetTypeSize() const
	{
		return sizeof(ElementType);
	}

	uint32 GetAllocatedSize(void) const
	{
		
	}

	int32 GetSlack() const
	{
		return ArrayMax - ArrayNum;
	}

	bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < ArrayNum;
	}

	int32 Num() const
	{
		return ArrayNum;
	}

	int32 Max() const
	{
		return ArrayMax;
	}

	ElementType& operator[](int32 Index)
	{
		
	}

	const ElementType& operator[](int32 Index) const
	{
	}


	template <typename OtherElementType>
	void CopyToEmpty(const OtherElementType* OtherData, int32 OtherNum, int32 PrevMax, int32 ExtraSlack)
	{

	}
protected:
	int32 ArrayNum;
	int32 ArrayMax;
};