#include "gl.h"
#include <stdlib.h>
#include "gl_shader.h"

struct VertexShader : public gl_shader
{
	VertexShader()
	{

	}

	struct VS_Input
	{
		vector3 position;
		vector3 color;
	};

	struct VS_Output
	{
		vector3 position;
		vector3 color;
	};

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position	= Input.position;
		Out.color		= Input.color;
		return Output;
	}

	VS_Output Output;
	virtual GLvoid* process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};

VertexShader VS;

void glInit()
{
	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(0.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

GLfloat points[3*100];
GLfloat lines[3*100*2];

void glSetup()
{
	for (auto& Value : points)
	{
		Value = (GLfloat)rand() / 500;
	}
	for (auto& Value : lines)
	{
		Value = (GLfloat)rand() / 500;
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, points);
	glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &VS);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, NULL);
}

void glRender()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINT_LIST, 0, 100);

	glFlush();
}
