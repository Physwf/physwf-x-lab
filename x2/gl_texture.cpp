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

#include "gl.h"
#include "gl_texture.h"
#include "gl_frontend.h"

NAIL_API void glPixelStorei(GLenum pname, GLint param)
{
	switch (pname)
	{
	case GL_UNPACK_ALIGNMENT:
	{
		switch (param)
		{
		case UNPACK_ALIGNMENT_1:
		case UNPACK_ALIGNMENT_2:
		case UNPACK_ALIGNMENT_4:
		case UNPACK_ALIGNMENT_8:
			glContext.unpack_mode = param;
			break;
		default:
			glSetError(GL_INVALID_VALUE, "Invalid unpack alignment value!");
			break;
		}
	}
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Invalid enum value for glPixelStorei'pname!");
		break;
	}
}
NAIL_API void glActiveTexture(GLenum texture)
{
	if ((texture - GL_TEXTURE0) > (MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)) return;

}

NAIL_API void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{

}

NAIL_API void glBindTexture(GLenum target, GLuint texture)
{

}

NAIL_API void glDeleteTextures(GLsizei n, const GLuint *textures)
{

}

NAIL_API void glGenTextures(GLsizei n, GLuint *textures)
{

}