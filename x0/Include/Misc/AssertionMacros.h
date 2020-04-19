#pragma once

#include <cassert>

#define check(expr)					{ assert(expr); }
#define checkf(expr,format,...)		{ assert(expr); }
#define checkSlow(expr)				{ assert(expr); }
#define ensure(expr)				{ assert(expr); }
#define verify(expr)				{ assert(expr); }
#define ensureMsgf(expr,format,...) { assert(expr); }