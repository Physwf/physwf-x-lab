#include "gl_pipeline.h"

#include <cassert>
#include <memory>

gl_vector2 operator+(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return gl_vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}
gl_vector2 operator-(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return gl_vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

float operator*(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y);
}

gl_vector2 operator/(const gl_vector2& lhs, float divider)
{
	return { lhs.x / divider,lhs.y / divider };
}

float cross(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return (lhs.x* rhs.y - rhs.x*lhs.y);
}

gl_vector3 operator+(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return gl_vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
gl_vector3 operator-(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return gl_vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.y + rhs.y);
}
float operator*(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

gl_pipeline glPpeline;

bool gl_pipeline_init()
{
	glPpeline.command_list = nullptr;
	glPpeline.command_tail = nullptr;

	for (int i = 0; i < 3; ++i)
	{
		glPpeline.frame_buffers[i] = nullptr;// gl_frame_buffer::create(512, 512);
	}
	glPpeline.front_buffer_index = -1;
	glPpeline.back_buffer_index = 0;

	gl_shader_init();

	return true;
}

void gl_emit_draw_command()
{
	gl_draw_command* cmd = (gl_draw_command*)gl_malloc(sizeof(gl_draw_command));
	cmd->next = nullptr;

	//ia
	if (glContext.indices_pointer)
	{
		GLsizei indices_size = glContext.count*(glContext.indices_type == GL_BYTE ? 1 : 2);
		cmd->ia.indices_copy = gl_malloc(indices_size);
		memcpy_s(cmd->ia.indices_copy, indices_size, glContext.indices_pointer, indices_size);
		cmd->ia.indices_count = glContext.count;
	}
	else
	{
		cmd->ia.indices = nullptr;
		cmd->ia.indices_copy = nullptr;
		cmd->ia.indices_count = glContext.count;
	}
	cmd->ia.indices = nullptr;
	cmd->ia.primitive_type = glContext.draw_mode;
	cmd->ia.vertices = nullptr;
	cmd->ia.indices_type = glContext.indices_type;
	//vs
	cmd->vs.program = glContext.program;
	cmd->vs.vertices_result = nullptr;
	gl_program_object* program_object = gl_find_program_object(glContext.program);
	cmd->vs.vertex_size = program_object->vertex_shader_object->shader->input_size;
	//pa
	cmd->pa.primitive_type = cmd->ia.primitive_type;
	cmd->pa.primitives = nullptr;
	cmd->pa.tail = nullptr;
	cmd->pa.primitive_count = 0;
	cmd->pa.viewport_x = glContext.viewport_x;
	cmd->pa.viewport_y = glContext.viewport_y;
	cmd->pa.viewport_width = glContext.viewport_width;
	cmd->pa.viewport_height = glContext.viewport_height;
	cmd->pa.depth_far = glContext.depth_far;
	cmd->pa.depth_near = glContext.depth_near;
	cmd->pa.front_face = glContext.front_face;
	cmd->pa.cull_face = glContext.cull_face;
	//rs
	GLenum front_face = glContext.front_face;
	GLenum back_face = glContext.front_face == GL_CCW ? GL_CW : GL_CCW;
	cmd->rs.cull_mode = glContext.cull_face == GL_FRONT ? front_face : back_face;//ÌÞ³ý¹æÔò
	cmd->rs.point_size = 1.0f;
	GLsizei count = glContext.viewport_width * glContext.viewport_height;
	cmd->rs.fragment_buffer = (gl_fragment*)gl_malloc(glContext.viewport_width * glContext.viewport_height * sizeof(gl_fragment));
	assert(cmd->rs.fragment_buffer != nullptr);
	cmd->rs.buffer_width = glContext.viewport_width;
	cmd->rs.buffer_height = glContext.viewport_height;
	cmd->rs.clear_fragment_buffer();

	for (int i = 0; i < 3; ++i)
	{
		if (glPpeline.frame_buffers[i] == nullptr)
		{
			glPpeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPpeline.front_buffer_index = -1;
			glPpeline.back_buffer_index = 0;
		}
		else if (glPpeline.frame_buffers[i]->buffer_width != glContext.viewport_width || glPpeline.frame_buffers[i]->buffer_height != glContext.viewport_height)
		{
			gl_frame_buffer::destory(glPpeline.frame_buffers[i]);
			glPpeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPpeline.front_buffer_index = -1;
			glPpeline.back_buffer_index = 0;
		}
	}

	if (glContext.clear_bitmask & GL_COLOR_BUFFER_BIT)
	{
		glPpeline.frame_buffers[glPpeline.back_buffer_index]->set_clearcolor(gl_vector4(glContext.clear_color.r, glContext.clear_color.g, glContext.clear_color.b, glContext.clear_color.a));
	}
	if (glContext.clear_bitmask & GL_DEPTH_BUFFER_BIT)
	{
		glPpeline.frame_buffers[glPpeline.back_buffer_index]->set_cleardepth(glContext.clear_depth);
	}
	if (glContext.clear_bitmask & GL_STENCIL_BUFFER_BIT)
	{

	}

	//todo multi-thread
	if (glPpeline.command_list == nullptr)
	{
		glPpeline.command_list = cmd;
		glPpeline.command_tail = cmd;
	}
	else
	{
		glPpeline.command_tail->next = cmd;
	}
}

void gl_do_draw()
{
	while (glPpeline.command_list)
	{
		gl_draw_command* cmd = glPpeline.command_list;
		glPpeline.command_list = cmd->next;
		gl_input_assemble(cmd);
		gl_vertex_shader(cmd);
		gl_primitive_assemble(cmd);
		gl_rasterize(cmd);
		gl_fragment_shader(cmd);
		gl_output_merge(cmd);
		gl_swap_frame_buffer(cmd);
	}
}

void gl_swap_frame_buffer(gl_draw_command* cmd)
{
	glPpeline.front_buffer_index = glPpeline.back_buffer_index;
	glPpeline.back_buffer_index++;
	glPpeline.back_buffer_index %= 3;
}

void gl_set_frame_buffer(GLsizei x, GLsizei y, const gl_vector4& color, GLfloat depth)
{
	gl_frame_buffer* buffer = glPpeline.frame_buffers[glPpeline.back_buffer_index];
	buffer->set_color(x, y, color);
	buffer->set_depth(x, y, depth);
}

NAIL_API void gl_copy_front_buffer(unsigned char* rgbbuffer)
{
	gl_frame_buffer* fb = glPpeline.frame_buffers[glPpeline.front_buffer_index];
	for (GLsizei y = 0; y < fb->buffer_height; ++y)
	{
		for (GLsizei x = 0; x < fb->buffer_width; ++x)
		{
			gl_vector4 color = fb->get_color(x, y);
			GLsizei rgb_index = (y*fb->buffer_width + x) * 4;
			rgbbuffer[rgb_index + 0] = (unsigned char)(gl_campf(color.b) * 255);
			rgbbuffer[rgb_index + 1] = (unsigned char)(gl_campf(color.g) * 255);
			rgbbuffer[rgb_index + 2] = (unsigned char)(gl_campf(color.r) * 255);
		}
	}

// 	for (LONG y = 0; y < Height; ++y)
// 	{
// 		for (LONG x = 0; x < Width; ++x)
// 		{
// 			pFrameBuffer[4 * y * Width + x * 4 + 0] = 255;
// 			pFrameBuffer[4 * y * Width + x * 4 + 1] = 0;
// 			pFrameBuffer[4 * y * Width + x * 4 + 2] = 0;
// 		}
// 	}
}
