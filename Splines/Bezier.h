#pragma once

#include <vector>
#include "Algebra.h"
#include "Geometry.h"

template<int N>
class Bezier
{
public:
	Bezier() {}
	Bezier(const std::vector<QPoint2D>& CPs)
	{
		int Count = (int)CPs.size();
		if (Count > N) Count = N;
		for (int i = 0; i < Count; ++i)
			ControlPoints[i] = CPs[i];
	}

	void UpdateControlPoints(const std::vector<QPoint2D>& CPs);
	QPoint2D Interpolate(double t);

private:
	QPoint2D ControlPoints[N];
};

template<int N>
void Bezier<N>::UpdateControlPoints(const std::vector <QPoint2D>& CPs)
{
	int Count = (int)CPs.size();
	if (Count > N) Count = N;
	for (int i = 0; i < Count; ++i)
		ControlPoints[i] = CPs[i];
}

template<int N>
QPoint2D Bezier<N>::Interpolate(double t)
{
	T ret;
	for (int i = 0; i < N; ++i)
	{
		int C = Combination(N,i);
		T P = ControlPoints[i];
		double ratio = std::pow(1 - t, N - i)* std::pow(t, i);
		ret += (C * ratio) * P;
	}
	return ret;
}
