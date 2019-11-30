#pragma once

#include "gl_memory.h"
#include "gl_frontend.h"
#include "gl_shader.h"
#include "gl_utilities.h"


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
	gl_vector2():x(0.0f), y(0.0f) { }
	gl_vector2(float _x, float _y):x(_x),y(_y) { }

	gl_vector2 operator-()
	{
		return gl_vector2(-x, -y);
	};
};

gl_vector2 operator+(const gl_vector2& lhs, const gl_vector2& rhs);
gl_vector2 operator-(const gl_vector2& lhs, const gl_vector2& rhs);
float operator*(const gl_vector2& lhs, const gl_vector2& rhs);
gl_vector2 operator/(const gl_vector2& lhs, float divider);
float cross(const gl_vector2& lhs, const gl_vector2& rhs);


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

	gl_vector3() :x(0.0f), y(0.0f),z(0.0f) { }
	gl_vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z)  { }
};

gl_vector3 operator+(const gl_vector3& lhs, const gl_vector3& rhs);
gl_vector3 operator-(const gl_vector3& lhs, const gl_vector3& rhs);
float operator*(const gl_vector3& lhs, const gl_vector3& rhs);

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

	gl_vector2 to_vector2() const
	{
		return gl_vector2(x, y);
	}
	gl_vector3 to_vector3() const
	{
		return gl_vector3(x, y, z);
	}

	gl_vector4() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	gl_vector4(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) { }

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

inline GLfloat gl_campf(GLfloat v)
{
	return glClamp(v, 0.0f, 1.0f);
}

inline GLfloat gl_lerp(GLfloat v1, GLfloat v2, GLfloat t)
{
	return v1 + (v2 - v1)*t;
}

inline gl_vector4 gl_lerp(const gl_vector4& v1, const gl_vector4& v2, GLfloat t)
{
	gl_vector4 result;
	result.x = gl_lerp(v1.x, v2.x, t);
	result.y = gl_lerp(v1.y, v2.y, t);
	result.z = gl_lerp(v1.z, v2.z, t);
	result.w = gl_lerp(v1.w, v2.w, t);
	return result;
}

inline gl_vector4* gl_lerp(const gl_vector4* attrb1, const gl_vector4* attrb2, GLsizei n, GLfloat t)
{
	gl_vector4* result = (gl_vector4*)gl_malloc(n * sizeof(gl_vector4));
	for (GLsizei i = 0; i < n; ++i)
	{
		*(result + i) = gl_lerp(attrb1[i], attrb2[i], t);
	}
	return result;
}

inline bool gl_is_line_segment_intersect(const gl_vector2& a, const gl_vector2& b, const gl_vector2& c, const gl_vector2& d)
{
	gl_vector2 ac = a - c;
	gl_vector2 ad = a - d;
	gl_vector2 bc = b - c;
	gl_vector2 bd = b - d;
	gl_vector2 ca = -ac;
	gl_vector2 da = -ad;
	gl_vector2 cb = -bc;
	gl_vector2 db = -bd;
	return cross(ac, ad) * cross(bc, bd) <= 0.0f && cross(ca, cb) * cross(da, db) <= 0.0f;
}

inline bool gl_is_point_in_line_segment(const gl_vector2& p1, const gl_vector2& p2, const gl_vector2& p)
{
	if (p.x > glMin(p1.x, p2.x) && p.x < glMax(p1.x, p1.x) && p.y > glMin(p1.y, p2.y) && p.y < glMax(p1.y, p1.y))
	{
		return cross(p1 - p, p2 - p) == 0;
	}
	return false;
}

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
	GLsizei		vertex_size;
	GLuint		program;
	GLvoid*		vertices_result;
};

struct gl_pa_state
{
	GLint					viewport_x;
	GLint					viewport_y;
	GLsizei					viewport_width;
	GLsizei					viewport_height;

	GLclampf				depth_near;
	GLclampf				depth_far;

	GLenum					front_face;
	GLenum					cull_face;
	GLsizei					vertex_size;
	GLsizei					primitive_count;
	GLenum					primitive_type;
	gl_primitive_node*		primitives;
	gl_primitive_node*		tail;
};

struct gl_frame_buffer
{
	GLsizei buffer_width;
	GLsizei buffer_height;
	GLfloat* depth_buffer;
	gl_vector4* color_buffer;

	static gl_frame_buffer* create(GLsizei w, GLsizei h)
	{
		if (w <= 0 || h <= 0) return nullptr;
		gl_frame_buffer* result = (gl_frame_buffer*)gl_malloc(sizeof(gl_frame_buffer));
		result->buffer_width = w;
		result->buffer_height = h;
		GLsizei count = w * h;
		result->depth_buffer = (GLfloat*)gl_malloc(count * sizeof(GLfloat));
		result->color_buffer = (gl_vector4*)gl_malloc(count * sizeof(gl_vector4));
		return result;
	}

	static void destory(gl_frame_buffer* buffer)
	{
		gl_free(buffer->depth_buffer);
		gl_free(buffer->color_buffer);
		gl_free(buffer);
	}

	void set_depth(GLsizei x, GLsizei y, GLfloat depth)
	{
		if (x < 0 || x > buffer_width) return;
		if (y < 0 || y > buffer_height) return;
		depth_buffer[y*buffer_width + x] = depth;
	}

	void set_cleardepth(GLfloat depth)
	{
		for (GLsizei y = 0; y < buffer_height; ++y)
		{
			for (GLsizei x = 0; x < buffer_width; ++x)
			{
				depth_buffer[y*buffer_width + x] = depth;
			}
		}
	}

	GLfloat get_depth(GLsizei x, GLsizei y)
	{
		if (x < 0) x = 0;
		if (x > buffer_width) x = buffer_width;
		if (y < 0) y = 0;
		if (y > buffer_height) y = buffer_height;
		return depth_buffer[y*buffer_width + x];
	}

	void set_clearcolor(const gl_vector4& color)
	{
		for (GLsizei y = 0; y < buffer_height; ++y)
		{
			for (GLsizei x = 0; x < buffer_width; ++x)
			{
				color_buffer[y*buffer_width + x] = color;
			}
		}
	}

	void set_color(GLsizei x, GLsizei y, const gl_vector4& color)
	{
		if (x < 0 || x > buffer_width) return;
		if (y < 0 || y > buffer_height) return;
		color_buffer[y*buffer_width + x] = color;
	}

	gl_vector4 get_color(GLsizei x, GLsizei y)
	{
		if (x < 0) x = 0;
		if (x >= buffer_width) x = buffer_width-1;
		if (y < 0) y = 0;
		if (y >= buffer_height) y = buffer_height-1;
		return color_buffer[y*buffer_width + x];
	}

};

struct gl_fragment
{
	GLfloat depth;
	GLvoid* varing_attribute;
};

struct gl_rs_state
{
	GLfloat point_size;

	GLsizei buffer_width;
	GLsizei buffer_height;
	gl_fragment* fragment_buffer;

	void clear_fragment_buffer()
	{
		for (GLsizei y = 0; y < buffer_height; ++y)
		{
			for (GLsizei x = 0; x < buffer_width; ++x)
			{
				GLsizei index = y * buffer_height + x;
				fragment_buffer[index].depth = 1.0f;
				fragment_buffer[index].varing_attribute = nullptr;
			}
		}
	}

	gl_fragment& get_fragment(GLsizei x, GLsizei y)
	{
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x >= buffer_width) x = buffer_width - 1;
		if (y >= buffer_height) y = buffer_height - 1;
		return fragment_buffer[y*buffer_width+x];
	}
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
	gl_named_object_node* named_object_list;

	gl_frame_buffer* frame_buffers[3];
	GLuint back_buffer_index;
	GLuint front_buffer_index;

	gl_draw_command* command_list;
	gl_draw_command* command_tail;
};

extern gl_pipeline glPpeline;

bool gl_pipeline_init();
void gl_emit_draw_command();
void gl_do_draw();
void gl_swap_frame_buffer(gl_draw_command* cmd);

NAIL_API void gl_copy_front_buffer(unsigned char* rgbbuffer);

void gl_input_assemble(gl_draw_command* cmd);

void gl_vertex_shader(gl_draw_command* cmd);

void gl_primitive_assemble(gl_draw_command* cmd);

void gl_rasterize(gl_draw_command* cmd);

void gl_fragment_shader(gl_draw_command* cmd);

void gl_output_merge(gl_draw_command* cmd);

