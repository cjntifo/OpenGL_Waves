#pragma once

#ifndef WORLD_H
#define WORLD_H

#include "GL_Util.h"
#include "Terrain.h"

class World
{
public:
	World()
	{		
		earth.init();
		
		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = vertexShaderSource;
		code.fragmentCode = fragmentShaderSource;
		shaderProgram = Shader(code);

		//Create grid
		GeometryGenerator geoGen;
		//Create cylinder
		geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

		// Setup water VAO
		glGenVertexArrays(1, &cylinderVAO);
		glGenBuffers(1, &cylinderVBO);
		glGenBuffers(1, &cylinderEBO);
		glBindVertexArray(cylinderVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * cylinder.Vertices.size(), cylinder.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cylinder.Indices.size(), cylinder.Indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);

		//Create sphere
		geoGen.CreateSphere(0.5f, 20, 20, sphere);

		// Setup water VAO
		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glGenBuffers(1, &sphereEBO);
		glBindVertexArray(sphereVAO);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * sphere.Vertices.size(), sphere.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * sphere.Indices.size(), sphere.Indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);

		/*
		
		UINT totalVertexCount = 
		box.Vertices.size() + 
		grid.Vertices.size() + 
		sphere.Vertices.size() +
		cylinder.Vertices.size();

		UINT totalIndexCount = 
			mBoxIndexCount + 
			mGridIndexCount + 
			mSphereIndexCount +
			mCylinderIndexCount;

		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//

		std::vector<Vertex> vertices(totalVertexCount);

		XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

		UINT k = 0;
		for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos   = box.Vertices[i].Position;
			vertices[k].Color = black;
		}
		
		std::vector<UINT> indices;
		indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
		indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
		indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
		indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

		*/
	}

	~World()
	{
		glDeleteVertexArrays(1, &sphereVAO);
		glDeleteBuffers(1, &sphereVBO);
	}

	void update(GLFWwindow* window, Camera camera, float deltaTime, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
	{
		earth.update(window, camera, deltaTime, SCR_WIDTH, SCR_HEIGHT);

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

		// set color for cylinders
		glm::vec4 blue = glm::vec4(0.5, 0.5, 1, 1);
		shaderProgram.setVec4("ourColor", blue);

		// bind and draw cylinder element buffer
		glBindVertexArray(cylinderVAO);
		
		// offset each pillar by positions
		for (unsigned int i = 0; i < 5; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			model = glm::mat4(1.0f);
			model = glm::translate(model, pillarPositions[i]);
			shaderProgram.setMat4("model", model);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_RENDER_MODE, cylinder.Indices.size(), GL_UNSIGNED_INT, 0);
		}

		glm::vec4 red = glm::vec4(1, 0.5, 0.5, 1);
		shaderProgram.setVec4("ourColor", red);

		// bind and draw sphere element buffer
		glBindVertexArray(sphereVAO);

		for (unsigned int i = 0; i < 5; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(pillarPositions[i].x, 3.5f, pillarPositions[i].z));
			shaderProgram.setMat4("model", model);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_RENDER_MODE, cylinder.Indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

private:
	Terrain earth;

	Shader shaderProgram;

	GeometryGenerator::MeshData cylinder;
	GLuint cylinderVAO, cylinderVBO, cylinderEBO;

	GeometryGenerator::MeshData sphere;
	GLuint sphereVAO, sphereVBO, sphereEBO;

	// world space positions of our cubes
	glm::vec3 pillarPositions[5] = {
		glm::vec3(20.0f,  1.5f,  20.0f),
		glm::vec3(-10.0f,  1.5f, -10.0f),
		glm::vec3(-15.0f, 1.5f, 0.0f),
		glm::vec3(10.0f, 1.5f, -10.0f),
		glm::vec3(-10.0f, 1.5f, -15.0f)
	};

	void createObject(GLuint *VAO_p, GLuint *VBO_p, GLuint *EBO_p, GeometryGenerator::MeshData mesh)
	{
		GLuint VAO = *VAO_p;
		GLuint VBO = *VBO_p;
		GLuint EBO = *EBO_p;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * mesh.Vertices.size(), mesh.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.Indices.size(), mesh.Indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	const char *playerVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"uniform float deltaTime; \n"
		"uniform float velX; \n"
		"uniform float velZ; \n"
		"void main()\n"
		"{\n"
		"	//float dx = (velX * deltaTime); \n"
		"	//float dz = (velZ * deltaTime); \n"
		"	//gl_Position = projection * view * model * vec4((position.x + dx), position.y, (position.z + dz), 1.0f); \n"
		"	gl_Position = projection * view * model * vec4(position, 1.0f); \n"
		"}\0";
	
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

#endif	// WORLD_H
