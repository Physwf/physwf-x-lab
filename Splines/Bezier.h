#pragma once

#include <vector>

template<typename T>
T Factorial(T n)
{
	T ret = 1;
	while (n > 1)
	{
		ret *= n;
		n--;
	}
	return ret;
}

template<typename T>
T Combination(T n, T k)
{
	return Factorial(n) / (Factorial(n - k) * Factorial(k));
}

template<typename T, int N>
class Bezier
{
public:
	Bezier() {}
	Bezier(const std::vector<T>& CPs) 
	{
		int Count = (int)CPs.size();
		if (Count > N) Count = N;
		for (int i = 0; i < Count; ++i)
			ControlPoints[i] = CPs[i];
	}

	void UpdateControlPoints(const std::vector<T>& CPs);
	T Interpolate(double t);

private:
	T ControlPoints[N];
};

template<typename T, int N>
void Bezier<T, N>::UpdateControlPoints(const std::vector<T>& CPs)
{
	int Count = (int)CPs.size();
	if (Count > N) Count = N;
	for (int i = 0; i < Count; ++i)
		ControlPoints[i] = CPs[i];
}

template<typename T, int N>
T Bezier<T, N>::Interpolate(double t)
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
