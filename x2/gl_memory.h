#pragma once

#include "gl.h"

int	  gl_minit();
void* gl_malloc(GLuint iSize);
void* gl_realloc(void* pOrignal, GLuint size);
void  gl_free(void* pData);