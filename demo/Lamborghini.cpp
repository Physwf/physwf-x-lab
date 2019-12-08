#include "Lamborghini.h"
#include "mesh.h"
#include "gl_shader.h"
#include "glut.h"

struct LamborghiniVertexShader : public gl_shader
{

	struct VS_Input
	{
		vector4 position;
		vector4 normal;
		vector4 TextCoord;
	};

	struct VS_Output
	{
		vector4 position;
		vector4 normal;
		vector4 TextCoord;
	};

	matrix4 proj;
	matrix4 model;

	LamborghiniVertexShader()
	{
		input_size = sizeof(VS_Input);
		output_size = sizeof(VS_Output);
	}

	GLvoid compile()
	{
		uniforms = gl_uniform_node::create((GLfloat*)&proj, "proj", sizeof(matrix4), GL_FLOAT_MAT4);
		uniforms->next = gl_uniform_node::create((GLfloat*)&model, "model", sizeof(matrix4), GL_FLOAT_MAT4);
	}

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position = multiply(model, Input.position);
		Out.position = multiply(proj, Out.position);
		Out.normal = multiply(model, Input.normal);
		Out.TextCoord = Input.TextCoord;
		return Out;
	}

	VS_Output Output;
	virtual GLvoid* vs_process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};
struct LamborghiniFragmentShader : public gl_shader
{
	struct PS_Input
	{
		vector4 position;
		vector4 normal;
		vector4 TextCoord;
	};

	struct PS_Output
	{
		vector4 color;
	};

	LamborghiniFragmentShader()
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
		Out.color = vector4(1.0f,0.0f,0.0f,1.0f);
		//X_LOG("%f,%f,%f\n",Input.color.x, Input.color.y,Input.color.x);
		return Out;
	}

	PS_Output Output;
	virtual GLvoid* fs_process(GLvoid* Vertex, GLsizei screenx, GLsizei screeny)
	{
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

LamborghiniVertexShader LamborghiniVS;
LamborghiniFragmentShader LamborghiniFS;

void Lamborghini::Init()
{
	M = new Mesh();
	M->LoadFromObj("./Lamborginhi Aventador OBJ/Lamborghini_Aventador.obj");


	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(0.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	//glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &LamborghiniVS);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, &LamborghiniFS);
	Program = glCreateProgram();
	glAttachShader(Program, vs);
	glAttachShader(Program, fs);
	glLinkProgram(Program);
}

void Lamborghini::Draw()
{
	if (M)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		for (auto& Pair : M->GetSubMeshes())
		{
			const Mesh::SubMesh& Sub = Pair.second;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), Sub.Vertices.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + sizeof(Mesh::Vector3));
			glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + 2 * sizeof(Mesh::Vector3));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glUseProgram(Program);
			GLfloat proj[16] = { 0 };
			glutMatrixOrthoLH(proj, -250, 249, -250, 249, -300, 1500);
			glUnitformMatrix4fv(0, 1, false, proj);
			GLfloat model[16] = { 0 };
			static float rad = 0.0f;
			rad += 0.01f;
			glutMatrixRotationY(model, rad);
			glUnitformMatrix4fv(1, 1, false, model);

			glDrawElements(GL_TRIANGLE_LIST, Sub.Indices.size(), GL_SHORT, Sub.Indices.data());
		}
	}
	glFlush();
}
