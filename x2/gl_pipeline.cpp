#include "gl_pipeline.h"
#include "gl_shader.h"
#include <algorithm>
#include <iterator>
#include <vector>

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
		cmd->ia.indices_count = glContext.count;
	}
	cmd->ia.indices = nullptr;
	cmd->ia.primitive_type = glContext.draw_mode;
	cmd->vs.vertices_result = nullptr;
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

	//todo
	//gl_draw_command* cmd = glPpeline.command_list;
}

GLsizei get_vertex_count_from_indices(gl_draw_command* cmd)
{
	GLsizei num_vertex = 0;
	if (glContext.indices_pointer)
	{
		GLshort* indices = (GLshort*)glContext.indices_pointer;
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

void gl_vertex_shader(gl_draw_command* cmd)
{
	if (glContext.program == 0) return;
	gl_program_object* program_object = gl_find_program_object(glContext.program);
	if (program_object == nullptr) return;
	gl_shader_object* vs_object = program_object->vertex_shader_object;
	if (vs_object == nullptr) return;
	gl_shader* vs = vs_object->shader;
	if (vs == nullptr) return;

	cmd->vs.vertices_result = gl_malloc(cmd->ia.vertices_count * vs->output_size);
	for (GLsizei i = 0; i < cmd->ia.vertices_count; ++i)
	{
		GLvoid* vs_out = vs->process((GLbyte*)cmd->ia.vertices + i * vs->input_size);
		memcpy_s((GLbyte*)cmd->vs.vertices_result + i * vs->input_size, vs->output_size, vs_out, vs->output_size);
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

bool gl_is_line_culled(const gl_vector4* p1, const gl_vector4* p2,bool& baccepted,bool& bculled1,bool& bculled2)
{
	GLbyte code1 = 0;
	if (p1->x < -1.0f)	code1 |= 1;
	if (p1->x > 1.0f)	code1 |= 2;
	if (p1->y < -1.0f)	code1 |= 4;
	if (p1->y > 1.0f)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -1.0f)	code2 |= 1;
	if (p2->x > 1.0f)	code2 |= 2;
	if (p2->y < -1.0f)	code2 |= 4;
	if (p2->y > 1.0f)	code2 |= 8;
	bculled2 = (code2 != 0);
	baccepted = (code1 | code2) == 0;
	return (code1 & code2) != 0;
}

bool gl_is_triangle_culled(const gl_vector4* p1, const gl_vector4* p2, const gl_vector4* p3, bool& baccepted, bool& bculled1, bool& bculled2, bool& bculled3)
{
	GLbyte code1 = 0;
	if (p1->x < -1.0f)	code1 |= 1;
	if (p1->x > 1.0f)	code1 |= 2;
	if (p1->y < -1.0f)	code1 |= 4;
	if (p1->y > 1.0f)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -1.0f)	code2 |= 1;
	if (p2->x > 1.0f)	code2 |= 2;
	if (p2->y < -1.0f)	code2 |= 4;
	if (p2->y > 1.0f)	code2 |= 8;
	GLbyte code3 = 0;
	if (p3->x < -1.0f)	code3 |= 1;
	if (p3->x > 1.0f)	code3 |= 2;
	if (p3->y < -1.0f)	code3 |= 4;
	if (p2->y > 1.0f)	code3 |= 8;
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
	GLfloat ts[4] = { 0.0f };
	if (p1->x == p2->x)//垂直
	{
		ts[0] =  (p1->y - 1.0f)			/ (p1->y - p2->y);
		ts[1] =	(p1->y - (-1.0f))		/ (p1->y - p2->y);
	}
	else if (p1->y == p2->y)//水平
	{
		ts[2] =	 (p1->x - 1.0f)			/ (p1->x - p2->x);
		ts[3] =  (p1->x - (-1.0f))		/ (p1->x - p2->x);
	}
	else
	{
		ts[0] =		(p1->y - 1.0f)			/ (p1->y - p2->y);
		ts[1] =		(p1->y - (-1.0f))		/ (p1->y - p2->y);
		ts[2] =		(p1->x - 1.0f)			/ (p1->x - p2->x);
		ts[3] =		(p1->x - (-1.0f))		/ (p1->x - p2->x);
	}
	gl_vector4 pts[5] =
	{
		gl_vector4(0.0f, 0.0f, 0.0f, 0.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 0.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 0.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 0.0f) ,
		*p2,
	};
	std::sort(std::begin(ts), std::end(ts));
	for (auto i = 0; i < 4; ++i)
	{
		GLfloat t = ts[i];
		if(t == 0.0f) continue;
		pts[i].w = p1->w * t + (1 - t) * p2->w;
		pts[i].x = p1->x * t + (1 - t) * p2->x;
		pts[i].y = p1->y * t + (1 - t) * p2->y;
		pts[i].z = p1->z * t + (1 - t) * p2->z;
	}
	gl_vector4* p = p1;
	for (auto i = 0; i < 5; ++i)
	{
		bool bacceped = false;
		bool bculled1 = false;
		bool bculled2 = false;
		gl_vector4* pp = &pts[i];
		if(gl_is_point_degenerated(pp)) continue;//skip
		if (gl_is_line_culled(p, pp, bacceped, bculled1, bculled2))
		{
			p = pp;
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
				//just copy
				memcpy_s(node->vertices, vertex_size, p1, vertex_size);
			}
			else
			{
				
				if (pp = p2)
				{
					//just copy
					memcpy_s(node->vertices, vertex_size, p2, vertex_size);
				}
				else
				{
					//do lerp first
					gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
					GLfloat t = ts[i];
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
					{
						pt.w = p1->w * t + (1 - t) * p2->w;
						pt.x = p1->x * t + (1 - t) * p2->x;
						pt.y = p1->y * t + (1 - t) * p2->y;
						pt.z = p1->z * t + (1 - t) * p2->z;
						memcpy_s((GLbyte*)node->vertices + i * sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
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
gl_vector4 compute_intersection(const gl_vector4& prev_p, const gl_vector4& cur_p, const gl_vector2& cw_boundary_start, const gl_vector2& cw_boundary_end)
{
	return gl_vector4(0,0,0,0);
}
void do_SutherlandHodgmanClip(const std::vector<gl_vector4>& vertices_in, std::vector<gl_vector4>& vertices_out, gl_vector2 clockwise_pstart, gl_vector2 clockwise_pend)
{
	vertices_out.clear();
	for (std::vector<gl_vector4>::size_type i = 0; i < vertices_in.size(); ++i)
	{
		gl_vector4 cur_p = vertices_in[0];
		gl_vector4 pre_p = vertices_in[(i + vertices_in.size() - 1) % vertices_in.size()];
		gl_vector4 intersectin_p = compute_intersection(cur_p, pre_p, clockwise_pstart, clockwise_pend);
		if (is_in_right_side_of_boundary(clockwise_pend , clockwise_pstart, cur_p.to_vector2()))
		{
			if (!is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p.to_vector2()))
			{
				vertices_out.push_back(intersectin_p);
			}
			vertices_out.push_back(cur_p);
		}
		else if(is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p.to_vector2()))
		{
			vertices_out.push_back(intersectin_p);
		}
	}
}


gl_primitive_node* gl_do_triangle_clipping(GLvoid* v1, GLvoid* v2, GLvoid* v3, bool bculled1, bool bculled2, bool bculled3, GLsizei vertex_size)
{
	///https://en.wikipedia.org/wiki/Sutherland-Hodgman_algorithm
	gl_vector4* p1 = (gl_vector4*)v1;
	gl_vector4* p2 = (gl_vector4*)v2;
	gl_vector4* p3 = (gl_vector4*)v3;

	std::vector<gl_vector4> vertices_in = { *p1,*p2,*p3 };
	std::vector<gl_vector4> vertices_out;
	do_SutherlandHodgmanClip(vertices_in, vertices_out, gl_vector2(-1.0f, 1.0f), gl_vector2(-1.0f, -1.0f));
	do_SutherlandHodgmanClip(vertices_out, vertices_in, gl_vector2(-1.0f, 1.0f), gl_vector2(-1.0f, -1.0f));
	do_SutherlandHodgmanClip(vertices_in, vertices_out, gl_vector2(-1.0f, 1.0f), gl_vector2(-1.0f, -1.0f));
	do_SutherlandHodgmanClip(vertices_out, vertices_in, gl_vector2(-1.0f, 1.0f), gl_vector2(-1.0f, -1.0f));

	return nullptr;
}

void gl_point_assemble(gl_draw_command* cmd)
{
	gl_primitive_node* tail = nullptr;
	for (GLsizei i = 0; i < cmd->ia.vertices_count; ++i)
	{
		const GLbyte* vertex_data = (GLbyte*)cmd->vs.vertices_result + i * cmd->pa.vertex_size;
		gl_vector4* position = (gl_vector4*)(vertex_data);
		position->x = position->x / position->w;
		position->y = position->y / position->w;
		position->z = position->z / position->w;
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
		GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + (i * 2)  * cmd->pa.vertex_size;
		GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i * 2 + 1) * cmd->pa.vertex_size;
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
		bool bculled = gl_is_line_culled(position1, position2, baccepted, bculled1, bculled2);
		if (bculled)
		{
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

}

void gl_line_list_rasterize(gl_draw_command* cmd)
{

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

}

void gl_output_merge(gl_draw_command* cmd)
{

}
