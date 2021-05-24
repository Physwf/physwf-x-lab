#pragma once

#include <initializer_list>
template <typename T,int Count>
class Vector
{
public:
	union
	{
		struct  
		{
			T X, Y, Z;
		};
		T Elements[Count];
	};

	Vector();
	Vector(std::initializer_list<T> InInitializer);

};

typedef Vector<int, 3> Vector3i;