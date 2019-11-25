#include "gl_frontend.h"
#include "gl_utilities.h"
#include "gl_memory.h"
#include "gl_pipeline.h"

#include <assert.h>
#include <string.h>

gl_frontend glContext;

void glContextInit()
{
	glClearError();

	glContext.viewport_x = 0;
	glContext.viewport_y = 0;
	glContext.viewport_width = 256; //glClamp(width, (GLuint)MIN_BUFFER_WIDTH, (GLuint)MAX_BUFFER_WIDTH);
	glContext.viewport_height = 256;// glClamp(width, (GLuint)MIN_BUFFER_HEIGHT, (GLuint)MAX_BUFFER_HEIGHT);

	glContext.depth_near = 0.0f;
	glContext.depth_far = 1.0f;

	glContext.array_buffer = 0;
	glContext.index_buffer = 0;

	glContext.indices_pointer = nullptr;

	glContext.program = 0;

	//glContext.buffer = (gl_fragment*)gl_malloc(glContext.buffer_width * glContext.buffer_height * sizeof(gl_fragment));

// 	for (gl_atrribute<GLfloat>& attrib : glContext.vertex_attributes)
// 	{
// 		attrib.values[0] = attrib.values[1] = attrib.values[2] = attrib.values[3] = .0f;
// 	}

// 	for (gl_atrribute_pointer& attrib_ptr : glContext.vertex_attribute_pointers)
// 	{
// 		attrib_ptr.pointer = nullptr;
// 		attrib_ptr.stride = 0;
// 	}
}

void glContextDestroy()
{
	glClearError();
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

NAIL_API void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glClearError();

	if (count < 0)
	{
		glSetError(GL_INVALID_VALUE, "count can't be less than zero!");
		return;
	}

	switch (mode)
	{
	case GL_POINT_LIST:
	case GL_LINE_LIST:
	case GL_LINE_LIST_ADJ:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJ:
	case GL_TRIANGLE_LIST:
	case GL_TRIANGLE_LIST_ADJ:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_STRIP_ADJ:
		glContext.draw_mode = mode;
		glContext.indices_pointer = nullptr;
		glContext.indices_type = GL_SHORT;
		glContext.count = count;
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
	gl_emit_draw_command();
}

NAIL_API void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	glClearError();

	switch (mode)
	{
	case GL_POINT_LIST:
	case GL_LINE_LIST:
	case GL_LINE_LIST_ADJ:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJ:
	case GL_TRIANGLE_LIST:
	case GL_TRIANGLE_LIST_ADJ:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_STRIP_ADJ:
	{
		glContext.draw_mode = mode;
		glContext.count = count;
		glContext.indices_pointer = indices;
		glContext.indices_type = type;

		switch (type)
		{
		case GL_BYTE:
		case GL_SHORT:
			break;
		default:
			glSetError(GL_INVALID_ENUM, "Illegal type argument!");
		}

	}
	break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
	gl_emit_draw_command();
}

