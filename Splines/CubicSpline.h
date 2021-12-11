#pragma once

template<typename T,int n>
class CubicSpline
{
public:
	CubicSpline() {}

private:
	T ControlPoints[n];
};