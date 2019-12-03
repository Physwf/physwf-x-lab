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

#include "gl_buffer.h"
#include "gl_frontend.h"

#include <memory>

gl_buffer_node head;

void gl_buffer_init()
{
	head.id = 0;
	head.data = nullptr;
	head.size = 0;
	head.next = nullptr;
}

void gl_buffer_uninit()
{
	gl_buffer_node* node = head.next;
	head.next = nullptr;
	while (node)
	{
		if (node->data)
		{
			gl_free(node->data);
			node->data = nullptr;
		}
		node->id = 0;
		node->size = 0;
		node = node->next;
		gl_free(node);
	}
}

GLuint gl_create_buffer()
{
	gl_buffer_node* node = &head;
	GLuint id = 1;
	while (node != nullptr)
	{
		id = node->id + 1;
		if (!node->next) break;
		node = node->next;
	}
	gl_buffer_node* new_node = (gl_buffer_node*)gl_malloc(sizeof(gl_buffer_node));
	new_node->id = id;
	new_node->size = 0;
	new_node->data = nullptr;
	new_node->next = nullptr;
	node->next = new_node;
	return id;
}

void gl_allocate_buffer(GLuint buffer, GLsizei size, GLvoid* data)
{
	gl_buffer_node* node = head.next;
	while (node)
	{
		if (node->id == buffer)
		{
			node->data = gl_malloc(size);
			memcpy_s(node->data, size, data, size);
			return;
		}
		node = node->next;
	}
	glSetError(GL_INVALID_OPERATION, "illegal buffer!");
}

void gl_realse_buffer(GLuint buffer)
{
	gl_buffer_node* node = head.next;
	gl_buffer_node* pre_node = &head;
	while (node)
	{
		if (node->id == buffer)
		{
			if (node->data)
			{
				gl_free(node->data);
				node->data = nullptr;
				pre_node->next = node->next;
				gl_free(node);
			}
			return;
		}
		pre_node = node;
		node = node->next;
	}
	glSetError(GL_INVALID_OPERATION, "illegal buffer!");
}

