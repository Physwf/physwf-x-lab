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

#pragma once

#include "gl.h"
#include "gl_objects.h"


struct gl_texture2d_mipmap
{
	GLsizei width;
	GLsizei height;
	GLvoid* data;
};

struct gl_texture2d
{
	GLenum format;
	GLsizei mipmap_count;
	gl_texture2d_mipmap** mipmaps;
};

struct gl_texture_cube
{
	GLenum format;
	GLsizei mipmap_count;
	gl_texture2d_mipmap** mipmaps_x_positive;
	gl_texture2d_mipmap** mipmaps_x_nagetive;
	gl_texture2d_mipmap** mipmaps_y_positive;
	gl_texture2d_mipmap** mipmaps_y_nagetive;
	gl_texture2d_mipmap** mipmaps_z_positive;
	gl_texture2d_mipmap** mipmaps_z_nagetive;
};

struct gl_texture_object : public gl_named_object
{
	GLenum type;
	GLvoid* texture;
};

struct gl_texture_unit_params
{
	gl_texture_object* binded_object;
};

struct gl_texture_unit
{
	gl_texture_unit_params params;
	//gl_texture_object* object;
};

gl_texture_object*		gl_find_texture_object(GLuint name);
gl_texture2d*			gl_create_texture2d();
gl_texture_cube*		gl_create_texture_cube();