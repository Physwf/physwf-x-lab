#include "box.h"
#include "paramset.h"

BoxFilter* CreateBoxFilter(const ParamSet &ps)
{
	Float xw = 0.5f;
	Float yw = 0.5f;
	return new BoxFilter(Vector2f(xw, yw));
}

