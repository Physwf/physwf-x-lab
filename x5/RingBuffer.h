#pragma once


template<size_t Size>
class RingBuffer
{
private:
	struct RingNode
	{
	public:
		RingNode() : Prev(this), Next(this), Data(NULL), DataSize(0) {}
		RingNode(void* InData, size_t InDataSize) : Prev(this), Next(this), Data(InData), DataSize(InDataSize) {}
		~RingNode() {}

		void Insert(RingNode* Point)
		{
			Next = Point->Next;
			Prev = Point;
			Point->Next = this;
		}
		void Detach()
		{
			Prev->Next = Next;
			Next->Prev = Prev;
		}

		void* operator()
		{
			return Data;
		}

	private:
		RingNode* Prev;
		RingNode* Next;
		void* Data;
		size_t DataSize;
	};
public:
	RingBuffer():Head(NULL),Tail(NULL) , SizeUsed(0)
	{ 
	}
	~RingBuffer() {}

	void* Alloc(size_t AllocSize)
	{
		if (AllocSize > (Size - SizeUsed)) return NULL;
		BYTE* Ret = Buffer + SizeUsed;
		SizeUsed += AllocSize;
		if()
		new RingNode(Ret, AllocSize);
		return Ret;
	}
	void Free(void* Ptr)
	{

	}
private:
	void* Head;
	void* Tail;
	BYTE Buffer[Size];
	size_t SizeUsed;
	
};