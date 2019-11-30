#pragma once

#include "gl.h"

#define MAX_VERTEX_ATTRIBUTE 8

#define MIN_BUFFER_WIDTH	128
#define MIN_BUFFER_HEIGHT	128
#define MAX_BUFFER_WIDTH	1920
#define MAX_BUFFER_HEIGHT	1080

#define MAX_ERROR_DESC_LEGHT 1024

template <typename T>
union gl_atrribute
{
	typename T Type;

	union
	{
		T values[4];
		struct 
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};
	};

	gl_atrribute() :values{0,0,0,0} {}
	gl_atrribute(T _x, T _y, T _z, T _w) :values{ _x,_y,_z,_w } {}
};

struct gl_atrribute_pointer
{
	GLbool	bEnabled;
	const GLvoid* pointer;
	GLint size;
	GLenum	type;
	GLsizei stride;
	gl_atrribute_pointer() :pointer(nullptr) {}
};

struct gl_vertex
{
	gl_atrribute<GLfloat> attributes[MAX_VERTEX_ATTRIBUTE];
};

struct gl_frontend
{
	GLint			viewport_x;
	GLint			viewport_y;
	GLsizei			viewport_width;
	GLsizei			viewport_height;

	GLclampf		depth_near;
	GLclampf		depth_far;
	GLbitfield		clear_bitmask;

	GLenum			front_face;
	GLenum			cull_face;

	gl_atrribute<GLfloat>	clear_color;
	GLclampf				clear_depth;

	GLuint			array_buffer;
	GLuint			index_buffer;

	gl_atrribute<GLfloat>			vertex_attributes[MAX_VERTEX_ATTRIBUTE];
	gl_atrribute_pointer			vertex_attribute_pointers[MAX_VERTEX_ATTRIBUTE];

	const GLvoid* indices_pointer;
	GLenum indices_type;
	GLsizei count;
	GLenum	draw_mode;

	GLuint program;

	GLenum error;
	GLchar error_desc[MAX_ERROR_DESC_LEGHT];
};



extern gl_frontend glContext;

void glContextInit();
void glContextDestroy();
void glClearError();
void glSetError(GLenum error, const GLchar* szErrorDesc);