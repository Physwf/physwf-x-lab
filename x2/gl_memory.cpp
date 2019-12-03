#include "gl_memory.h"
#include <stdlib.h>

#include "gl_pipeline.h"

int gl_minit()
{
	return 0;
}

void* gl_malloc(GLuint iSize,const char* name/* = nullptr*/)
{
	void* result = ::malloc(iSize);
	//glPpeline.data[result] = iSize;
// 	if (name)
// 	{
// 		glPpeline.datanames[result] = name;
// 	}
	return result;
}

void* gl_realloc(void* pOrignal, GLuint size)
{
	return ::realloc(pOrignal, size);
}

void gl_free(void* pData)
{
	//glPpeline.datanames.erase(pData);
	//glPpeline.data.erase(pData);
	::free(pData);
}
