#include "gl_memory.h"
#include "gl_utilities.h"

struct gl_buffer_node
{
	GLuint id;
	GLvoid* data;
	GLsizei size;

	gl_buffer_node* next;
};

void gl_buffer_init();
void gl_buffer_uninit();

GLuint gl_create_buffer();
void gl_allocate_buffer(GLuint buffer, GLsizei size,GLvoid* data);
void gl_realse_buffer(GLuint buffer);
