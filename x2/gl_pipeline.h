#pragma once

#include "gl_memory.h"
#include "gl_frontend.h"
#include "gl_shader.h"

#include <memory>

struct gl_vector2
{
	union 
	{
		struct
		{
			float x, y;
		};
		struct
		{
			float u, v;
		};
	};
};

struct gl_vector3
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			float u, v, t;
		};
	};
};

struct gl_vector4
{
	union
	{
		float values[4];

		struct 
		{ 
			float x, y, z, w; 
		};
		struct
		{
			float r, g, b, a;
		};
	};

	gl_vector4(float _x,float _y, float _z, float _w):x(_x),y(_y),z(_z),w(_w) { }

	template<typename T>
	void set(T* data, GLsizei size)
	{
		x = y = z = 0;
		w = 1.0f;

		for (GLsizei i = 0; i < size; ++i)
		{
			values[i] = (float)data[i];
		}
	}
};

struct gl_primitive_node
{
	GLvoid*		vertices;
	GLsizei		vertices_count;
	gl_primitive_node* next;
};

//input assemble
struct gl_ia_state
{
	GLvoid*	 indices_copy;
	GLenum   indices_type;

	GLshort* indices;
	GLsizei  indices_count;
	GLvoid*	 vertices;
	GLsizei  vertices_count;
	GLenum	 primitive_type;
};
//vertex shahder 
struct gl_vs_state
{
	GLvoid*	 vertices_result;
};

struct gl_pa_state
{
	GLsizei					vertex_size;
	GLsizei					primitive_count;
	GLenum					primitive_type;
	gl_primitive_node*		primitives;
	gl_primitive_node*		tail;
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

struct gl_draw_command
{
	gl_ia_state ia;
	gl_vs_state vs;
	gl_pa_state pa;
	gl_rs_state rs;
	gl_ps_state ps;
	gl_om_state om;

	gl_draw_command* next;
};

struct gl_pipeline
{
	gl_draw_command* command_list;
};

extern gl_pipeline glPpeline;

bool gl_pipeline_init();

void gl_emit_draw_command();

template<typename TIndex>
void gl_fill_indices_copy(gl_draw_command* cmd, const TIndex* index_data, GLsizei count)
{
	GLsizei indices_size = count * sizeof(GLshort);
	cmd->ia.indices_count = count;

	if (cmd->ia.indices == nullptr)
	{
		cmd->ia.indices = (GLshort*)gl_malloc(indices_size);
	}
	else
	{
		cmd->ia.indices = (GLshort*)gl_realloc(cmd->ia.indices, indices_size);
	}

	if (index_data == nullptr)
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			cmd->ia.indices[i] = i;
		}
	}
	else
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			cmd->ia.indices[i] = index_data[i];
		}
	}
}


GLsizei get_vertex_count_from_indices(gl_draw_command* cmd);

template <typename T>
gl_vector4 to_vector4_from_attribute(const GLvoid* pointer, GLsizei index, GLsizei stride, GLsizei size)
{
	T* start = (T*)pointer;
	gl_vector4 result(0.0f, 0.0f, 0.0f, 1.0f);
	result.set((T*)pointer, size);
	return result;
}

gl_vector4 get_attribute_from_attribute_pointer(const gl_atrribute_pointer& attr_pointer,GLsizei index);

bool gl_check_input_validity(gl_draw_command* cmd);

template<typename TVertexIn>
void gl_input_assemble(gl_draw_command* cmd)
{
	static const GLsizei vertex_size = sizeof(TVertexIn);

	cmd->ia.primitive_type = glContext.draw_mode;

	//assemble indices
	switch (cmd->ia.indices_type)
	{
	case GL_BYTE:
		gl_fill_indices_copy<GLbyte>(cmd, cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	case GL_SHORT:
		gl_fill_indices_copy<GLshort>(cmd, cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	default:
		return;
	}
	//assemble vertices
	GLsizei vertex_count = get_vertex_count_from_indices();
	cmd->ia.vertices_count = vertex_count;

	if (cmd->ia.vertices == nullptr)
	{
		cmd->ia.vertices = gl_malloc(vertex_size*vertex_count);
	}
	else
	{
		cmd->ia.vertices = gl_realloc(cmd->ia.vertices, vertex_size*vertex_count);
	}

	gl_vector4* vertices = (gl_vector4*)cmd->ia.vertices;

	for (int v = 0; v < vertex_count; ++v)
	{
		for (int i = 0; i < MAX_VERTEX_ATTRIBUTE; ++i)
		{
			gl_vector4& attribute = vertices[v*vertex_size + i * sizeof(gl_vector4)];
			if (glContext.vertex_attribute_pointers[i].bEnabled)
			{
				attribute = get_attribute_from_attribute_pointer(glContext.vertex_attribute_pointers[i], v);
			}
			else
			{
				attribute.x = glContext.vertex_attributes[i].x;
				attribute.y = glContext.vertex_attributes[i].y;
				attribute.z = glContext.vertex_attributes[i].z;
				attribute.w = glContext.vertex_attributes[i].w;
			}
		}
	}

	if (!gl_check_input_validity(cmd)) return;
}

void gl_vertex_shader(gl_draw_command* cmd);

void gl_primitive_assemble(gl_draw_command* cmd);

void gl_rasterize(gl_draw_command* cmd);

void gl_fragment_shader(gl_draw_command* cmd);

void gl_output_merge(gl_draw_command* cmd);

