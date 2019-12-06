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

typedef unsigned int	GLbitfield;
typedef unsigned int	GLenum;
typedef char			GLchar;
typedef signed char		GLbyte;
typedef unsigned char	GLubyte;
typedef int				GLint;
typedef int				GLintptr;
typedef unsigned int	GLuint;
typedef short			GLshort;
typedef unsigned short  GLushort;
typedef int				GLsizei;
typedef int				GLsizeiptr;
typedef float			GLfloat;
typedef double			GLdouble;
typedef float			GLclampf;
typedef bool			GLbool;
typedef void			GLvoid;

//DataType
#define GL_BYTE								0x0100
#define GL_UNSIGNED_BYTE					0x0101
#define GL_SHORT							0x0102
#define GL_UNSIGNED_SHORT					0x0103
#define GL_INT								0x0104
#define GL_UNSIGNED_INT						0x0105
#define GL_FLOAT							0x0106
#define GL_DOUBLE							0x0107
#define GL_UNSIGNED_SHORT_5_6_5				0x0108
#define GL_UNSIGNED_SHORT_4_4_4_4			0x0109
#define GL_UNSIGNED_SHORT_5_5_5_1			0x0110

//uniforms DataType
#define GL_FLOAT_VEC2						0x0108
#define GL_FLOAT_VEC3						0x0109
#define GL_FLOAT_VEC4						0x0110
#define GL_INT_VEC2							0x0111
#define GL_INT_VEC3							0x0112
#define GL_INT_VEC4							0x0113
#define GL_BOOL_VEC2						0x0114
#define GL_BOOL_VEC3						0x0115 
#define GL_BOOL_VEC4						0x0116 
#define GL_FLOAT_MAT2						0x0117
#define GL_FLOAT_MAT3						0x0118 
#define GL_FLOAT_MAT4						0x0119
#define GL_SAMPLER_2D						0x0120
#define GL_SAMPLER_CUBE						0x0121

//Primitives
#define GL_POINT_LIST						0x0000
#define GL_LINE_LIST						0x0001
#define GL_LINE_LIST_ADJ					0x0002
#define GL_LINE_STRIP						0x0003
#define GL_LINE_STRIP_ADJ					0x0004
#define GL_TRIANGLE_LIST					0x0005
#define GL_TRIANGLE_LIST_ADJ				0x0006
#define GL_TRIANGLE_STRIP					0x0007
#define GL_TRIANGLE_STRIP_ADJ				0x0008

//shaders
#define GL_VERTEX_SHADER					0x1000
#define GL_FRAGMENT_SHADER					0x1001

//GLerros
#define GL_NO_ERROR							0
#define GL_INVALID_ENUM						0x00100
#define GL_INVALID_FRAMEBUFFER_OPERATION	0x00101
#define GL_INVALID_VALUE					0x00102
#define GL_INVALID_OPERATION				0x00103
#define GL_OUT_OF_MEMEORY					0x00104

#define GL_MAX_VERTEX_ATTRIBUTE				0x00200
#define GL_ACTIVE_UNIFORM_MAX_LENGTH		0x00201
#define GL_ACTIVE_UNIFORMS					0x00202

#define GL_COLOR_BUFFER_BIT					0x00001
#define GL_DEPTH_BUFFER_BIT					0x00002
#define GL_STENCIL_BUFFER_BIT				0x00004			


#define GL_FRONT							0x20001
#define GL_BACK								0x20002
#define GL_FRONT_AND_BACK					0x20003

#define GL_CCW								0x20011
#define GL_CW								0x20012

#define GL_UNPACK_ALIGNMENT					0x30000

//formats
#define GL_ALPHA							0x50000
#define GL_RGB								0x50001
#define GL_RGBA								0x50002
#define GL_LUMINANCE						0x50003
#define GL_LUMINANCE_ALPHA					0x50004
//textures
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS	0x60001
#define GL_MAX_TEXTURE_IMAGE_UNITS			0x60002
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS	0x60003

//targets
#define GL_TEXTURE_2D						0x60100
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X		0x60101
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X		0x60102
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y		0x60103
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y		0x60104
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z		0x60105
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z		0x60106
#define GL_TEXTURE_CUBE_MAP					0x60107

#define GL_TEXTURE0							0x70000
#define GL_TEXTURE1							0x70001
#define GL_TEXTURE2							0x70002
#define GL_TEXTURE3							0x70003
#define GL_TEXTURE4							0x70004
#define GL_TEXTURE5							0x70005
#define GL_TEXTURE6							0x70006
#define GL_TEXTURE7							0x70007

#define GL_TEXTURE_WRAP_S					0x80001
#define GL_TEXTURE_WRAP_T					0x80002
#define GL_TEXTURE_MIN_FILTER				0x80003
#define GL_TEXTURE_MAG_FILTER				0x80004
//wrap mode
#define GL_CLAMP_TO_EDGE					0x80101
#define GL_REPEAT							0x80102
#define GL_MIRRORED_REPEAT					0x80103
//min/mag
#define GL_NEAREST							0x80201
#define GL_LINEAR							0x80202
#define GL_NEAREST_MIPMAP_NEAREST			0x80203
#define GL_NEAREST_MIPMAP_LINEAR			0x80204
#define GL_LINEAR_MIPMAP_NEAREST			0x80205
#define GL_LINEAR_MIPMAP_LINEAR				0x80206

#include "NAIL.h"

NAIL_API void glViewport(GLint x, GLint y, GLsizei w, GLsizei h);
NAIL_API void glDepthRangef(GLclampf n, GLclampf f);
NAIL_API void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
NAIL_API void glClearDepth(GLclampf d);
NAIL_API void glClear(GLbitfield buf);
NAIL_API void glFrontFace(GLenum dir);
NAIL_API void glCullFace(GLenum mode);
NAIL_API void glFlush();
NAIL_API void glVertexAttrib1f(GLuint index, GLfloat x);
NAIL_API void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y);
NAIL_API void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z);
NAIL_API void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

NAIL_API void glVertexAttrib1fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib2fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib3fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib4fv(GLuint index, const GLfloat* values);

NAIL_API void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLbool normalized, GLsizei stride, const void* ptr);

NAIL_API void glEnableVertexAttribArray(GLuint index);
NAIL_API void glDisableVertexAttribArray(GLuint index);

NAIL_API void glGenBuffer(GLsizei n, GLuint* buffers);
NAIL_API void glBindBuffer(GLenum target, GLuint buffer);
NAIL_API void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
NAIL_API void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

NAIL_API void glGenVertexArrays(GLsizei n, GLuint* buffers);
NAIL_API void glDeleteVertexArrays(GLsizei n, GLuint* buffers);
NAIL_API void glBindVertexArray(GLuint buffer);

NAIL_API void glDrawArrays(GLenum mode, GLint first, GLsizei count);
NAIL_API void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

NAIL_API GLuint glCreateShader(GLenum type);
NAIL_API void glCompileShader(GLuint shader);
//NAIL_API void glShaderSource(uint shader, sizei count, const char **string, const int *length);
NAIL_API void glShaderSource(GLuint shader,GLvoid* source);
NAIL_API void glDeleteShader(GLuint shader);
NAIL_API GLuint glCreateProgram();
NAIL_API void glAttachShader(GLuint program, GLuint shader);
NAIL_API void glDetachShader(GLuint program, GLuint shader);
NAIL_API void glLinkProgram(GLuint program);
NAIL_API void glUseProgram(GLuint program);
NAIL_API void glDeleteProgram(GLuint program);
NAIL_API GLint glGetUniformLocation(GLuint program, const GLchar* name);

NAIL_API void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);

NAIL_API void glUniform1i(GLint location, GLint v0);
NAIL_API void glUniform2i(GLint location, GLint v0, GLint v1);
NAIL_API void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
NAIL_API void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

NAIL_API void glUniform1f(GLint location, GLfloat v0);
NAIL_API void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
NAIL_API void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
NAIL_API void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

NAIL_API void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
NAIL_API void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
NAIL_API void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
NAIL_API void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);

NAIL_API void glUniform1iv(GLint location, GLsizei count, const GLint *value);
NAIL_API void glUniform2iv(GLint location, GLsizei count, const GLint *value);
NAIL_API void glUniform3iv(GLint location, GLsizei count, const GLint *value);
NAIL_API void glUniform4iv(GLint location, GLsizei count, const GLint *value);

NAIL_API void glUnitformMatrix2fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* value);
NAIL_API void glUnitformMatrix3fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* value);
NAIL_API void glUnitformMatrix4fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* value);

NAIL_API void glPixelStorei(GLenum pname, GLint param);
NAIL_API void glActiveTexture(GLenum texture);
NAIL_API void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
NAIL_API void glBindTexture(GLenum target, GLuint texture);
NAIL_API void glDeleteTextures(GLsizei n, const GLuint *textures);
NAIL_API void glGenTextures(GLsizei n, GLuint *textures);
NAIL_API void glGenerateMipmap(GLenum target);
NAIL_API void glTexParameteri(GLenum target, GLenum pname, GLint param);
NAIL_API void glTexParameterf(GLenum target, GLenum pname, GLfloat param);


