#pragma once


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
};

struct gl_shader
{
	gl_uniform_node* uniforms;

	GLsizei input_size;
	GLsizei output_size;

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
