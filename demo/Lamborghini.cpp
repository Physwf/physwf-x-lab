#include "Lamborghini.h"
#include "mesh.h"
#include "gl_shader.h"
#include "glut.h"
#include "light.h"
#include "material.h"

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
		//size = sizeof(LamborghiniVertexShader);
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
	struct AmbientLight
	{
		vector4 Color;
	};
	struct DirectionalLight
	{
		vector4 Direction;
		vector4 Color;
	};
	struct PointLight
	{
		vector4 Position;
		vector4 Color;
	};
	struct Material
	{
		vector4 ambient;
		vector4 diffuse;
	};

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
		//size = sizeof(LamborghiniFragmentShader);
		input_size = sizeof(PS_Input);
		output_size = sizeof(PS_Output);
	}

	GLvoid compile()
	{
		uniforms = gl_uniform_node::create((GLuint*)&diffuse, "diffuse", sizeof(sampler2D), GL_UNSIGNED_INT);
		uniforms->next = gl_uniform_node::create((GLuint*)&specular, "specular", sizeof(sampler2D), GL_UNSIGNED_INT);
		uniforms->next->next = gl_uniform_node::create((GLfloat*)&material.ambient, "material.ambient", sizeof(material.ambient), GL_FLOAT_VEC4);
		uniforms->next->next->next = gl_uniform_node::create((GLfloat*)&material.diffuse, "material.diffuse", sizeof(material.diffuse), GL_FLOAT_VEC4);
		uniforms->next->next->next->next = gl_uniform_node::create((GLfloat*)&AL.Color, "AL.Color", sizeof(AL.Color), GL_FLOAT_VEC4);
		uniforms->next->next->next->next->next = gl_uniform_node::create((GLfloat*)&DL.Color, "DL.Color", sizeof(DL.Color), GL_FLOAT_VEC4);
		uniforms->next->next->next->next->next->next = gl_uniform_node::create((GLfloat*)&DL.Direction, "DL.Direction", sizeof(DL.Direction), GL_FLOAT_VEC4);
	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		Out.color = vector4(1.0f,0.0f,0.0f,1.0f);
		vector3 normal = normalize(Input.normal.to_vector3());
		vector4 ac = multiply(material.ambient, AL.Color);
		vector4 dc = multiply(material.diffuse, multiply(dot(normal, DL.Direction.to_vector3()), DL.Color));
		Out.color = add( ac,dc ) ;
		//X_LOG("%f,%f,%f\n",Input.color.x, Input.color.y,Input.color.x);
		return Out;
	}

	PS_Output Output;

	sampler2D diffuse;
	sampler2D specular;
	Material material;

	AmbientLight AL;
	DirectionalLight DL;
	PointLight PLs[8];

	virtual GLvoid* fs_process(GLvoid* Vertex, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, screenx, screeny);
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
	glCompileShader(fs);
	Program = glCreateProgram();
	glAttachShader(Program, vs);
	glAttachShader(Program, fs);
	glLinkProgram(Program);
	glUseProgram(Program);
	GLfloat proj[16] = { 0 };
	glutMatrixOrthoLH(proj, -250, 249, -250, 249, -300, 1500);
	glUnitformMatrix4fv(0, 1, false, proj);
	

	AmbientLight AL;
	AL.Color = LinearColor(0.4f, 0.4f, 0.4f, 1.0f);
	DirctionalLight DL;
	DL.Color = LinearColor(0.6f, 0.6f, 0.6f, 1.0f);
	DL.Direction = { 1.6f, 1.6f, 1.6f };

	GLint loc = glGetUniformLocation(Program, "AL.Color");
	glUniform4fv(loc, 1, &AL.Color.r);
	loc = glGetUniformLocation(Program, "DL.Color");
	glUniform4fv(loc, 1, &DL.Color.r);
	loc = glGetUniformLocation(Program, "DL.Direction");
	glUniform4fv(loc, 1, &DL.Direction.x);
}

void Lamborghini::Draw()
{
	if (M)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		GLfloat model[16] = { 0 };
		static float rad = 0.0f;
		rad += 0.01f;
		glutMatrixRotationY(model, rad);
		glUnitformMatrix4fv(1, 1, false, model);

		for (auto& Pair : M->GetSubMeshes())
		{
			const Mesh::SubMesh& Sub = Pair.second;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), Sub.Vertices.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + sizeof(Vector));
			glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + 2 * sizeof(Vector));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			const Material& Mtl = M->GetMaterial(Sub.MaterialName);
			GLint loc = glGetUniformLocation(Program, "material.ambient");
			glUniform4fv(loc, 1, &Mtl.Ka.r);
			loc = glGetUniformLocation(Program, "material.diffuse");
			glUniform4fv(loc, 1, &Mtl.Kd.r);
			glDrawElements(GL_TRIANGLE_LIST, Sub.Indices.size(), GL_SHORT, Sub.Indices.data());
		}
		glFlush();
	}
}
