#include "gl_pipeline.h"
#include "gl_shader.h"

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
