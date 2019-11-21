#include "gl_memory.h"
#include "gl_context.h"

struct gl_vector2
{
	union 
	{
		float x, y;
		float u, v;
	};
};

struct gl_vector3
{
	union
	{
		float x, y, z;
		float u, v, t;
	};
};

struct gl_vector4
{
	union
	{
		float x, y, z, w;
		float r, g, b, a;
	};
};

struct gl_primitive_list
{
	GLvoid*		vertices;
	GLsizei		vertices_number;
	GLenum		primitive_type;
};

struct gl_ia_state
{
	gl_primitive_list		primitives;
};

struct gl_vs_state
{

};

struct gl_rs_state
{

};

struct gl_ps_state
{

};

struct gl_om_state
{

};

struct gl_pipeline
{
	gl_ia_state ia;
	gl_vs_state vs;
	gl_rs_state rs;
	gl_ps_state ps;
	gl_om_state om;
};

gl_pipeline glPpeline;

template<typename TVertexIn>
void gl_input_assemble()
{
	if (glContext.indices_copy)
	{
		for (int i = 0; i < MAX_VERTEX_ATTRIBUTE; ++i)
		{
			if (glContext.vertex_attribute_pointers[i].bEnabled)
			{

			}
		}
	}
	else
	{

	}

	
}