/* Copyright(C) 2019-2020 Physwf

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < http://www.gnu.org/licenses/>.
*/

#pragma once
#include "NAIL.h"
#include <cmath>

#include "gl_objects.h"
#include "gl_shader.h"


#define ACTIVE_UNIFORM_MAX_LENGTH	64
#define MAX_UNIFORMS				128

struct gl_uniform
{
	GLchar		name[ACTIVE_UNIFORM_MAX_LENGTH];
	GLfloat*	fvalue;
	GLuint		ivalue;
	GLsizei		size;
	GLenum		type;
};

struct gl_uniform_node
{
	gl_uniform* uniform;
	gl_uniform_node* next;

	static NAIL_API gl_uniform_node* create(GLfloat* value, const GLchar* name, GLsizei size, GLenum type);
	static NAIL_API gl_uniform_node* create(GLuint* value, const GLchar* name, GLsizei size, GLenum type);
};



struct gl_shader
{
	gl_uniform_node* uniforms;

	GLsizei input_size;
	GLsizei output_size;

	struct vector2
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

		vector2() :x(0.0f), y(0.0f) {}
		vector2(float _x, float _y) :x(_x), y(_y) {}
	};

	struct vector3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			struct
			{
				float u, v, w;
			};
		};


		vector3() :x(0.0f), y(0.0f), z(0.0f) {}
		vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	};

	struct vector4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float u, v, w, t;
			};
		};

		vector3 to_vector3() { return vector3(x,y,z); }

		vector4():x(0.0f), y(0.0f), z(0.0f), w(1.0f){}
		vector4(float _x,float _y, float _z, float _w):x(_x), y(_y), z(_z), w(_w){}
	};

	struct matrix4
	{
		float a00, a01, a02, a03;
		float a10, a11, a12, a13;
		float a20, a21, a22, a23;
		float a30, a31, a32, a33;
	};

	typedef unsigned int sampler2D;
	typedef unsigned int samplerCube;

	vector4 multiply(const matrix4& m, const vector4& v)
	{
		return 
		{
			m.a00*v.x + m.a01*v.y + m.a02*v.z + m.a03*v.w,
			m.a10*v.x + m.a11*v.y + m.a12*v.z + m.a13*v.w,
			m.a20*v.x + m.a21*v.y + m.a22*v.z + m.a23*v.w,
			m.a30*v.x + m.a31*v.y + m.a32*v.z + m.a33*v.w
		};
	}

	float clamp(float v) 
	{ 
		if (v < 0.0f) return 0.0f; 
		if (v > 1.0f) return 1.0f;
		return v;
	}
	vector4 add(const vector4& v1, const vector4& v2) { return vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
	vector3 add(const vector3& v1, const vector3& v2) { return vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
	vector2 add(const vector2& v1, const vector2& v2) { return vector2(v1.x + v2.x, v1.y + v2.y); }
	vector4 sub(const vector4& v1, const vector4& v2) { return vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);}
	vector3 sub(const vector3& v1, const vector3& v2) { return vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
	vector2 sub(const vector2& v1, const vector2& v2) { return vector2(v1.x - v2.x, v1.y - v2.y); }
	float dot(const vector4& v1, const vector4& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w); }
	float dot(const vector3& v1, const vector3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }
	float dot(const vector2& v1, const vector2& v2) { return (v1.x * v2.x + v1.y * v2.y); }
	//vector4 multiply(const vector4& v1, const vector4& v2) {  }
	vector3 multiply(const vector3& v1, const vector3& v2) { return vector3(v1.y*v2.z-v2.y*v1.z, v1.z*v2.x-v2.z*v1.x, v1.x*v2.y-v2.x*v1.y); }
	float multiply(const vector2& v1, const vector2& v2) { return v1.x*v2.y - v2.x*v1.y; }

	vector4 normalize(const vector4& v)
	{
		float sqrt = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
		return vector4(v.x / sqrt, v.y / sqrt, v.z / sqrt, v.w / sqrt);
	}
	vector3 normalize(const vector3& v)
	{
		float sqrt = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
		return vector3(v.x / sqrt, v.y / sqrt, v.z / sqrt);
	}
	vector2 normalize(const vector2& v)
	{
		float sqrt = std::sqrt(v.x*v.x + v.y*v.y);
		return vector2(v.x / sqrt, v.y / sqrt);
	}

	vector4 texture(sampler2D sampler, vector2 texCoord)
	{
		vector4 result;
		gl_sample_texture2d(sampler, texCoord.u, texCoord.v, (GLfloat*)&result);
		return result;
	}

	vector4 texture(samplerCube sampler, vector3 texCoord)
	{
		vector4 result;
		gl_sample_texture_cube(sampler, texCoord.u, texCoord.v, texCoord.w, (GLfloat*)&result);
		return result;
	}

	virtual GLvoid compile() = 0;
	virtual GLvoid* process(GLvoid*) = 0;
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

gl_program_object* gl_find_program_object(GLuint name);
