#pragma once

#include <cassert>

#define check(expr)					{ assert(expr); }
#define checkSlow(expr)				{ assert(expr); }
#define ensure(expr)				{ assert(expr); }