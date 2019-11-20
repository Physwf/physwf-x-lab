#include "gl_memory.h"

struct gl_vector2
{
	float x, y;
};

struct gl_vector3
{
	float x, y, z;
};

struct gl_vector4
{
	float x, y, z, w;
};

struct gl_primitive_list
{
	gl_vector3* vertices;
	GLsizei		vertices_number;
	GLenum		primitive_type;
};

struct gl_ia_stage
{
	GLvoid* indices_copy;
	GLenum index_type;
	GLsizei indices_number;

	gl_primitive_list		primitives;
};

void gl_input_assemble()
{

}