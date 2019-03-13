#pragma once

#include "gl.h"

int	  glMInit();
void* glMAlloc(GLuint iSize);
void  glFree(void* pData);