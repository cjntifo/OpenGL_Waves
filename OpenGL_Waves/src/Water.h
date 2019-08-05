#pragma once

#ifndef WATER_H
#define WATER_H

#include "GL_Util.h"

class Water
{
public:
	Water()
	{
		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = waveVertexShaderSource;
		code.fragmentCode = waveFragmentShaderSource;
		shaderProgram = Shader(code);

		//Create grid
		//GeometryGenerator::MeshData grid;
		GeometryGenerator geoGen;
		geoGen.CreateGrid(50.0f, 50.0f, 300, 200, grid);

		// Setup water VAO
		//GLuint waterVAO, waterVBO, waterEBO;
		glGenVertexArrays(1, &waterVAO);
		glGenBuffers(1, &waterVBO);
		glGenBuffers(1, &waterEBO);
		glBindVertexArray(waterVAO);

		glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex)*grid.Vertices.size(), grid.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * grid.Indices.size(), grid.Indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	~Water()
	{
		glDeleteVertexArrays(1, &waterVAO);
		glDeleteBuffers(1, &waterVBO);
	}

	void update(Camera camera, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
	{
		// ativate shader program
		shaderProgram.use();

		// pass current time to shader for wave calculations
		GLfloat time = (float)glfwGetTime();
		shaderProgram.setFloat("time", time);

		// specify a wavelength of 4
		GLfloat wavelength = 4.0f;
		shaderProgram.setFloat("wavelength", wavelength);

		// set maxmimum wave height at 1
		GLfloat peak = 1.0f;
		shaderProgram.setFloat("peak", peak);

		// set wave direction to be a random float
		GLfloat waveDir = static_cast <float> (rand()) / (static_cast <float> (255.0f / 0.5f));
		shaderProgram.setFloat("waveDir", waveDir);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("projection", projection);

		// pass camera/view transformation matrix to shader 
		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		// pass model matrix to shader 
		glm::mat4 model = glm::mat4();
		shaderProgram.setMat4("model", model);

		// Set Render Mode
		GLenum GL_RENDER_MODE = GL_LINES; // GL_LINES or GL_TRIANGLES
										  
		// bind and draw water element buffer
		glBindVertexArray(waterVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_RENDER_MODE, 3 * grid.Indices.size(), GL_UNSIGNED_INT, 0);
	}

private:
	Shader shaderProgram;

	GeometryGenerator::MeshData grid;
	GLuint waterVAO, waterVBO, waterEBO;
	
	const char *waveVertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 position; \n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec2 aTexCoord; \n"
		"out vec2 TexCoord; \n"
		"const float pi = 3.14159265; \n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"uniform float waveDir; \n"
		"uniform float wavelength; \n"
		"uniform float peak; \n"
		"uniform float time; \n"
		"void main()\n"
		"{\n"
		"	float a = dot(position.x, normalize(waveDir)); \n"
		"	float b = peak - 1.0f; \n"
		"	float k = (2.0f * pi) / wavelength; \n"
		"	float c = sqrt(9.81f / k); \n"
		"	float wave_dx = (exp(k * b) / k) * sin(k * (a + c * time)); \n"
		"	float wave_dy = -(exp(k * b) / k) * cos(k * (a + c * time)); \n"
		"	gl_Position = projection * view * model * vec4((position.x + wave_dx), (position.y + wave_dy), (position.z), 1.0f); \n"
		"	TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y); \n"
		"}\0";

	const char *waveFragmentShaderSource = "#version 330 core \n"
		"out vec4 FragColor; \n"
		"uniform sampler2D texture1; \n"
		"uniform sampler2D texture2; \n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(0.0, 0.0, 0.8, 0.2); \n"
		"}\0";
};

#endif	// WATER_H
