#include "gl.h"
#include <stdlib.h>
#include "gl_shader.h"
#include "glut.h"

struct VertexShader : public gl_shader
{
	
	struct VS_Input
	{
		vector4 position;
		vector4 color;
	};

	struct VS_Output
	{
		vector4 position;
		vector4 color;
	};

	matrix4 proj;

	VertexShader()
	{
		input_size = sizeof(VS_Input);
		output_size = sizeof(VS_Output);
	}

	GLvoid compile()
	{
		uniforms = gl_uniform_node::create((GLfloat*)&proj,"proj",sizeof(matrix4), GL_FLOAT_MAT4);
	}

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position	= multiply(proj, Input.position) ;
		Out.color		= Input.color;
		return Out;
	}

	VS_Output Output;
	virtual GLvoid* process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};
struct FragmentShader : public gl_shader
{
	struct PS_Input
	{
		vector4 position;
		vector4 color;
	};

	struct PS_Output
	{
		vector4 color;
	};

	FragmentShader()
	{
		input_size = sizeof(PS_Input);
		output_size = sizeof(PS_Output);
	}

	GLvoid compile()
	{

	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		Out.color = Input.color;
		return Out;
	}

	PS_Output Output;
	virtual GLvoid* process(GLvoid* Vertex)
	{
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

VertexShader VS;
FragmentShader FS;

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
	glEnableVertexAttribArray(0);
	glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &VS);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, &FS);
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);

	GLfloat proj[16] = { 0 };
	glutMatrixOrthoLH(proj, 500, 500, 0, 500);
	glUnitformMatrix4fv(0, 1, false, proj);
}

void glRender()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINT_LIST, 0, 100);

	glFlush();
}
