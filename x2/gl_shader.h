#pragma once
#include "NAIL.h"

#define GL_PROGRAM				0x1003

#define ACTIVE_UNIFORM_MAX_LENGTH	64
#define MAX_UNIFORMS				128

struct gl_uniform
{
	GLchar		name[ACTIVE_UNIFORM_MAX_LENGTH];
	GLfloat*	value;
	GLsizei		size;
	GLenum		type;
};

struct gl_uniform_node
{
	gl_uniform* uniform;
	gl_uniform_node* next;

	static NAIL_API gl_uniform_node* create(GLfloat* value, const GLchar* name, GLsizei size, GLenum type);
};

struct gl_shader
{
	gl_uniform_node* uniforms;

	GLsizei input_size;
	GLsizei output_size;

	struct vector2
	{
		float x, y;
	};

	struct vector3
	{
		float x, y, z;
	};

	struct vector4
	{
		float x, y, z, w;

		vector4():x(0.0f), y(0.0f), z(0.0f), w(0.0f){}
		vector4(float _x,float _y, float _z, float _w):x(_x), y(_y), z(_z), w(_w){}
	};

	struct matrix4
	{
		float a00, a01, a02, a03;
		float a10, a11, a12, a13;
		float a20, a21, a22, a23;
		float a30, a31, a32, a33;
	};

	vector4 multiply(matrix4 m, vector4 v)
	{
		return 
		{
			m.a00*v.x + m.a01*v.y + m.a02*v.z + m.a03*v.w,
			m.a10*v.x + m.a11*v.y + m.a12*v.z + m.a13*v.w,
			m.a20*v.x + m.a21*v.y + m.a22*v.z + m.a23*v.w,
			m.a30*v.x + m.a31*v.y + m.a32*v.z + m.a33*v.w
		};
	}


	virtual GLvoid compile() = 0;
	virtual GLvoid* process(GLvoid*) = 0;
};

struct gl_named_object
{
	GLuint name;
	GLenum type;
};

struct gl_shader_object : public gl_named_object
{
	gl_shader* shader;
};

struct gl_program_object : public gl_named_object
{
	gl_uniform*		uniforms[MAX_UNIFORMS];
	GLuint			active_uniforms;
	gl_shader_object* vertex_shader_object;
	gl_shader_object* fragment_shader_object;
};

struct gl_named_object_node
{
	gl_named_object*		object;
	gl_named_object_node*	next;
};


bool gl_shader_init();
gl_program_object* gl_find_program_object(GLuint name);
