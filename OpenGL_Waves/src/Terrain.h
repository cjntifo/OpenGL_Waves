#pragma once

#ifndef TERRAIN_H
#define TERRAIN_H

#include "GL_Util.h"
#include <time.h>

class Terrain
{
public:
	Terrain()
	{
		
	}

	~Terrain()
	{
		glDeleteVertexArrays(1, &gridVAO);
		glDeleteBuffers(1, &gridVBO);
	}

	void init()
	{
		srand(time(NULL));
		seed = rand();

		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = vertexShaderSource;
		code.fragmentCode = fragmentShaderSource;
		shaderProgram = Shader(code);

		//Create grid
		GeometryGenerator geoGen;
		geoGen.CreateGrid(50.0f, 50.0f, 100, 100, grid);
		generateTerrain();

		// Setup water VAO
		glGenVertexArrays(1, &gridVAO);
		glGenBuffers(1, &gridVBO);
		glGenBuffers(1, &gridEBO);
		glBindVertexArray(gridVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * grid.Vertices.size(), grid.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * grid.Indices.size(), grid.Indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	void update(GLFWwindow* window, Camera camera, float deltaTime, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
	{
		// ativate shader program
		shaderProgram.use();

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

										  // set grid color
		glm::vec4 white = glm::vec4(1, 1, 1, 1);
		shaderProgram.setVec4("ourColor", white);

		// bind and draw grid element buffer
		glBindVertexArray(gridVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_RENDER_MODE, grid.Indices.size(), GL_UNSIGNED_INT, 0);
	}

private:
	Shader shaderProgram;

	GeometryGenerator::MeshData grid;
	GLuint gridVAO, gridVBO, gridEBO;

	void generateTerrain()
	{
		for (int i = 0; i < grid.Vertices.size(); i++)
		{
			grid.Vertices[i].Position.y = -2.0f + generateHeight(grid.Vertices[i].Position.x, grid.Vertices[i].Position.z);
		}
	}

	int seed;
	const int AMPLITUDE = 1;

	float generateHeight(float x, float z)
	{
		//float total = getInterpolatedNoise(x, z) * AMPLITUDE;
		
		float total = getInterpolatedNoise(x/4.0f, z/4.0f) * AMPLITUDE;
		total += getInterpolatedNoise(x/2.0f, z/2.0f) * AMPLITUDE/3.0f;
		//total += getInterpolatedNoise(x, z) * AMPLITUDE / 9.0f;
		
		return total;
	}

	float interpolate(float a, float b, float blend)
	{
		double theta = blend * glm::pi<float>();
		float f = (1 - cos(theta)) * 0.5;
		return a * (1.0f - f) + b * f;
	}

	float getInterpolatedNoise(float x, float z)
	{
		int intX = (int)x;
		int intZ = (int)z;
		float fracX = x - intX;
		float fracZ = z - intZ;

		float v1 = generateSmoothNoise(intX, intZ);
		float v2 = generateSmoothNoise(intX + 1, intZ);
		float v3 = generateSmoothNoise(intX, intZ + 1);
		float v4 = generateSmoothNoise(intX + 1, intZ + 1);

		float i1 = interpolate(v1, v2, fracX);
		float i2 = interpolate(v3, v4, fracX);

		return interpolate(i1, i2, fracZ);
	}

	float generateSmoothNoise(float x, float z)
	{
		float corners = (generateNoise(x-1, z-1) + generateNoise(x-1, z+1) + generateNoise(x+1, z-1) + generateNoise(x+1, z+1));
		float sides = (generateNoise(x - 1, z) + generateNoise(x, z - 1) + generateNoise(x + 1, z) + generateNoise(x, z + 1));
		float center = generateNoise(x, z);

		return (corners / 16.0f) + (sides / 8.0f) + (center / 4.0f);
	}

	float generateNoise(float x, float z)
	{
		srand(x * 4698 + z * 3253 + seed);
		return rand() % 3;
	}

	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main()\n"
		"{\n"
		"	gl_Position = projection * view * model * vec4(position, 1.0f); \n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = ourColor;\n"
		"}\n\0";
};

#endif	// TERRAIN_H
