#include "gl_memory.h"
#include "gl_frontend.h"

struct gl_vector2
{
	union 
	{
		struct
		{
			float x, y;
		}
		struct
		{
			float u, v;
		}
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

	gl_vector4(float _x,float _y, float _z, float _w):x(_x),y(_y),z(_z),y(_y) { }

	template<typename T>
	void set(T* data, GLsizei size)
	{
		x = y = z = 0;
		w = 1.0f;

		for (GLsizei i = 0; i < size; ++i)
		{
			values[i] = data[i];
		}
	}
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

template <typename TIndex>
GLsizei get_vertex_count_from_indices()
{
	GLsizei num_vertex = 0;
	if (glContext.indices_copy)
	{
		TIndex* indices = (TIndex*)glContext.indices_copy;
		for (GLsizei = 0; i < glContext.count; ++i)
		{
			TIndex current_index = glContext.indices_copy[i] + 1;
			if (num_vertex < current_index)
			{
				num_vertex = current_index;
			}
		}
	}
}
template <typename T>
gl_vector4 to_vector4_from_attribute(GLvoid* pointer, GLsizei index, GLsizei stride, GLsizei size)
{
	T* start = (T*)pointer;
	gl_vector4 result(0.0f, 0.0f, 0.0f, 1.0f);
	result.set((T*)pointer, size);
	return result;
}

gl_vector4 get_attribute_from_attribute_pointer(const gl_atrribute_pointer& attr_pointer,GLsizei index)
{
	switch (attr_pointer.type)
	{
	case GL_BYTE:
		return to_vector4_from_attribute<GLbyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_UNSIGNED_BYTE:
		return to_vector4_from_attribute<GLubyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_SHORT:
		return to_vector4_from_attribute<GLshort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_UNSIGNED_SHORT:
		return to_vector4_from_attribute<GLushort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_INT:
		return to_vector4_from_attribute<GLint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_UNSIGNED_INT:
		return to_vector4_from_attribute<GLuint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_FLOAT:
		return to_vector4_from_attribute<GLfloat>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	case GL_DOUBLE:
		return to_vector4_from_attribute<GLdouble>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.stride);
	}
}

template<typename TVertexIn>
void gl_input_assemble()
{
	static const GLsizei vertex_size = sizeof(TVertexIn);

	GLsizei vertex_count = 0;

	if (glContext.indices_copy)
	{
		vertex_count = glContext.indices_type == GL_BYTE ?  get_vertex_count_from_indices<GLbyte>() : get_vertex_count_from_indices<GLshort>();
	}
	else
	{
		vertex_count = glContext.count;
	}

	glPpeline.ia.primitives = gl_malloc(vertex_size*vertex_count);
	gl_vector4* primitives = (gl_vector4*)glPpeline.ia.primitives;

	for (int v = 0; v < vertex_count; ++v)
	{
		for (int i = 0; i < MAX_VERTEX_ATTRIBUTE; ++i)
		{
			if (glContext.vertex_attribute_pointers[i].bEnabled)
			{
				gl_vector4& attribute = primitives[v*vertex_size + i * sizeof(gl_vector4)];
				attribute = get_attribute_from_attribute_pointer(glContext.vertex_attribute_pointers[i],v);
			}
			else
			{

			}
		}
	}

}
