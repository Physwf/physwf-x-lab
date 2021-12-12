#pragma once

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
	T m[Row][Coloum];

	QMatrix() : m{ {0} } {}

	T Determinant() const;
	QMatrix Adjugate() const;
	QMatrix Inverse() const;
	QMatrix<T, Row - 1, Coloum - 1> Minor(int r, int c);
};

template<typename T, int Coloum, int Row>
QMatrix<T, Coloum, Row> QMatrix<T, Coloum, Row>::Inverse() const
{
	T Det_Rec = 1 / Determinant();
	QMatrix Adj = Adjugate();
	for (int r = 0; r < R; ++r)
	{
		for (int c = 0; c < C; ++c)
		{
			Adj.m[r][c] *= Det_Rec;
		}
	}
	return Adj;
}

template<typename T, int Row, int Coloum>
inline QMatrix<T, Row - 1, Coloum - 1> QMatrix<T, Row, Coloum>::Minor(int R, int C)
{
	QMatrix<T, Row - 1, Coloum - 1> Result;
	for (int r = 0; r < R; ++r)
	{
		for (int c = 0; c < C; ++c)
		{
			Result.m[r][c] = m[r][c];
		}
	}
	for (int r = R; r < Row - 1; ++r)
	{
		for (int c = C; c < Coloum - 1; ++c)
		{
			Result.m[r][c] = m[r + 1][c + 1];
		}
	}
	return Result;
}

template<typename T, int Coloum, int Row>
QMatrix<T, Coloum, Row> QMatrix<T, Coloum, Row>::Adjugate() const
{
	QMatrix<T, Coloum, Row> Result;
	for (int r = 0; r < Row; ++r)
	{
		for (int c = 0; c < Coloum; ++c)
		{
			Result.m[r][c] = Minor(r, c).Determinant();
		}
	}
}

template<typename T, int Coloum, int Row>
T QMatrix<T, Coloum, Row>::Determinant() const
{
	T Sum = 0;
	int Permutation[Row] = { 0 };
	for (int i = 0; i < Row; ++i) Permutation[i] = i;
	int i = 0;
	for (int r = 0; r < Coloum; ++r)
	{
		T Product = 1;
		for (int j = 0; j < Row; ++j)
		{
			int c = Permutation[(j + i) % Row];
			Product *= m[r][c];
		}
		Sum += Product;
	}
	for (int i = Row - 1; i >= 0; --i) Permutation[i] = i;
	for (int r = 0; r < Coloum; ++r)
	{
		T Product = 1;
		for (int j = 0; j < Row; ++j)
		{
			int c = Permutation[(j + i) % Row];
			Product *= m[r][c];
		}
		Sum -= Product;
	}
}
