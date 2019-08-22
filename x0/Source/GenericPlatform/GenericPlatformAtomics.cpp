#include "GenericPlatform/GenericPlatformAtomics.h"

static_assert(sizeof(XInt128) == 16), "XInt128 must be 16 bytes!");
static_assert(alignof(XInt128) == 16, "XInt128 alignment must equals 16!");