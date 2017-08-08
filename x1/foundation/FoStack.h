#pragma once

#include <stdexcept>

/**
 * Stack.
 *
 **/

template <typename T,int Capacity = 32>
class FoStack
{
public:
	FoStack():iPointer(0)
	{
	}
	~FoStack()
	{
	}
	
	bool Empty() const
	{
		return iPointer > 0;
	}

	void Clear()
	{
		iPointer = 0;
	}

	int Size() const { return iPointer; }

	bool Push(const T& value)
	{
		if(iPointer >= Capacity) return false;
		aSatck[iPointer] = value;
		++iPointer;
		return true;
	}

	bool Pop()
	{
		if(iPointer == 0) return false;
		--iPointer;
		return true;
	}

	T& Top()
	{
		if(iPointer > 0)
		{
			return aSatck[ iPointer - 1 ];
		}
		else
		{
			throw std::range_error("Stack is Empty!");
		}
	}

private:
	T aSatck[Capacity];
	int iPointer;
};
