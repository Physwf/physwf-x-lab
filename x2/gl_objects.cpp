/* Copyright(C) 2019-2020 Physwf

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < http://www.gnu.org/licenses/>.
*/

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