#include "Algebra.h"
#include <assert.h>

void MatrixUnitTest()
{
	{
		QMatrixf<2, 2> M{ 1,1,2,3 };
		assert(1 == M.Determinant());
	}
	{
		QMatrixf<3, 3> M{ 1,1,1,2,2,2,3,3,3 };
		assert(0 == M.Determinant());
	} 
	{
		QMatrixf<3, 3> M{ 1,2,3,4,5,6,7,8,9 };
		assert(0 == M.Determinant());
	}
	{
		QMatrixf<3, 3> M{ 3,-2,4, 1,2,1, 0,1,-1 };
		assert(-7 == M.Determinant());
	}

	{
		QMatrixf<3, 3> M{ 3,-2,4, 1,2,1, 0,1,-1 };
		QMatrixf<3, 3> Adj = M.Adjugate();
	}

	{
		QMatrixf<3, 3> M{ 3,-2,4, 1,2,1, 0,1,-1 };
		QMatrixf<3, 3> Inv = M.Inverse();
		QMatrixf<3, 3> Product =  Inv * M;
	}

	{
		QMatrixf<3, 3> M{ -3,2,-5, -1,0,-2, 3,-4,1 };
		QMatrixf<2, 2> m00 = M.MinorMaxtrix(0, 0);
		QMatrixf<2, 2> m01 = M.MinorMaxtrix(0, 1);
		QMatrixf<2, 2> m02 = M.MinorMaxtrix(0, 2);
		QMatrixf<2, 2> m10 = M.MinorMaxtrix(1, 0);
		QMatrixf<2, 2> m11 = M.MinorMaxtrix(1, 1);
		QMatrixf<2, 2> m12 = M.MinorMaxtrix(1, 2);
		QMatrixf<2, 2> m20 = M.MinorMaxtrix(2, 0);
		QMatrixf<2, 2> m21 = M.MinorMaxtrix(2, 1);
		QMatrixf<2, 2> m22 = M.MinorMaxtrix(2, 2);
		m00.Print();
		m01.Print();
		m02.Print();
		m10.Print();
		m11.Print();
		m12.Print();
		m20.Print();
		m21.Print();
		m22.Print();

		QMatrixf<3, 3> adj = M.Adjugate();
		//QMatrixf<3, 3> Product = M * Inv;
	}
}