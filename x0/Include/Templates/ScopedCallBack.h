#pragma once

#include "CoreTypes.h"

template<class CallbackType>
class TScopedCallback
{
public:

	TScopedCallback()
		:Counter(0)
	{

	}

	~TScopedCallback()
	{
		if (HasRequest())
		{
			CallbackType::FireCallback();
		}
	}

	void Request()
	{
		++Counter;
	}

	void UnRequest()
	{
		--Counter;
	}

	bool HasRequest() const
	{
		return Counter > 0;
	}

private:
	int32 Counter;
};