#pragma once

#include <initializer_list>
#include <iostream>

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

template<typename T, int Row, int Coloum>
struct QMatrix
{
	static_assert(Row > 0, "Illegal Row count!");
	static_assert(Coloum > 0, "Illegal Coloum count!");

	T m[Row][Coloum];

	QMatrix() : m{ {0} } {}
	QMatrix(std::initializer_list<T> list);

	bool IsSquare() const { return Row == Coloum; }
	T Determinant() const;
	QMatrix<T, Coloum, Row> Transpose() const;
	QMatrix Adjugate() const;
	QMatrix Inverse() const;
	QMatrix<T, Row-1, Coloum - 1> MinorMaxtrix(int r, int c) const;
	T Cofactor(int r, int c) const;

	template<int Row2, int Coloum2>
	QMatrix<T, Row, Coloum2> operator*(const QMatrix<T, Row2, Coloum2>& rhs) const;

	void Print(const char* delimater = ",");
};

template<typename T, int Row, int Coloum>
void QMatrix<T, Row, Coloum>::Print(const char* delimater /*= ","*/)
{
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			std::cout << m[r][c] << delimater;
		}
		std::cout << std::endl;
	}
}

template<typename T, int Row, int Coloum>
QMatrix<T, Coloum, Row> QMatrix<T, Row, Coloum>::Transpose() const
{
	QMatrix<T, Coloum, Row> Result;
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			Result.m[c][r] = m[r][c];
		}
	}
	return Result;
}

template<typename T, int Row, int Coloum>
template<int Row2, int Coloum2>
QMatrix<T, Row, Coloum2> QMatrix<T, Row, Coloum>::operator*(const QMatrix<T, Row2, Coloum2>& rhs) const
{
	static_assert(Coloum == Row2, "Multiply only valid when Coloum1 == Row2");
	QMatrix<T, Row, Coloum2> Result;
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum2; ++c)
		{
			T Sum = 0;
			for (int i = 0; i < Row2; ++i)
			{
				Sum += m[r][i] * rhs.m[i][c];
			}
			Result.m[r][c] = Sum;
		}
	}
	return Result;
}

template<typename T, int Row, int Coloum>
QMatrix<T, Row, Coloum>::QMatrix(std::initializer_list<T> list)
{
	if (list.size() < Row * Coloum) return;
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			m[r][c] = list.begin()[r * Coloum + c];
		}
	}
}

template<typename T, int Coloum, int Row>
QMatrix<T, Coloum, Row> QMatrix<T, Coloum, Row>::Inverse() const
{
	T Det_Rec = 1 / Determinant();
	QMatrix Adj = Adjugate();
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			Adj.m[r][c] *= Det_Rec;
		}
	}
	return Adj;
}

template<typename T, int Row, int Coloum>
QMatrix<T, Row-1, Coloum - 1> QMatrix<T, Row, Coloum>::MinorMaxtrix(int R, int C) const
{
	QMatrix<T, Row - 1, Coloum - 1> Result;
	for (int r = 0; r < Row - 1; ++r)
	{
		for (int c = 0; c < Coloum - 1; ++c)
		{
			int src_r = r < R ? r : r + 1;
			int src_c = c < C ? c : c + 1;
			Result.m[r][c] = m[src_r][src_c];
		}
	}
	return Result;
}

template<typename T, int Row, int Coloum>
inline T QMatrix<T, Row, Coloum>::Cofactor(int R, int C) const
{
	return ((R + C) % 2 == 0 ? 1 : -1) * MinorMaxtrix(R, C).Determinant();
}

template<typename T, int Row, int Coloum>
QMatrix<T, Row, Coloum> QMatrix<T, Row, Coloum>::Adjugate() const
{
	QMatrix<T, Row, Coloum> Result;
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			Result.m[r][c] = Cofactor(r, c);
		}
	}
	return Result.Transpose();
}


template<typename T, int Coloum, int Row>
T QMatrix<T, Coloum, Row>::Determinant() const
{
	static_assert(Coloum == Row, "Only Square Matrix has Determinant!");

	if (Row == 2) return m[0][0] * m[1][1] - m[0][1] * m[1][0];

	T Sum = 0;
	int Permutation[Row] = { 0 };
	for (int i = 0; i < Row; ++i) Permutation[i] = i;
	for (int loop = 0; loop < Row; ++loop)
	{
		T Product = 1;
		for (int r = 0; r < Row; ++r)
		{
			 int c = Permutation[(loop+r)%Row];
			 Product *= m[r][c];
		}
		Sum += Product;
	}
	
	for (int i = 0; i < Row; ++i) Permutation[i] = Row - i - 1;
	for (int loop = 0; loop < Row; ++loop)
	{
		T Product = 1;
		for (int r = 0; r < Row; ++r)
		{
			int c = Permutation[(loop + r) % Row];
			Product *= m[r][c];
		}
		Sum -= Product;
	}
	return Sum; 
}

template<int Coloum, int Row>
using QMatrixf = struct QMatrix<float, Coloum, Row>;
template<int Coloum, int Row>
using QMatrixd = struct QMatrix<double, Coloum, Row>;