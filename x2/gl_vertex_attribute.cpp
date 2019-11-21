﻿#include "gl.h"
#include "gl_context.h"
#include "gl_utilities.h"

NAIL_API void glViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
	glClearError();
	glContext.buffer_width = w;
	glContext.buffer_height = h;
}


NAIL_API void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	glClearError();

	glContext.clear_color.r = glClamp(r, 0.0f, 0.1f);
	glContext.clear_color.g = glClamp(g, 0.0f, 0.1f);
	glContext.clear_color.b = glClamp(b, 0.0f, 0.1f);
	glContext.clear_color.a = glClamp(a, 0.0f, 0.1f);
}

NAIL_API void glClearDepth(GLclampf d)
{
	glClearError();
	glContext.clear_depth = glClamp(d, 0.0f, 0.1f);
}

NAIL_API void glVertexAttrib1f(GLuint index, GLfloat x)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, 0, 0, 1);
}

NAIL_API void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, 0, 1);
}

NAIL_API void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, z, 1);
}

NAIL_API void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, z, w);
}

NAIL_API void glVertexAttrib1fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], 0, 0, 1);
}

NAIL_API void glVertexAttrib2fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], 0, 1);
}

NAIL_API void glVertexAttrib3fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], values[2], 1);
}

NAIL_API void glVertexAttrib4fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], values[2], values[3]);
}

NAIL_API void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLbool normalized, GLsizei stride, const void* ptr)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].pointer = ptr;
	glContext.vertex_attribute_pointers[index].stride = stride;
	glContext.vertex_attribute_pointers[index].type = type;
}

NAIL_API void glEnableVertexAttribArray(GLuint index)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].bEnabled = true;
}


NAIL_API void glDisableVertexAttribArray(GLuint index)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].bEnabled = false;
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
		glContext.primitive_type = mode;
		glContext.indices_copy = nullptr;
		glContext.count = count;
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
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
		glContext.primitive_type = mode;
		GLsizei indices_size = type == GL_SHORT ? count * 2 : count;
		glContext.indices_copy = gl_malloc(indices_size);
		memcpy_s(glContext.indices_copy, indices_size, indices, indices_size);
		glContext.count = count;
		glContext.indices_type = type;
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
}



