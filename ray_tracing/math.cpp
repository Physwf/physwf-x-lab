#include "math.h"
#include "core.h"
#include <cctype>

int VecRead(const char* desc, std::size_t size, Vec& Result)
{
	std::size_t startpos = 0;
	int index = 0;
	for (std::size_t i = startpos; i < size; ++i)
	{
		if (desc[i] == ',' && index < 3)
		{
			if (index < 3)
			{
				Result[index++] = std::atof(&desc[startpos]);
				startpos = i + 1;
			}
		}
	}
	return index == 3;
}

int TransmissionDirection(const Surf* m1, const Surf* m2, const Point& I, const Point& N, Point& T)
{
	Float n1, n2, eta, c1, cs2;

	n1 = m1 ? m1->n : 1.0;
	n2 = m2 ? m2->n : 1.0;

	eta = n1 / n2;
	c1 = -Dot(I, N);
	cs2 = 1.0 - eta * eta * (1.0 - c1 * c1);
	if (cs2 < 0.0) return 0;
	MultiplyAdd(eta, I, eta*c1 - sqrtl(cs2), N, T);
	return 1;
}

