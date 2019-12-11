#include "Lamborghini.h"
#include "mesh.h"
#include "gl_shader.h"
#include "glut.h"
#include "light.h"
#include "material.h"
#include <cmath>

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
		create_uniform((GLfloat*)&proj, "proj", sizeof(matrix4), GL_FLOAT_MAT4);
		create_uniform((GLfloat*)&model, "model", sizeof(matrix4), GL_FLOAT_MAT4);
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

struct LamborghiniFragmentShaderBase : public gl_shader
{
	struct AmbientLight
	{
		vector3 Color;
	};
	struct DirectionalLight
	{
		vector3 Direction;
		vector3 Color;
	};
	struct PointLight
	{
		vector3 Position;
		vector3 Color;
	};
	struct Material
	{
		vector3 Ka;
		vector3 Kd;
		vector3 Ks;
		vector3 Tf;
		float Ni;
		float Ns;
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

	LamborghiniFragmentShaderBase()
	{
		//size = sizeof(LamborghiniFragmentShader);
		input_size = sizeof(PS_Input);
		output_size = sizeof(PS_Output);
	}

	virtual GLvoid compile()
	{
		create_uniform((GLfloat*)&AL.Color, "AL.Color", sizeof(AL.Color), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&DL.Color, "DL.Color", sizeof(DL.Color), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&DL.Direction, "DL.Direction", sizeof(DL.Direction), GL_FLOAT_VEC3);
	}

	
	PS_Output Output;

	

	Material material;

	AmbientLight AL;
	DirectionalLight DL;
	PointLight PLs[8];

	
};

struct LamborghiniFragmentShaderBody : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLuint*)&diffuseSampler2D, "diffuseSampler2D", sizeof(sampler2D), GL_UNSIGNED_INT);
		create_uniform((GLuint*)&specularSampler2D, "specularSampler2D", sizeof(sampler2D), GL_UNSIGNED_INT);

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ks, "material.Ks", sizeof(material.Ks), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
		create_uniform((GLfloat*)&material.Ns, "material.Ns", sizeof(material.Ns), GL_FLOAT);
	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		//ambient
		vector3 ambientColor = multiply(AL.Color, material.Ka);
		//diffuse
		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);
		float lamb = clamp(dot(normal, light_dir));
		vector3 lambColor = multiply(material.Kd, multiply(lamb, DL.Color));
		//specular
		vector3 half = add(light_dir , vector3(0.0f, 0.0f, -1.0f));
		half = normalize(half);
		float ndoth = clamp(dot(normal, half));
		float intensity = std::powf(ndoth, material.Ns);
		vector3 specularColor = multiply(intensity, material.Ks);

		vector3 color = add(ambientColor, lambColor);
		color = add(specularColor, color);

		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}

	sampler2D diffuseSampler2D;
	sampler2D specularSampler2D;
};

struct LamborghiniFragmentShaderCollider : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		//ambient
		vector3 ambientColor = multiply(AL.Color, material.Ka);
		//diffuse
		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);
		float lamb = clamp(dot(normal, light_dir));
		vector3 lambColor = multiply(material.Kd, multiply(lamb, DL.Color));
		//specular
		vector3 half = add(light_dir, vector3(0.0f, 0.0f, -1.0f));
		half = normalize(half);
		float ndoth = clamp(dot(normal, half));
		float intensity = std::powf(ndoth, material.Ns);
		vector3 specularColor = multiply(intensity, material.Ks);

		vector3 color = add(ambientColor, lambColor);
		color = add(specularColor, color);

		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

struct LamborghiniFragmentShaderGlass : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ks, "material.Ks", sizeof(material.Ks), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
		create_uniform((GLfloat*)&material.Ns, "material.Ns", sizeof(material.Ns), GL_FLOAT);
	}


	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		//ambient
		vector3 ambientColor = multiply(AL.Color, material.Ka);
		//diffuse
		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);
		float lamb = clamp(dot(normal, light_dir));
		vector3 lambColor = multiply(material.Kd, multiply(lamb, DL.Color));
		//specular
		vector3 half = add(light_dir, vector3(0.0f, 0.0f, -1.0f));
		half = normalize(half);
		float ndoth = clamp(dot(normal, half));
		float intensity = std::powf(ndoth, material.Ns);
		vector3 specularColor = multiply(intensity, material.Ks);

		vector3 color = add(ambientColor, lambColor);
		color = add(specularColor, color);

		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

LamborghiniVertexShader LamborghiniVS;
LamborghiniFragmentShaderBody LamborghiniBodyFS;
LamborghiniFragmentShaderCollider LamborghiniColliderFS;
LamborghiniFragmentShaderGlass LamborghiniGlassFS;

void Lamborghini::Init()
{
	M = new Mesh();
	M->LoadFromObj("./Lamborginhi Aventador OBJ/Lamborghini_Aventador.obj");


	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &LamborghiniVS);
	glCompileShader(vs);

	GLuint body_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(body_fs, &LamborghiniBodyFS);
	glCompileShader(body_fs);

	BodyProgram = glCreateProgram();
	glAttachShader(BodyProgram, vs);
	glAttachShader(BodyProgram, body_fs);
	glLinkProgram(BodyProgram);

	GLuint collider_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(collider_fs, &LamborghiniColliderFS);
	glCompileShader(collider_fs);

	ColliderProgram = glCreateProgram();
	glAttachShader(ColliderProgram, vs);
	glAttachShader(ColliderProgram, collider_fs);
	glLinkProgram(ColliderProgram);

	GLuint glass_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glass_fs, &LamborghiniGlassFS);
	glCompileShader(glass_fs);

	GlassProgram = glCreateProgram();
	glAttachShader(GlassProgram, vs);
	glAttachShader(GlassProgram, glass_fs);
	glLinkProgram(GlassProgram);
}

void Lamborghini::Draw()
{
	if (M)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		

		int i = 0;
		for (auto& Pair : M->GetSubMeshes())
		{
			const Mesh::SubMesh& Sub = Pair.second;
			//++i;
			//if(i==2) continue;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), Sub.Vertices.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + sizeof(Vector));
			glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + 2 * sizeof(Vector));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			const Material& Mtl = M->GetMaterial(Sub.MaterialName);
			GLuint Program;
			if (Sub.MaterialName == "Lamborghini_Aventador:BodySG")
			{
				Program = BodyProgram;
				glUseProgram(Program);

				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc,Mtl.Ni);
				loc = glGetUniformLocation(Program, "material.Ns");
				glUniform1f(loc,Mtl.Ns);

				const Texture2D* diffuseTexture = M->GetTexture2D(Mtl.map_Kd);
				const Texture2D* specularTexture = M->GetTexture2D(Mtl.map_Ks);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTexture->GetHandle());
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularTexture->GetHandle());
				loc = glGetUniformLocation(Program, "diffuseSampler2D");
				glUniform1i(loc, 0);
				loc = glGetUniformLocation(Program, "specularSampler2D");
				glUniform1i(loc, 1);
			}
			else if(Sub.MaterialName == "Lamborghini_Aventador:ColliderSG")
			{
				Program = ColliderProgram;
				glUseProgram(Program);
				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc, Mtl.Ni);
			}
			else if (Sub.MaterialName == "Lamborghini_Aventador:GlassSG")
			{
				Program = GlassProgram;
				glUseProgram(Program);
				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Ks");
				glUniform4fv(loc, 1, &Mtl.Ks.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc, Mtl.Ni);
				loc = glGetUniformLocation(Program, "material.Ns");
				glUniform1f(loc, Mtl.Ns);
			}

			{
				GLfloat proj[16] = { 0 };
				glutMatrixOrthoLH(proj, -250, 249, -200, 299, -300, 1500);
				GLint loc = glGetUniformLocation(Program, "proj");
				glUnitformMatrix4fv(loc, 1, false, proj);
				GLfloat model[16] = { 0 };
				static float rad = 0.0f;
				rad += 0.01f;
				glutMatrixRotationY(model, rad);
				loc = glGetUniformLocation(Program, "model");
				glUnitformMatrix4fv(loc, 1, false, model);
			}


			{
				AmbientLight AL;
				AL.Color = LinearColor(0.4f, 0.4f, 0.4f, 1.0f);
				DirctionalLight DL;
				DL.Color = LinearColor(0.6f, 0.6f, 0.6f, 1.0f);
				DL.Direction = { 0.0f, -1.0, 1.0f };

				GLint loc = glGetUniformLocation(Program, "AL.Color");
				glUniform4fv(loc, 1, &AL.Color.r);
				loc = glGetUniformLocation(Program, "DL.Color");
				glUniform4fv(loc, 1, &DL.Color.r);
				loc = glGetUniformLocation(Program, "DL.Direction");
				glUniform4fv(loc, 1, &DL.Direction.x);
			}
			

			glDrawElements(GL_TRIANGLE_LIST, Sub.Indices.size(), GL_SHORT, Sub.Indices.data());
		}
		glFlush();
	}
}
