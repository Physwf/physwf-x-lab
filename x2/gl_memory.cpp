#include "gl_memory.h"
#include <stdlib.h>

int glMInit()
{
	return 0;
}

void* glMAlloc(GLuint iSize)
{
	return ::malloc(iSize);
}

void glFree(void* pData)
{
	::free(pData);
}
