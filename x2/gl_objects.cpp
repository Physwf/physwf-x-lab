#include "gl_frontend.h"
#include "gl_buffer.h"

NAIL_API void glGenBuffer(GLsizei n, GLuint* buffers)
{
	GLsizei i = 0;
	while (i < n)
	{
		buffers[i++]  = gl_create_buffer();
	}
}

NAIL_API void glBindBuffer(GLenum target, GLuint buffer)
{

}

NAIL_API void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{

}

NAIL_API void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{

}

NAIL_API void glGenVertexArrays(GLsizei n, GLuint* buffers)
{

}
NAIL_API void glDeleteVertexArrays(GLsizei n, GLuint* buffers)
{

}
NAIL_API void glBindVertexArray(GLuint buffer)
{

}