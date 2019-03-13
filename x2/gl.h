#pragma once

typedef unsigned int	GLEnum;
typedef char			GLchar;
typedef signed char		GLbyte;
typedef unsigned char	GLubyte;
typedef int				GLint;
typedef unsigned int	GLuint;
typedef short			GLshort;
typedef unsigned short  GLushort;
typedef int				GLsizei;
typedef float			GLfloat;
typedef bool			GLbool;

//GLerros

#define GL_NO_ERROR							0
#define GL_INVALID_ENUM						0x00100
#define GL_INVALID_FRAMEBUFFER_OPERATION	0x00101
#define GL_INVALID_VALUE					0x00102
#define GL_INVALID_OPERATION				0x00103
#define GL_OUT_OF_MEMEORY					0x00104

#define GL_MAX_VERTEX_ATTRIBUTE				0x00200


#define NAIL_API

NAIL_API void glVertexAttrib1f(GLuint index, GLfloat x);
NAIL_API void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y);
NAIL_API void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z);
NAIL_API void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

NAIL_API void glVertexAttrib1fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib2fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib3fv(GLuint index, const GLfloat* values);
NAIL_API void glVertexAttrib4fv(GLuint index, const GLfloat* values);

NAIL_API void glVertexAttribPointer(GLuint index, GLint size, GLEnum type, GLbool normalized, GLsizei stride, const void* ptr);

NAIL_API void glEnableVertexAttribArray(GLuint index);
NAIL_API void glDisableVertexAttribArray(GLuint index);

NAIL_API GLuint glCreateShader(GLEnum type);
NAIL_API void glDeleteShader(GLuint shader);
NAIL_API GLuint glCreateProgram();
NAIL_API void glAttachShader(GLuint program, GLuint shader);
NAIL_API void glDetachShader(GLuint program, GLuint shader);
NAIL_API void glLinkProgram(GLuint program);
NAIL_API void glUseProgram(GLuint program);
NAIL_API void glDeleteProgram(GLuint program);

