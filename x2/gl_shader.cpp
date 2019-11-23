#include "gl_frontend.h"
#include "gl_shader.h"
#include "gl_memory.h"

#include <string>

gl_named_object_node named_object_list;

bool gl_shader_init()
{
	named_object_list.object->name = 0;
	named_object_list.next = nullptr;
	return true;
}

GLuint gl_create_named_object(GLenum type)
{
	GLsizei object_size = 0; 
	gl_named_object* object = nullptr;
	switch (type)
	{
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:
	{
		gl_shader_object* shader_object = (gl_shader_object*)gl_malloc(sizeof(gl_shader_object));
		shader_object->shader = nullptr;
		object = shader_object;
	}
		break;
	case GL_PROGRAM:
	{
		gl_program_object* program_object = (gl_program_object*)gl_malloc(sizeof(gl_program_object));
		for (gl_uniform*& uniform : program_object->uniforms)
		{
			uniform = nullptr;
		}
		program_object->vertex_shader_object = nullptr;
		program_object->fragment_shader_object = nullptr;
		object = program_object;
	}
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Invalid shader type!");
		return 0;
	}

	gl_named_object_node* node = &named_object_list;
	while (!node->next)
	{
		node = node->next;
	}
	gl_named_object_node* new_node = (gl_named_object_node*)gl_malloc(sizeof(gl_named_object_node));
	node->next = new_node;

	object->name = node->object->name + 1;
	object->type = type;
	new_node->object = object;
	new_node->next = nullptr;
	return object->name;
}

gl_named_object_node* gl_find_named_object(GLuint name)
{
	gl_named_object_node* node = &named_object_list;
	while (node->next)
	{
		node = node->next;
		if (node->object->name == name) return node;
	}
	return nullptr;
}

gl_program_object* gl_find_program_object_check(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	gl_named_object* object = node->object;
	if (object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	if (object->type != GL_PROGRAM)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	return (gl_program_object*)node->object;
}

void gl_destroy_named_object(GLuint name)
{
	gl_named_object_node* node = &named_object_list;
	while (node->next)
	{
		gl_named_object_node* prenode = node;
		node = node->next;
		if (node->object->name == name)
		{
			prenode->next = node->next;
			node->next = nullptr;
			gl_free(node);
			break;
		}
	}
	glSetError(GL_INVALID_VALUE, "Invalid shader name!");
}


gl_program_object* gl_find_program_object(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr) return nullptr;
	gl_named_object* object = node->object;
	if (object == nullptr) return nullptr;
	if (object->type != GL_PROGRAM)
	{
		return nullptr;
	}
	return (gl_program_object*)object;
}

NAIL_API GLuint glCreateShader(GLenum type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:
		return gl_create_named_object(type);
		break;
	default:
		glSetError(GL_INVALID_ENUM, "invalid shader type");
	}
	return 0;
}

NAIL_API void glShaderSource(GLuint shader, GLvoid* source)
{
	gl_named_object_node* node = gl_find_named_object(shader);
	if (node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_named_object* object = node->object;
	if (object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	if (object->type != GL_VERTEX_SHADER && object->type != GL_FRAGMENT_SHADER)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)object;
	shader_object->shader = (gl_shader*)source;
}

NAIL_API void glDeleteShader(GLuint shader)
{
	gl_destroy_named_object(shader);
}

NAIL_API GLuint glCreateProgram()
{
	return gl_create_named_object(GL_PROGRAM);
}

NAIL_API void glAttachShader(GLuint program, GLuint shader)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;

	gl_named_object_node* shader_node = gl_find_named_object(shader);
	if (shader_node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)shader_node->object;
	if (shader_object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}

	if (shader_object->type == GL_VERTEX_SHADER)
	{
		if (program_object->vertex_shader_object != nullptr)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have vertex shader!");
			return;
		}
		else
		{
			program_object->vertex_shader_object = shader_object;
		}
	}
	else if (shader_object->type == GL_FRAGMENT_SHADER)
	{
		if (program_object->fragment_shader_object != nullptr)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have fragment shader!");
			return;
		}
		else
		{
			program_object->fragment_shader_object = shader_object;
		}
	}
}

NAIL_API void glDetachShader(GLuint program, GLuint shader)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;
	
	gl_named_object_node* shader_node = gl_find_named_object(shader);
	if (shader_node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)shader_node->object;
	if (shader_object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}

	if (shader_object->type == GL_VERTEX_SHADER)
	{
		if (program_object->vertex_shader_object != shader_object)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have vertex shader!");
			return;
		}
		else
		{
			program_object->vertex_shader_object = nullptr;
		}
	}
	else if (shader_object->type == GL_FRAGMENT_SHADER)
	{
		if (program_object->fragment_shader_object != shader_object)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have fragment shader!");
			return;
		}
		else
		{
			program_object->fragment_shader_object = nullptr;
		}
	}
}

NAIL_API void glLinkProgram(GLuint program)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;

	gl_shader_object* vs_object = program_object->vertex_shader_object;
	gl_shader_object* fs_object = program_object->fragment_shader_object;
	if (vs_object == nullptr || fs_object == nullptr)
	{
		glSetError(GL_INVALID_OPERATION, "shader is not attached!");
		return;
	}
	gl_shader* vs = vs_object->shader;
	gl_shader* fs = fs_object->shader;
	if (vs == nullptr || fs == nullptr)
	{
		glSetError(GL_INVALID_OPERATION, "shader has no source!");
		return;
	}
	GLsizei index = 0;

	gl_uniform_node* uniform_node = vs->uniforms;
	while (uniform_node)
	{
		program_object->uniforms[index] = uniform_node->uniform;
		uniform_node = uniform_node->next;
		++index;
		if (index >= MAX_UNIFORMS)
		{
			glSetError(GL_INVALID_VALUE, "Too much uniform!");
			return;
		}
	}
	uniform_node = fs->uniforms;
	while (uniform_node)
	{
		program_object->uniforms[index] = uniform_node->uniform;
		uniform_node = uniform_node->next;
		++index;
		if (index >= MAX_UNIFORMS)
		{
			glSetError(GL_INVALID_VALUE, "Too much uniform!");
			return;
		}
	}
	program_object->active_uniforms = index;
}

NAIL_API void glUseProgram(GLuint program)
{
	if (gl_find_program_object_check(program) == nullptr) return;

	glContext.program = program;
}

NAIL_API void glDeleteProgram(GLuint program)
{

}

NAIL_API GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	gl_named_object_node* node = gl_find_named_object(program);
	if (program_object == nullptr) return -1;
	
	for (GLuint i = 0; i < program_object->active_uniforms; ++i)
	{
		gl_uniform* uniform = program_object->uniforms[i];
		if (uniform == nullptr)
		{
			return -1;//this should not happen
		}
		if (strcmp(uniform->name, name) == 0) return i;
	}
	return -1;
}

NAIL_API void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;
	if (index >= program_object->active_uniforms)
	{
		glSetError(GL_INVALID_VALUE, "Invalid index!");
		return;
	}
	gl_uniform* uniform = program_object->uniforms[index];
	if (length != nullptr) *length = strlen(uniform->name);
	if (size != nullptr) *size = uniform->size;
	if (type != nullptr) *type = uniform->size;
	if (name != nullptr) strcpy_s(name, bufsize, uniform->name); 
}

GLfloat* gl_get_uniform_check(GLint location)
{
	if (glContext.program == 0)
	{
		glSetError(GL_INVALID_OPERATION, "No program is used!");
		return nullptr;
	}
	gl_program_object* program_object = gl_find_program_object_check(glContext.program);
	if (program_object == nullptr) return nullptr;
	if (location >= (GLint)program_object->active_uniforms)
	{
		glSetError(GL_INVALID_VALUE, "Invalid index!");
		return nullptr;
	}
	return (GLfloat*)program_object->uniforms[location]->value;
}

NAIL_API void glUniform1i(GLint location, GLint v0)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v0;
}

NAIL_API void glUniform2i(GLint location, GLint v0, GLint v1)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v0;
	value[1] = (GLfloat)v1;
}

NAIL_API void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v0;
	value[1] = (GLfloat)v1;
	value[2] = (GLfloat)v2;
}

NAIL_API void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v0;
	value[1] = (GLfloat)v1;
	value[2] = (GLfloat)v2;
	value[3] = (GLfloat)v3;
}

NAIL_API void glUniform1f(GLint location, GLfloat v0)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v0;
}

NAIL_API void glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v0;
	value[1] = v1;
}

NAIL_API void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v0;
	value[1] = v1;
	value[2] = v2;
}

NAIL_API void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v0;
	value[1] = v1;
	value[2] = v2;
	value[3] = v3;
}

NAIL_API void glUniform1fv(GLint location, GLsizei count, const GLfloat *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v[0];
}

NAIL_API void glUniform2fv(GLint location, GLsizei count, const GLfloat *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v[0];
	value[1] = v[1];
}

NAIL_API void glUniform3fv(GLint location, GLsizei count, const GLfloat *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v[0];
	value[1] = v[1];
	value[2] = v[2];
}

NAIL_API void glUniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = v[0];
	value[1] = v[1];
	value[2] = v[2];
	value[3] = v[3];
}

NAIL_API void glUniform1iv(GLint location, GLsizei count, const GLint *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
}
NAIL_API void glUniform2iv(GLint location, GLsizei count, const GLint *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
}

NAIL_API void glUniform3iv(GLint location, GLsizei count, const GLint *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
	value[2] = (GLfloat)v[2];
}

NAIL_API void glUniform4iv(GLint location, GLsizei count, const GLint *v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
	value[2] = (GLfloat)v[2];
	value[3] = (GLfloat)v[3];
}

NAIL_API void glUnitformMatrix2fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
	value[4] = (GLfloat)v[2];
	value[5] = (GLfloat)v[3];
}

NAIL_API void glUnitformMatrix3fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
	value[2] = (GLfloat)v[2];
	value[4] = (GLfloat)v[3];
	value[5] = (GLfloat)v[4];
	value[6] = (GLfloat)v[5];
}

NAIL_API void glUnitformMatrix4fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	GLfloat* value = gl_get_uniform_check(location);
	if (value == nullptr) return;
	value[0] = (GLfloat)v[0];
	value[1] = (GLfloat)v[1];
	value[2] = (GLfloat)v[2];
	value[3] = (GLfloat)v[3];
	value[4] = (GLfloat)v[4];
	value[5] = (GLfloat)v[5];
	value[6] = (GLfloat)v[6];
	value[7] = (GLfloat)v[7];
}