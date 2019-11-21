#include "gl_memory.h"
#include <stdlib.h>

int gl_minit()
{
	return 0;
}

void* gl_malloc(GLuint iSize)
{
	return ::malloc(iSize);
}

void gl_free(void* pData)
{
	::free(pData);
}
