#include "gl.h"
#include "gl_context.h"

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

NAIL_API void glVertexAttribPointer(GLuint index, GLint size, GLEnum type, GLbool normalized, GLsizei stride, const void* ptr)
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

NAIL_API GLuint glCreateShader(GLEnum type)
{
	return 0;
}

NAIL_API void glDeleteShader(GLuint shader)
{

}

NAIL_API GLuint glCreateProgram()
{
	return 0;
}

NAIL_API void glAttachShader(GLuint program, GLuint shader)
{

}

NAIL_API void glDetachShader(GLuint program, GLuint shader)
{

}

NAIL_API void glLinkProgram(GLuint program)
{

}

NAIL_API void glUseProgram(GLuint program)
{

}

NAIL_API void glDeleteProgram(GLuint program)
{

}