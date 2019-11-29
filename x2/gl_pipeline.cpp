#include "gl_pipeline.h"
#include "gl_shader.h"
#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
#include <assert.h>

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
	if (glContext.indices_pointer)
	{
		GLsizei indices_size = glContext.count*glContext.indices_type == GL_BYTE ? 1 : 2;
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
	cmd->vs.program = glContext.program;
	cmd->vs.vertices_result = nullptr;
	gl_program_object* program_object = gl_find_program_object(glContext.program);
	cmd->vs.vertex_size = program_object->vertex_shader_object->shader->input_size;
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

	cmd->rs.point_size = 1.0f;

	cmd->next = nullptr;
	
	for (int i = 0; i < 3; ++i)
	{
		if (glPpeline.frame_buffers[i] == nullptr)
		{
			glPpeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPpeline.front_buffer_index = -1;
			glPpeline.back_buffer_index = 0;
		}
		else if(glPpeline.frame_buffers[i]->buffer_width != glContext.viewport_width || glPpeline.frame_buffers[i]->buffer_height != glContext.viewport_height)
		{
			gl_frame_buffer::destory(glPpeline.frame_buffers[i]);
			glPpeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPpeline.front_buffer_index = -1;
			glPpeline.back_buffer_index = 0;
		}
	}
	
	GLsizei count = glContext.viewport_width * glContext.viewport_height;
	cmd->rs.fragment_buffer = (gl_fragment*)gl_malloc(glContext.viewport_width * glContext.viewport_height * sizeof(gl_fragment));
	assert(cmd->rs.fragment_buffer != nullptr);
	cmd->rs.buffer_width = glContext.viewport_width;
	cmd->rs.buffer_height = glContext.viewport_height;
	cmd->rs.clear_fragment_buffer();

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

GLsizei get_vertex_count_from_indices(gl_draw_command* cmd)
{
	GLsizei num_vertex = 0;
	if (cmd->ia.indices)
	{
		GLshort* indices = (GLshort*)cmd->ia.indices;
		for (GLsizei i = 0; i < glContext.count; ++i)
		{
			GLshort current_index = cmd->ia.indices[i] + 1;
			if (num_vertex < current_index)
			{
				num_vertex = current_index;
			}
		}
	}
	return num_vertex;
}

gl_vector4 get_attribute_from_attribute_pointer(const gl_atrribute_pointer& attr_pointer, GLsizei index)
{
	switch (attr_pointer.type)
	{
	case GL_BYTE:
		return to_vector4_from_attribute<GLbyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_BYTE:
		return to_vector4_from_attribute<GLubyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_SHORT:
		return to_vector4_from_attribute<GLshort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_SHORT:
		return to_vector4_from_attribute<GLushort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_INT:
		return to_vector4_from_attribute<GLint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_INT:
		return to_vector4_from_attribute<GLuint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_FLOAT:
		return to_vector4_from_attribute<GLfloat>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_DOUBLE:
		return to_vector4_from_attribute<GLdouble>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	default:
		return gl_vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

bool gl_check_input_validity(gl_draw_command* cmd)
{
	//check validity(todo)
	switch (glContext.draw_mode)
	{
	case GL_POINT_LIST:
		break;
	case GL_LINE_LIST:
		break;
	case GL_LINE_LIST_ADJ:
		break;
	case GL_LINE_STRIP:
		break;
	case GL_LINE_STRIP_ADJ:
		break;
	case GL_TRIANGLE_LIST:
		break;
	case GL_TRIANGLE_LIST_ADJ:
		break;
	case GL_TRIANGLE_STRIP:
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		break;
	}
	return true;
}

void gl_input_assemble(gl_draw_command* cmd)
{
	GLsizei vertex_size = cmd->vs.vertex_size;

	cmd->ia.primitive_type = glContext.draw_mode;

	//assemble indices
	switch (cmd->ia.indices_type)
	{
	case GL_BYTE:
		gl_fill_indices_copy<GLbyte>(cmd, (const GLbyte*)cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	case GL_SHORT:
		gl_fill_indices_copy<GLshort>(cmd, (const GLshort*)cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	default:
		return;
	}
	//assemble vertices
	GLsizei vertex_count = get_vertex_count_from_indices(cmd);
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
	// If an array corresponding to a generic attribute required by a vertex shader is not enabled, 
	// then the corresponding element is taken from the current generic attribute state	// OpenGLR ES Common Profile Specification Version 2.0.25 (Full Specification) (November 2, 2010)
	// 顶点属性取决于vertex shader
	GLsizei attrib_count = vertex_size / sizeof(gl_vector4);
	for (int v = 0; v < vertex_count; ++v)
	{
		for (int i = 0; i < attrib_count; ++i)
		{
			if (glContext.vertex_attribute_pointers[i].bEnabled)
			{
				gl_vector4& attribute = vertices[v*attrib_count + i];
				attribute = get_attribute_from_attribute_pointer(glContext.vertex_attribute_pointers[i], v);
			}
			else
			{
				gl_vector4& attribute = vertices[v*attrib_count + i];
				attribute.x = glContext.vertex_attributes[i].x;
				attribute.y = glContext.vertex_attributes[i].y;
				attribute.z = glContext.vertex_attributes[i].z;
				attribute.w = glContext.vertex_attributes[i].w;
			}
		}
	}

	if (!gl_check_input_validity(cmd)) return;
}

void gl_vertex_shader(gl_draw_command* cmd)
{
	if (cmd->vs.program == 0) return;
	gl_program_object* program_object = gl_find_program_object(cmd->vs.program);
	if (program_object == nullptr) return;
	gl_shader_object* vs_object = program_object->vertex_shader_object;
	if (vs_object == nullptr) return;
	gl_shader* vs = vs_object->shader;
	if (vs == nullptr) return;

	cmd->vs.vertices_result = gl_malloc(cmd->ia.vertices_count * vs->output_size);
	for (GLsizei i = 0; i < cmd->ia.vertices_count; ++i)
	{
		GLvoid* vs_out = vs->process((GLbyte*)cmd->ia.vertices + i * vs->input_size);
		memcpy_s((GLbyte*)cmd->vs.vertices_result + i * vs->output_size, vs->output_size, vs_out, vs->output_size);
	}
	cmd->pa.vertex_size = vs->output_size;
}

bool gl_is_point_culled(const gl_vector4* p)
{
	if (p->x > p->w || p->x < -p->w) return true;
	if (p->y > p->w || p->y < -p->w) return true;
	if (p->z > p->w || p->z < 0.0f) return true;
	return false;
}
//https://en.wikipedia.org/wiki/Cohen-Sutherland_algorithm
bool gl_is_line_culled(const gl_vector4* p1, const gl_vector4* p2,bool& baccepted,bool& bculled1,bool& bculled2)
{
	GLbyte code1 = 0;
	if (p1->x < -p1->w)	code1 |= 1;
	if (p1->x > p1->w)	code1 |= 2;
	if (p1->y < -p1->w)	code1 |= 4;
	if (p1->y > p1->w)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -p2->w)	code2 |= 1;
	if (p2->x > p2->w)	code2 |= 2;
	if (p2->y < -p2->w)	code2 |= 4;
	if (p2->y > p2->w)	code2 |= 8;
	bculled2 = (code2 != 0);
	baccepted = (code1 | code2) == 0;
	return (code1 & code2) != 0;
}

bool gl_is_triangle_culled(const gl_vector4* p1, const gl_vector4* p2, const gl_vector4* p3, bool& baccepted, bool& bculled1, bool& bculled2, bool& bculled3)
{
	GLbyte code1 = 0;
	if (p1->x < -p1->w)	code1 |= 1;
	if (p1->x > p1->w)	code1 |= 2;
	if (p1->y < -p1->w)	code1 |= 4;
	if (p1->y > p1->w)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -p2->w)	code2 |= 1;
	if (p2->x > p2->w)	code2 |= 2;
	if (p2->y < -p2->w)	code2 |= 4;
	if (p2->y > p2->w)	code2 |= 8;
	GLbyte code3 = 0;
	if (p3->x < -p3->w)	code3 |= 1;
	if (p3->x > p3->w)	code3 |= 2;
	if (p3->y < -p3->w)	code3 |= 4;
	if (p2->y > p3->w)	code3 |= 8;
	bculled3 = (code3 != 0);
	baccepted = (code1 | code2 | code3) == 0;
	return (code1 & code2 & code3) != 0;
}

bool gl_is_point_degenerated(gl_vector4* p)
{
	return (p->x == 0 && p->y == 0 && p->z == 0 && p->w == 0);
}

gl_primitive_node* gl_do_line_clipping(GLvoid* v1, GLvoid* v2, bool bculled1, bool bculled2, GLsizei vertex_size)
{
	gl_vector4* p1 = (gl_vector4*)v1;
	gl_vector4* p2 = (gl_vector4*)v2;
	assert(p1->w != 0 && p2->w != 0);
	GLfloat ts[4] = { 0.0f };
	if (p1->x == p2->x)//垂直
	{
		ts[0] =  (p1->y - p1->w)		/ (p1->y - p2->y);
		ts[1] =	(p1->y - (-p1->w))		/ (p1->y - p2->y);
	}
	else if (p1->y == p2->y)//水平
	{
		ts[2] =	 (p1->x - p1->w)		/ (p1->x - p2->x);
		ts[3] =  (p1->x - (-p1->w))		/ (p1->x - p2->x);
	}
	else
	{
		ts[0] =		(p1->y - p1->w)			/ (p1->y - p2->y);
		ts[1] =		(p1->y - (-p1->w))		/ (p1->y - p2->y);
		ts[2] =		(p1->x - p1->w)			/ (p1->x - p2->x);
		ts[3] =		(p1->x - (-p1->w))		/ (p1->x - p2->x);
	}
	gl_vector4 pts[5] =
	{
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		*p2,
	};
	std::sort(std::begin(ts), std::end(ts));
	int count = 0;
	for (int i = 0; i < 4; ++i)
	{
		if(ts[i]<=0) continue;
		if (i > 0 && ts[i-1] == ts[i])
		{
			continue;
		}
		ts[count++] = ts[i];
	}
	for (auto i = 0; i < count; ++i)
	{
		GLfloat t = ts[i];
		if(t <= 0.0f) continue;
		pts[i].w = p1->w * t + (1 - t) * p2->w;
		pts[i].x = p1->x * t + (1 - t) * p2->x;
		pts[i].y = p1->y * t + (1 - t) * p2->y;
		pts[i].z = p1->z * t + (1 - t) * p2->z;
	}
	gl_vector4* p = p1;
	GLfloat t = 0.0f;
	for (auto i = 0; i < 5; ++i)
	{
		bool bacceped = false;
		bool bculled1 = false;
		bool bculled2 = false;
		gl_vector4* pp = &pts[i];
		GLfloat tt = ts[i];
		if(gl_is_point_degenerated(pp)) continue;//skip
		if (gl_is_line_culled(p, pp, bacceped, bculled1, bculled2))
		{
			p = pp;
			t = tt;
			continue;
		}
		else
		{
			//baccepted must be true
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (p == p1)
			{
				memcpy_s(node->vertices, vertex_size, p1, vertex_size);//just copy
				gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
				for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))//do lerp
				{
					gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
					gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
					pt.w = p1i->w * tt + (1 - tt) * p2i->w;
					pt.x = p1i->x * tt + (1 - tt) * p2i->x;
					pt.y = p1i->y * tt + (1 - tt) * p2i->y;
					pt.z = p1i->z * tt + (1 - tt) * p2i->z;
					if (pt.w == 0.0f)
					{
						pt.w = 1.0f;
					}
					assert(pt.w != 0.0f);
					memcpy_s((gl_vector4*)((GLbyte*)node->vertices + vertex_size) + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
				}
			}
			else
			{
				if (pp = p2)
				{
					gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))//do lerp
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * t + (1 - t) * p2i->w;
						pt.x = p1i->x * t + (1 - t) * p2i->x;
						pt.y = p1i->y * t + (1 - t) * p2i->y;
						pt.z = p1i->z * t + (1 - t) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)node->vertices + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
					memcpy_s((GLbyte*)node->vertices + vertex_size, vertex_size, p2, vertex_size);//just copy
				}
				else
				{
					//do lerp
					gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * t + (1 - t) * p2i->w;
						pt.x = p1i->x * t + (1 - t) * p2i->x;
						pt.y = p1i->y * t + (1 - t) * p2i->y;
						pt.z = p1i->z * t + (1 - t) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)node->vertices + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * tt + (1 - tt) * p2i->w;
						pt.x = p1i->x * tt + (1 - tt) * p2i->x;
						pt.y = p1i->y * tt + (1 - tt) * p2i->y;
						pt.z = p1i->z * tt + (1 - tt) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)((GLbyte*)node->vertices + vertex_size) + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
				}
			}
			return node;//only one node can be produced, so just return
		}
	}
	return nullptr;
}
//clockwise inside means in right side
bool is_in_right_side_of_boundary(const gl_vector2& cw_boundary_start, const gl_vector2& cw_boundary_end, const gl_vector2& point)
{
	return cross(cw_boundary_end - cw_boundary_start, point - cw_boundary_start) >= 0;//==0表示在边界上
}
gl_vector4* compute_intersection(const gl_vector4* pre_p, const gl_vector4* cur_p, const gl_vector2& cw_boundary_start, const gl_vector2& cw_boundary_end, GLsizei vertex_size)
{
	GLfloat t = 0.0f;
	if (cw_boundary_start.x == cw_boundary_end.x)
	{
		if (cw_boundary_start.x == -1.0f)
		{
			t = (pre_p->x - (-1.0f)) / (pre_p->x - cur_p->x);
		}
		else if (cw_boundary_start.x == 1.0f)
		{
			t = (pre_p->x - 1.0f) / (pre_p->x - cur_p->x);
		}
	}
	else if(cw_boundary_start.y == cw_boundary_end.y)
	{
		if (cw_boundary_start.y == -1.0f)
		{
			t = (pre_p->y - (-1.0f)) / (pre_p->y - cur_p->y);
		}
		else if (cw_boundary_start.y == 1.0f)
		{
			t = (pre_p->y - 1.0f) / (pre_p->y - cur_p->y);
		}
	}

	gl_vector4* result = (gl_vector4*)gl_malloc(vertex_size);
	gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
	for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
	{
		const gl_vector4* pre_pi = (pre_p + i);
		const gl_vector4* cur_pi = (cur_p + i);
		pt.w = pre_pi->w * t + (1 - t) * cur_pi->w;
		pt.x = pre_pi->x * t + (1 - t) * cur_pi->x;
		pt.y = pre_pi->y * t + (1 - t) * cur_pi->y;
		pt.z = pre_pi->z * t + (1 - t) * cur_pi->z;
		memcpy_s(result + i , sizeof(gl_vector4), &pt, sizeof(gl_vector4));
	}
	return result;
}

void do_SutherlandHodgmanClip(const std::vector<const gl_vector4*>& vertices_in, std::vector<const gl_vector4*>& vertices_out,const gl_vector2& clockwise_pstart, const gl_vector2& clockwise_pend, GLsizei vertex_size)
{
	///https://en.wikipedia.org/wiki/Sutherland-Hodgman_algorithm
	vertices_out.clear();
	for (std::vector<gl_vector4>::size_type i = 0; i < vertices_in.size(); ++i)
	{
		const gl_vector4* cur_p = vertices_in[0];
		const gl_vector4* pre_p = vertices_in[(i + vertices_in.size() - 1) % vertices_in.size()];
		gl_vector4* intersectin_p = compute_intersection(cur_p, pre_p, clockwise_pstart, clockwise_pend, vertex_size);
		if (is_in_right_side_of_boundary(clockwise_pend , clockwise_pstart, cur_p->to_vector2()))
		{
			if (!is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p->to_vector2()))
			{
				vertices_out.push_back(intersectin_p);
			}
			vertices_out.push_back(cur_p);
		}
		else if(is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p->to_vector2()))
		{
			vertices_out.push_back(intersectin_p);
		}
	}
	//clockwise sort
	std::sort(vertices_out.begin(), vertices_out.end(), [](const gl_vector4* p1, const gl_vector4* p2) {
		if (p1->x < p2->x)
		{
			return -1;
		}
		else if (p1->x > p2->x)
		{
			return 1;
		}
		else
		{
			if (p1->y < p2->y)
			{
				return -1;
			}
			else if (p1->y > p2->y)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	});
}


gl_primitive_node* gl_do_triangle_clipping(const GLvoid* v1, const GLvoid* v2, const GLvoid* v3, GLsizei vertex_size, GLsizei& primitives_count, gl_primitive_node** tail)
{
	const gl_vector4* p1 = (gl_vector4*)v1;
	const gl_vector4* p2 = (gl_vector4*)v2;
	const gl_vector4* p3 = (gl_vector4*)v3;

	std::vector<const gl_vector4*> vertices_in = { p1, p2, p3 };
	std::vector<const gl_vector4*> vertices_out;
	std::set<const gl_vector4*> all_vertices;
	do_SutherlandHodgmanClip(vertices_in, vertices_out, gl_vector2(-1.0f, 1.0f), gl_vector2(1.0f, 1.0f), vertex_size);
	all_vertices.insert(vertices_in.begin(), vertices_in.end());
	do_SutherlandHodgmanClip(vertices_out, vertices_in, gl_vector2(1.0f, 1.0f), gl_vector2(1.0f, -1.0f), vertex_size);
	all_vertices.insert(vertices_out.begin(), vertices_out.end());
	do_SutherlandHodgmanClip(vertices_in, vertices_out, gl_vector2(1.0f, -1.0f), gl_vector2(-1.0f, -1.0f), vertex_size);
	all_vertices.insert(vertices_in.begin(), vertices_in.end());
	do_SutherlandHodgmanClip(vertices_out, vertices_in, gl_vector2(-1.0f, -1.0f), gl_vector2(-1.0f, 1.0f), vertex_size);
	//内存回收
	for (const gl_vector4* v : all_vertices)
	{
		if(v == p1 || v == p2 || v == p3) continue;
		gl_free((GLvoid*)v);
	}
	if (vertices_in.size() > 0)
	{
		assert(vertices_in.size() >= 3);
		gl_primitive_node* node = nullptr;
		for (std::vector<const gl_vector4*>::size_type i = 0; i < vertices_in.size()-2; i++)
		{
			node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			node->vertices = gl_malloc(vertex_size * 3);
			node->vertices_count = 3;
			const GLvoid* vertices[3] = { vertices_in[0], vertices_in[i + 1], vertices_in[i + 2] };
			for (auto i = 0; i < 3; ++i)
			{
				memcpy_s((GLbyte*)node->vertices + i * vertex_size, vertex_size, vertices[i], vertex_size);
			}
		}
	}
	return nullptr;
}

void gl_point_assemble(gl_draw_command* cmd)
{
	gl_primitive_node* tail = nullptr;
	for (GLsizei i = 0; i < cmd->ia.vertices_count; ++i)
	{
		const GLbyte* vertex_data = (GLbyte*)cmd->vs.vertices_result + i * cmd->pa.vertex_size;
		gl_vector4* position = (gl_vector4*)(vertex_data);
		if (gl_is_point_culled(position)) continue;
		gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
		node->vertices = gl_malloc(cmd->pa.vertex_size);
		memcpy_s(node->vertices, cmd->pa.vertex_size, vertex_data, cmd->pa.vertex_size);
		node->next = nullptr;
		node->vertices_count = 1;
		if (tail == nullptr)
		{
			cmd->pa.primitives = node;
		}
		else
		{
			tail->next = node;
		}
		tail = node;
		++cmd->pa.primitive_count;
	}
}

void gl_line_list_assemble(gl_draw_command* cmd)
{
	for (GLsizei i = 0; i < cmd->ia.vertices_count; i+=2)
	{
		GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + (i + 0) * cmd->pa.vertex_size;
		GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i + 1) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		if (position2->w == 0.0f)
		{
			position2->w = 1.0f;
		}
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		bool bculled = gl_is_line_culled(position1, position2, baccepted, bculled1, bculled2);
		if (bculled)
		{
			printf("bculled");
			continue;//cliped
		}
		else if(baccepted)
		{
			//todo z culling
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			gl_primitive_node* node = gl_do_line_clipping(vertex_data1, vertex_data2, bculled1, bculled2, cmd->pa.vertex_size);
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
	}
}

void gl_line_list_adj_assemble(gl_draw_command* cmd) { }

void gl_line_strip_assemble(gl_draw_command* cmd)
{
	for (GLsizei i = 0; i < cmd->ia.vertices_count-1; ++i)
	{
		GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + cmd->pa.vertex_size;
		GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i + 1) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		position1->x = position1->x / position1->w;
		position1->y = position1->y / position1->w;
		position1->z = position1->z / position1->w;
		position2->x = position2->x / position2->w;
		position2->y = position2->y / position2->w;
		position2->z = position2->z / position2->w;
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		if (gl_is_line_culled(position1, position2, baccepted, bculled1, bculled2))
		{
			continue;
		}
		else if (baccepted)
		{
			//todo z culling
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			if (gl_primitive_node* node = gl_do_line_clipping(vertex_data1, vertex_data2, bculled1, bculled2, cmd->pa.vertex_size))
			{
				if (cmd->pa.tail == nullptr)
				{
					cmd->pa.primitives = node;
				}
				else
				{
					cmd->pa.tail->next = node;
				}
				cmd->pa.tail = node;
				++cmd->pa.primitive_count;
			}
		}
	}
}

void gl_line_strip_adj_assemble(gl_draw_command* cmd) { }

void gl_triangle_list_assemble(gl_draw_command* cmd)
{ 
	gl_primitive_node* tail = nullptr;
	for (GLsizei i = 0; i < cmd->ia.vertices_count; i+=3)
	{
		const GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + (i * 3) * cmd->pa.vertex_size;
		const GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i * 3 + 1) * cmd->pa.vertex_size;
		const GLbyte* vertex_data3 = (GLbyte*)cmd->vs.vertices_result + (i * 3 + 2) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		gl_vector4* position3 = (gl_vector4*)(vertex_data3);
		position1->x = position1->x / position1->w;
		position1->y = position1->y / position1->w;
		position1->z = position1->z / position1->w;
		position2->x = position2->x / position2->w;
		position2->y = position2->y / position2->w;
		position2->z = position2->z / position2->w;
		position3->x = position3->x / position3->w;
		position3->y = position3->y / position3->w;
		position3->z = position3->z / position3->w;
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		bool bculled3 = false;
		if (gl_is_triangle_culled(position1, position2, position3, baccepted, bculled1, bculled2, bculled3))
		{
			continue;
		}
		else if (baccepted)
		{
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 3;
			node->vertices = gl_malloc(primitive_size);
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			GLsizei primitive_count = 0;
			if (gl_primitive_node* node = gl_do_triangle_clipping(vertex_data1, vertex_data2, vertex_data3, cmd->pa.vertex_size, primitive_count, &cmd->pa.tail))
			{
				if (cmd->pa.tail == nullptr)
				{
					cmd->pa.primitives = node;
				}
				else
				{
					tail->next = node;
				}
				cmd->pa.tail = node;
				cmd->pa.primitive_count += primitive_count;
			}
		}
	}
}

void gl_triangle_list_adj_assemble(gl_draw_command* cmd) { }

void gl_triangle_strip_assemble(gl_draw_command* cmd)
{ 

}

void gl_triangle_strip_adj_assemble(gl_draw_command* cmd) { }

void gl_primitive_assemble(gl_draw_command* cmd)
{
	switch (cmd->ia.primitive_type)
	{
	case GL_POINT_LIST:
		gl_point_assemble(cmd);
		break;
	case GL_LINE_LIST:
		gl_line_list_assemble(cmd);
		break;
	case GL_LINE_LIST_ADJ:
		gl_line_list_adj_assemble(cmd);
		break;
	case GL_LINE_STRIP:
		gl_line_strip_assemble(cmd);
		break;
	case GL_LINE_STRIP_ADJ:
		gl_line_strip_adj_assemble(cmd);
		break;
	case GL_TRIANGLE_LIST:
		gl_triangle_list_assemble(cmd);
		break;
	case GL_TRIANGLE_LIST_ADJ:
		gl_triangle_list_adj_assemble(cmd);
		break;
	case GL_TRIANGLE_STRIP:
		gl_triangle_strip_assemble(cmd);
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		gl_triangle_strip_adj_assemble(cmd);
		break;
	}
}


void gl_point_rasterize(gl_draw_command* cmd)
{
	gl_primitive_node* node = cmd->pa.primitives;
	while (node)
	{
		gl_vector4* position = (gl_vector4*)node->vertices;
		position->x /= position->w;
		position->y /= position->w;
		position->z /= position->w;
		GLsizei x = (GLsizei)(position->x * cmd->pa.viewport_width);
		GLsizei y = (GLsizei)(position->y * cmd->pa.viewport_height);
		if (x >= 0 && x < cmd->pa.viewport_width && y >= 0 && y < cmd->pa.viewport_height)
		{
			gl_fragment& fragment = cmd->rs.fragment_buffer[y*cmd->pa.viewport_width + x];
			GLfloat depth = gl_campf(position->z);
			if (fragment.depth > position->z)
			{
				assert(depth >= 0.0f && depth <= 1.0f);
				fragment.depth = depth;
				fragment.varing_attribute = node->vertices;
			}
		}
		node = node->next;
	}
}

void gl_line_list_rasterize(gl_draw_command* cmd)
{
	gl_primitive_node* node = cmd->pa.primitives;
	while (node)
	{
		gl_vector4* p1 = (gl_vector4*)node->vertices;
		gl_vector4* p2 = (gl_vector4*)((GLbyte*)node->vertices + cmd->pa.vertex_size);
		p1->x /= p1->w;
		p1->y /= p1->w;
		p1->z /= p1->w;
		p2->x /= p2->w;
		p2->y /= p2->w;
		p2->z /= p2->w;
		assert(p2->w != 0.0f && p1->w != 0.0f);
		GLfloat x1 = (p1->x * cmd->pa.viewport_width);
		GLfloat y1 = (p1->y * cmd->pa.viewport_height);
		GLfloat x2 = (p2->x * cmd->pa.viewport_width);
		GLfloat y2 = (p2->y * cmd->pa.viewport_height);
		GLfloat k = (y2 - y1) / (x2 - x1);
		GLsizei xstart, ystart, xend, yend;
		if (x1 <= x2)
		{
			xstart	= (GLsizei)x1 - 1;
			xend	= (GLsizei)x2 + 1;
		}
		else
		{
			xstart	= (GLsizei)x2 - 1;
			xend	= (GLsizei)x1 - 1;
		}
		if (y1 <= y2)
		{
			ystart	= (GLsizei)y1 - 1;
			yend	= (GLsizei)y2 + 1;
		}
		else
		{
			ystart	= (GLsizei)y2 - 1;
			yend	= (GLsizei)y1 + 1;
		}
		xstart = glClamp(xstart, 0, cmd->pa.viewport_width);
		xend = glClamp(xend, 0, cmd->pa.viewport_width);
		ystart = glClamp(ystart, 0, cmd->pa.viewport_height);
		yend = glClamp(yend, 0, cmd->pa.viewport_height);
		GLsizei xstep = xend - xstart > 0 ? 1 : -1;
		GLsizei ystep = yend - ystart > 0 ? 1 : -1;
		bool bxmajor = k >= -1.0f && k <= 1.0f;
		for (GLsizei y = ystart; y != yend;y+=ystep)
		{
			for (GLsizei x = xstart; x != xend; x+=xstep)
			{
				if (gl_is_in_diamond(gl_vector2(x1,y1), gl_vector2(x2, y2), x, y, bxmajor))
				{
					gl_fragment& fragment = cmd->rs.get_fragment(x, y);
					GLfloat t = bxmajor ? (x - x1) / (x2 - x1) : (y - y1) / (y2 - y1);
					t = gl_campf(t);
					GLfloat depth = gl_campf(gl_lerp(p1->z, p2->z, t));
					if (fragment.depth > depth)
					{
						fragment.depth = depth;
						fragment.varing_attribute = gl_lerp(p1, p2, cmd->pa.vertex_size / sizeof(gl_vector4), t);
					}
				}
			}
		}

// 		GLfloat k = (y2 - y1) / (x2 - x1);
// 		GLfloat d = 0.0f;
// 		if (k >= -1 && k <= 1)//x-major
// 		{
// 			GLsizei step = x1 - x2 > 0 ? -1 : 1;
// 			GLsizei xstart = (GLsizei)x1;
// 			GLsizei ystart = (GLsizei)y1;
// 			GLsizei xend = (GLsizei)x2;
// 			d = y1 - ystart;
// 			if (glAbs<GLfloat>(x1 - xstart) + glAbs<GLfloat>(y - ystart) < 0.5f)
// 			{
// 				gl_fragment* fragment = (gl_fragment*)gl_malloc(sizeof(gl_fragment));
// 				cmd->rs.fragment_buffer[y*cmd->pa.viewport_width + x] = fragment;
// 				fragment->depth = gl_campf(p1->z);
// 				fragment->varing_attribute = node->vertices;
// 			}
// 			for (GLsizei x = xstart+1; x != xend; x+=step)
// 			{
// 				GLfloat y = y1 + k * (x - x1);
// 				if (glAbs<GLfloat>(y - ystart) < 0.5f)
// 				{
// 					GLfloat t = (x - x1) / (x2 - x1);
// 					gl_fragment* fragment = (gl_fragment*)gl_malloc(sizeof(gl_fragment));
// 					cmd->rs.fragment_buffer[y*cmd->pa.viewport_width + x] = fragment;
// 					fragment->depth = gl_campf(gl_lerp(p1->z, p2->z, t));
// 					fragment->varing_attribute = gl_lerp(p1, p2,cmd->pa.vertex_size/(sizeof(gl_vector4)));
// 				}
// 			}
// 		}
// 		else//y-major
// 		{
// 			for (GLsizei y = y1, x = x1; y != y2; y += step)
// 			{
// 				gl_fragment* fragment = (gl_fragment*)gl_malloc(sizeof(gl_fragment));
// 				cmd->rs.fragment_buffer[y*cmd->pa.viewport_width + x] = fragment;
// 				//fragment->depth = gl_campf(position->z);
// 				d += k;
// 				if (step == 1)
// 				{
// 					if (d >= 1.0f) d -= 1.0f;
// 					if (d > 0.5f) x += step;
// 				}
// 				else
// 				{
// 					if (d <= -1.0f) d += 1.0f;
// 					if (d < -0.5f) x += step;
// 				}
// 				fragment->varing_attribute = node->vertices;
// 			}
// 		}
		node = node->next;
	}
}

void gl_line_list_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_line_strip_rasterize(gl_draw_command* cmd)
{

}

void gl_line_strip_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_list_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_list_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_strip_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_strip_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_rasterize(gl_draw_command* cmd)
{
	cmd->rs.clear_fragment_buffer();
	switch (cmd->ia.primitive_type)
	{
	case GL_POINT_LIST:
		gl_point_rasterize(cmd);
		break;
	case GL_LINE_LIST:
		gl_line_list_rasterize(cmd);
		break;
	case GL_LINE_LIST_ADJ:
		gl_line_list_adj_rasterize(cmd);
		break;
	case GL_LINE_STRIP:
		gl_line_strip_rasterize(cmd);
		break;
	case GL_LINE_STRIP_ADJ:
		gl_line_strip_adj_rasterize(cmd);
		break;
	case GL_TRIANGLE_LIST:
		gl_triangle_list_rasterize(cmd);
		break;
	case GL_TRIANGLE_LIST_ADJ:
		gl_triangle_list_adj_rasterize(cmd);
		break;
	case GL_TRIANGLE_STRIP:
		gl_triangle_strip_rasterize(cmd);
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		gl_triangle_strip_adj_rasterize(cmd);
		break;
	}
}

void gl_fragment_shader(gl_draw_command* cmd)
{
	if (cmd->vs.program == 0) return;
	gl_program_object* program_object = gl_find_program_object(cmd->vs.program);
	if (program_object == nullptr) return;
	gl_shader_object* fs_object = program_object->fragment_shader_object;
	if (fs_object == nullptr) return;
	gl_shader* fs = fs_object->shader;
	if (fs == nullptr) return;

	for (GLsizei y = 0; y < cmd->rs.buffer_height; ++y)
	{
		for (GLsizei x = 0; x < cmd->rs.buffer_width; ++x)
		{
			gl_fragment& fragment = cmd->rs.get_fragment(x, y);
			gl_frame_buffer* frame_buffer = glPpeline.frame_buffers[glPpeline.back_buffer_index];
			if (fragment.depth < 1.0f && fragment.depth >= 0.0f)
			{
				GLvoid* fs_out = fs->process((GLbyte*)fragment.varing_attribute);
				gl_vector4& color = ((gl_vector4*)fs_out)[0];
				frame_buffer->set_color(x, y, color);
			}
		}
	}
}

void gl_set_frame_buffer(GLsizei x, GLsizei y, const gl_vector4& color, GLfloat depth)
{
	gl_frame_buffer* buffer = glPpeline.frame_buffers[glPpeline.back_buffer_index];
	buffer->set_color(x, y, color);
	buffer->set_depth(x, y, depth);
}
void gl_output_merge(gl_draw_command* cmd)
{

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
