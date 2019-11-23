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

struct gl_primitive_list
{
	GLvoid*		vertices;
	GLsizei		vertices_number;
	GLenum		primitive_type;
};

//input assemble
struct gl_ia_state
{
	GLshort* indices;
	GLsizei  indices_count;
	GLvoid*	 vertices;
	GLsizei  vertices_count;
};
//vertex shahder 
struct gl_vs_state
{
	GLvoid*	 vertices_result;
};

struct gl_pa_state
{
	gl_primitive_list		primitives;
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
	gl_pa_state pa;
	gl_rs_state rs;
	gl_ps_state ps;
	gl_om_state om;
};

extern gl_pipeline glPpeline;

bool gl_pipeline_init();

template<typename TIndex>
void gl_fill_indices_copy(const TIndex* index_data, GLsizei count)
{
	GLsizei indices_size = count * sizeof(GLshort);
	glPpeline.ia.indices_count = count;

	if (glPpeline.ia.indices == nullptr)
	{
		glPpeline.ia.indices = (GLshort*)gl_malloc(indices_size);
	}
	else
	{
		glPpeline.ia.indices = (GLshort*)gl_realloc(glPpeline.ia.indices, indices_size);
	}

	if (index_data == nullptr)
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			glPpeline.ia.indices[i] = i;
		}
	}
	else
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			glPpeline.ia.indices[i] = index_data[i];
		}
	}
}


GLsizei get_vertex_count_from_indices()
{
	GLsizei num_vertex = 0;
	if (glContext.indices_pointer)
	{
		GLshort* indices = (GLshort*)glContext.indices_pointer;
		for (GLsizei i = 0; i < glContext.count; ++i)
		{
			GLshort current_index = glPpeline.ia.indices[i] + 1;
			if (num_vertex < current_index)
			{
				num_vertex = current_index;
			}
		}
	}
	return num_vertex;
}

template <typename T>
gl_vector4 to_vector4_from_attribute(const GLvoid* pointer, GLsizei index, GLsizei stride, GLsizei size)
{
	T* start = (T*)pointer;
	gl_vector4 result(0.0f, 0.0f, 0.0f, 1.0f);
	result.set((T*)pointer, size);
	return result;
}

gl_vector4 get_attribute_from_attribute_pointer(const gl_atrribute_pointer& attr_pointer,GLsizei index);

bool gl_check_input_validity();

template<typename TVertexIn>
void gl_input_assemble()
{
	static const GLsizei vertex_size = sizeof(TVertexIn);

	//assemble indices
	switch (glContext.indices_type)
	{
	case GL_BYTE:
		gl_fill_indices_copy<GLbyte>(glContext.indices_pointer, glContext.count);
		break;
	case GL_SHORT:
		gl_fill_indices_copy<GLshort>(glContext.indices_pointer, glContext.count);
		break;
	default:
		return;
	}
	//assemble vertices
	GLsizei vertex_count = get_vertex_count_from_indices();
	glPpeline.ia.vertices_count = vertex_count;

	if (glPpeline.ia.vertices == nullptr)
	{
		glPpeline.ia.vertices = gl_malloc(vertex_size*vertex_count);
	}
	else
	{
		glPpeline.ia.vertices = gl_realloc(glPpeline.ia.vertices, vertex_size*vertex_count);
	}

	gl_vector4* vertices = (gl_vector4*)glPpeline.ia.vertices;

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

	if (!gl_check_input_validity()) return;
}

void gl_vertex_shader()
{
	if (glContext.program == 0) return;
	gl_program_object* program_object = gl_find_program_object(glContext.program);
	if (program_object == nullptr) return;
	gl_shader_object* vs_object = program_object->vertex_shader_object;
	if (vs_object == nullptr) return;
	gl_shader* vs = vs_object->shader;
	if (vs == nullptr) return;

	glPpeline.vs.vertices_result = gl_malloc(glPpeline.ia.vertices_count * vs->output_size);
	for (GLsizei i = 0; i < glPpeline.ia.vertices_count; ++i)
	{
		GLvoid* vs_out = vs->process((GLbyte*)glPpeline.ia.vertices + i * vs->input_size);
		memcpy_s((GLbyte*)glPpeline.vs.vertices_result + i * vs->input_size, vs->output_size, vs_out, vs->output_size);
	}
}

void gl_primitive_assemable()
{

}

