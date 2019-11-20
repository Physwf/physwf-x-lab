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

struct gl_point
{
	gl_vector3 v;
};

struct gl_line
{
	gl_vector3 v1, v2;
};

struct gl_triangle
{
	gl_vector3 v1, v2, v3;
};

struct gl_quad
{
	gl_vector3 v1, v2, v3, v4;
};

void gl_input_assemble()
{

}