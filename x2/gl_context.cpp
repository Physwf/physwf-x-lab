#include "gl_context.h"
#include "gl_utilities.h"
#include "gl_memory.h"

#include <assert.h>
#include <string.h>

gl_context glContext;

void glContextInit()
{
	glClearError();

	glContext.buffer_width = 0; //glClamp(width, (GLuint)MIN_BUFFER_WIDTH, (GLuint)MAX_BUFFER_WIDTH);
	glContext.buffer_height = 0;// glClamp(width, (GLuint)MIN_BUFFER_HEIGHT, (GLuint)MAX_BUFFER_HEIGHT);

	glContext.buffer = (gl_fragment*)glMAlloc(glContext.buffer_width * glContext.buffer_height * sizeof(gl_fragment));

	for (gl_atrribute<GLfloat>& attrib : glContext.vertex_attributes)
	{
		attrib.values[0] = attrib.values[1] = attrib.values[2] = attrib.values[3] = .0f;
	}

	for (gl_atrribute_pointer& attrib_ptr : glContext.vertex_attribute_pointers)
	{
		attrib_ptr.pointer = nullptr;
		attrib_ptr.stride = 0;
	}
}

void glContextDestroy()
{
	glClearError();
	glFree(glContext.buffer);
}

void glClearError()
{
	glContext.error = GL_NO_ERROR;
	glContext.error_desc[0] = 0;
}

void glSetError(GLenum error, const GLchar* szErrorDesc)
{
	glContext.error = error;
	assert(strlen(szErrorDesc) < MAX_ERROR_DESC_LEGHT);
	strcpy_s(glContext.error_desc, szErrorDesc);
}

