#include "gl_pipeline.h"
#include "gl_shader.h"

#include <memory>

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