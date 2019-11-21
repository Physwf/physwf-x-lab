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
	union
	{
		T values[4];
		T x, y, z, w;
		T r, g, b, a;
	};

	gl_atrribute() :values{0,0,0,0} {}
	gl_atrribute(T _x, T _y, T _z, T _w) :values{ _x,_y,_z,_w } {}
};

struct gl_atrribute_pointer
{
	GLbool	bEnabled;
	const GLvoid* pointer;
	GLenum	type;
	GLsizei stride;
	gl_atrribute_pointer() :pointer(nullptr) {}
};

struct gl_vertex
{
	gl_atrribute<GLfloat> attributes[MAX_VERTEX_ATTRIBUTE];
};

struct gl_fragment
{
	gl_atrribute<GLfloat> position;
	gl_atrribute<GLfloat> color;
};

struct gl_vertex_processor
{
	virtual void process() = 0;
};

struct gl_fragment_processor
{
	virtual void process() = 0;
};

struct gl_vertex_shader_object
{
	GLuint name;
	gl_vertex_processor* processor;
};

struct gl_fragment_shader_object
{
	GLuint name;
	gl_fragment_processor* processor;
};

struct gl_vertex_shader_object_node
{
	gl_vertex_shader_object shader;
	gl_vertex_shader_object_node* next;
};

struct gl_fragment_shader_object_node
{
	gl_vertex_shader_object shader;
	gl_vertex_shader_object_node* next;
};

struct gl_program_object
{
	GLuint name;
	gl_vertex_shader_object vertex_shader_object;
	gl_fragment_shader_object fragment_shader_object;
};

struct gl_program_object_node
{
	gl_program_object program;
	gl_program_object_node* next;
};

struct gl_context
{
	GLuint			buffer_width;
	GLuint			buffer_height;

	gl_atrribute<GLfloat>	clear_color;
	GLclampf				clear_depth;

	gl_fragment*	buffer;

	GLuint			array_buffer;
	GLuint			index_buffer;

	gl_atrribute<GLfloat>			vertex_attributes[MAX_VERTEX_ATTRIBUTE];
	gl_atrribute_pointer			vertex_attribute_pointers[MAX_VERTEX_ATTRIBUTE];

	GLvoid* indices_copy;
	GLsizei count;
	GLenum	indices_type;
	GLenum	primitive_type;

	gl_vertex_shader_object_node*		vertex_shaders;
	gl_fragment_shader_object_node*		fragment_shaders;
	gl_program_object_node*				programs;

	GLenum error;
	GLchar error_desc[MAX_ERROR_DESC_LEGHT];
};



extern gl_context glContext;

void glContextInit();
void glContextDestroy();
void glClearError();
void glSetError(GLenum error, const GLchar* szErrorDesc);