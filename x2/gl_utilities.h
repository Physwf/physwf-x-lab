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

