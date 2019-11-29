#pragma once

template <typename T>
T glClamp(T value, T min, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
};

template <typename T> 
T glAbs(T value)
{
	if (value < 0) return -value;
	else return value;
}

template <typename T>
T glMin(T v1, T v2)
{
	return v1 <= v2 ? v1 : v2;
};

template <typename T, typename ... Args>
T glMin(T v1, T v2, Args... args)
{
	return glMin(glMin(v1, v2), args...);
};

template <typename T>
T glMax(T v1, T v2)
{
	return v1 <= v2 ? v2 : v1;
};

template <typename T, typename ... Args>
T glMax(T v1, T v2, Args... args)
{
	return glMax(glMax(v1, v2), args...);
};

