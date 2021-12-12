#pragma once

#include "Algebra.h"
#include "Geometry.h"

template<int N>
class CubicSpline
{
public:
	CubicSpline() {}

private:
	QPoint2D ControlPoints[N];
	float Solution[N][4];
};