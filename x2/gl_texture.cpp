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
#include "gl_memory.h"

#include <memory>

gl_texture_object* gl_find_texture_object(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr) return nullptr;
	gl_named_object* object = node->object;
	if (object == nullptr) return nullptr;
	if (object->type != __TEXTURE_OBJECT__)
	{
		return nullptr;
	}
	return (gl_texture_object*)object;
}

gl_texture2d* gl_create_texture2d()
{
	gl_texture2d* result = (gl_texture2d*)gl_malloc(sizeof(gl_texture2d));
	result->format = GL_TEXTURE_2D;
	result->mipmap_count = 0;
	result->mipmaps = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	return result;
}

void gl_destory_texture2d(gl_texture2d* texture)
{
	for (GLint i = 0; i < MAX_MIPMAP_LEVEL; ++i)
	{
		if (texture->mipmaps[i] != nullptr)
		{
			gl_free(texture->mipmaps[i]);
		}
	}
}

gl_texture_cube* gl_create_texture_cube()
{
	gl_texture_cube* result = (gl_texture_cube*)gl_malloc(sizeof(gl_texture_cube));
	result->format = GL_TEXTURE_CUBE_MAP;
	result->mipmap_count = 0;
	result->mipmaps_x_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_x_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_y_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_y_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_z_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_z_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_x_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_x_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_y_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_y_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_z_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_z_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	return result;
}

GLvoid* gl_create_texture(GLenum type)
{
	switch (type)
	{
	case GL_TEXTURE_2D:			return gl_create_texture2d();
	case GL_TEXTURE_CUBE_MAP:	return gl_create_texture_cube();
	}
	return nullptr;
}

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
	GLenum index = texture - GL_TEXTURE0;
	if (index > (MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1))
	{
		glSetError(GL_INVALID_VALUE, "Invalid texture param!");
		return;
	}
	glContext.selected_texture_unit = &glContext.texture_params[index];
}

void gl_read_byte_type(GLfloat* data, GLsizei width, GLsizei height, GLenum format, const GLbyte *pixels)
{
	switch (format)
	{
	case GL_ALPHA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLbyte alpha = pixels[index];
				GLfloat nomalized_alpha = alpha / 255.0f;
				data[index + 0] = 0.0f;
				data[index + 1] = 0.0f;
				data[index + 2] = 0.0f;
				data[index + 3] = nomalized_alpha;
			}
		}
		break;
	}
	case GL_LUMINANCE:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLbyte luminance = pixels[index];
				GLfloat nomalized_luminance = luminance / 255.0f;
				data[index + 0] = nomalized_luminance;
				data[index + 1] = nomalized_luminance;
				data[index + 2] = nomalized_luminance;
				data[index + 3] = 1.0f;
			}
		}
		break;
	}
	case GL_LUMINANCE_ALPHA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLbyte luminance =	pixels[index + 0];
				GLbyte alpha =		pixels[index + 1];
				GLfloat nomalized_luminance		= luminance / 255.0f;
				GLfloat nomalized_alpha			= alpha		/ 255.0f;
				data[index + 0] = nomalized_luminance;
				data[index + 1] = nomalized_luminance;
				data[index + 2] = nomalized_luminance;
				data[index + 3] = alpha;
			}
		}
		break;
	}
	case GL_RGB:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLbyte r = pixels[index + 0];
				GLbyte g = pixels[index + 1];
				GLbyte b = pixels[index + 2];
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				data[index + 0] = nomalized_r;
				data[index + 1] = nomalized_g;
				data[index + 2] = nomalized_b;
				data[index + 3] = 1.0f;
			}
		}
		break;
	}
	case GL_RGBA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLbyte r = pixels[index + 0];
				GLbyte g = pixels[index + 1];
				GLbyte b = pixels[index + 2];
				GLbyte a = pixels[index + 2];
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = b / 255.0f;
				data[index + 0] = nomalized_r;
				data[index + 1] = nomalized_g;
				data[index + 2] = nomalized_b;
				data[index + 3] = nomalized_a;
			}
		}
		break;
	}
	}
}

void gl_read_short_type(GLfloat* data, GLsizei width, GLsizei height, GLenum type, const GLshort *pixels)
{
	switch (type)
	{
	case GL_UNSIGNED_SHORT_5_6_5:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLshort pixel = pixels[index];
				GLbyte r = pixel & 0b1111100000000000;
				GLbyte g = pixel & 0b0000011111100000;
				GLbyte b = pixel & 0b0000000000011111;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				data[index + 0] = nomalized_r;
				data[index + 1] = nomalized_g;
				data[index + 2] = nomalized_b;
				data[index + 3] = 1.0f;
			}
		}
	}
		break;
	case GL_UNSIGNED_SHORT_4_4_4_4:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLshort pixel = pixels[index];
				GLbyte r = pixel & 0b1111000000000000;
				GLbyte g = pixel & 0b0000111100000000;
				GLbyte b = pixel & 0b0000000011110000;
				GLbyte a = pixel & 0b0000000000001111;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = a / 255.0f;
				data[index + 0] = nomalized_r;
				data[index + 1] = nomalized_g;
				data[index + 2] = nomalized_b;
				data[index + 3] = nomalized_a;
			}
		}
	}
		break;
	case GL_UNSIGNED_SHORT_5_5_5_1:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLshort pixel = pixels[index];
				GLbyte r = pixel & 0b1111100000000000;
				GLbyte g = pixel & 0b0000011111000000;
				GLbyte b = pixel & 0b0000000000111110;
				GLbyte a = pixel & 0b0000000000000001;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = a / 255.0f;
				data[index + 0] = nomalized_r;
				data[index + 1] = nomalized_g;
				data[index + 2] = nomalized_b;
				data[index + 3] = nomalized_a;
			}
		}
	}
		break;
	}
}

void gl_allocate_texture2d_mipmap(gl_texture2d_mipmap** pmipmap, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	gl_texture2d_mipmap* mipmap = *pmipmap;
	if (mipmap != nullptr)
	{
		gl_free(mipmap->data);
	}
	else
	{
		mipmap = (gl_texture2d_mipmap*)gl_malloc(sizeof(gl_texture2d_mipmap));
	}
	mipmap->width = width;
	mipmap->height = height;
	mipmap->data = gl_malloc((width * height * 4) * sizeof(GLfloat));
	switch (type)
	{
	case GL_BYTE:
		gl_read_byte_type((GLfloat*)mipmap->data, width, height, format, (const GLbyte*)pixels);
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
		gl_read_short_type((GLfloat*)mipmap->data, width, height, type, (const GLshort*)pixels);
		break;
	}
}

NAIL_API void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	CONDITION_VALIDATE(level < 0 || level > MAX_MIPMAP_LEVEL, GL_INVALID_VALUE, "Invalid level params!");
	GLsizei max_width_for_level = 1 << (MAX_MIPMAP_LEVEL - level);
	CONDITION_VALIDATE(width < 0 || height < 0 || width > max_width_for_level || height > max_width_for_level, GL_INVALID_VALUE, "Invalid width height param, must be zero!");
	CONDITION_VALIDATE(border != 0, GL_INVALID_VALUE, "Invalid border param, must be zero!");

	switch (type)
	{
	case GL_UNSIGNED_BYTE:
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
		CONDITION_VALIDATE(format != GL_RGB, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	case GL_UNSIGNED_SHORT_4_4_4_4:
		CONDITION_VALIDATE(format != GL_RGBA, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	case GL_UNSIGNED_SHORT_5_5_5_1:
		CONDITION_VALIDATE(format != GL_RGBA, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	default:
	{
		glSetError(GL_INVALID_VALUE, "Invalid type param!");
		return;
		break;
	}
	}

	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		gl_texture2d* texture = glContext.texture2d_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps[level], internalformat, width, height, format, type, pixels);
		break;
	}
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_x_positive[level], internalformat, width, height, format, type, pixels);
	}
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_x_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_y_positive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_y_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_z_positive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
	{
		gl_texture_cube* texture = glContext.texture_cube_target;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_z_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	default:
	{
		glSetError(GL_INVALID_ENUM, "Invalid target param!");
		break;
	}
	}

}

NAIL_API void glBindTexture(GLenum target, GLuint texture)
{
	CONDITION_VALIDATE(target != GL_TEXTURE_2D || target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM, "Invalid target!");
	gl_texture_object* object = gl_find_texture_object(texture);
	CONDITION_VALIDATE(object == nullptr, GL_INVALID_VALUE, "Invalid texture parameter!");
	CONDITION_VALIDATE(object->type != 0 && object->type != target, GL_INVALID_OPERATION, "Type not match!");
	//todo target validity check
	if (object->type == 0)
	{
		object->type = target;
		object->texture = gl_create_texture(object->type);
	}
	glContext.selected_texture_unit->binded_object = object;
	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		glContext.texture2d_target = (gl_texture2d*)object->texture;
		break;
	}
	case GL_TEXTURE_CUBE_MAP:
	{
		glContext.texture_cube_target = (gl_texture_cube*)object->texture;
		break;
	}
	}
}

NAIL_API void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	CONDITION_VALIDATE(textures == nullptr, GL_INVALID_VALUE, "Invalid textures parameter!");
	for (GLsizei i = 0; i < n; ++i)
	{
		gl_texture_object* object = gl_find_texture_object(textures[i]);
		if (object != nullptr)
		{
			//check binding
			//release_texture
			gl_destroy_named_object(textures[i]);
		}
	}
}

NAIL_API void glGenTextures(GLsizei n, GLuint *textures)
{
	CONDITION_VALIDATE(textures == nullptr, GL_INVALID_VALUE, "Invalid textures parameter!");
	for (GLsizei i = 0; i < n; ++i)
	{
		textures[i] = gl_create_named_object(__TEXTURE_OBJECT__);
	}
}
