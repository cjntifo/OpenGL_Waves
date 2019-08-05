#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include "GL_Util.h"

class Player
{
public:
	Player()
	{
		
	}

	~Player()
	{
		glDeleteVertexArrays(1, &playerVAO);
		glDeleteBuffers(1, &playerVBO);
	}

	void init()
	{
		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = playerVertexShaderSource;
		code.fragmentCode = fragmentShaderSource;
		playerShader = Shader(code);

		GeometryGenerator geoGen = GeometryGenerator();
		geoGen.CreateSphere(0.5f, 20, 20, player);

		// create player VAO
		glGenVertexArrays(1, &playerVAO);
		glGenBuffers(1, &playerVBO);
		glGenBuffers(1, &playerEBO);
		glBindVertexArray(playerVAO);

		glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * player.Vertices.size(), player.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * player.Indices.size(), player.Indices.data(), GL_STATIC_DRAW);

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
		//processInput(window, deltaTime);

		// ativate shader program
		playerShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		playerShader.setMat4("projection", projection);

		// pass camera/view transformation matrix to shader 
		glm::mat4 view = camera.GetViewMatrix();
		playerShader.setMat4("view", view);

		// pass model matrix to shader 
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, playerPosition);
		playerShader.setMat4("model", model);

		// set grid color
		glm::vec4 black = glm::vec4(0, 0, 0, 1);
		playerShader.setVec4("ourColor", black);

		// Set Render Mode
		GLenum GL_RENDER_MODE = GL_LINES;	// GL_LINES or GL_TRIANGLES

		// bind and draw grid element buffer
		glBindVertexArray(playerVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_RENDER_MODE, player.Indices.size(), GL_UNSIGNED_INT, 0);
	}

	void processInput(GLFWwindow *window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			doMovement(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			doMovement(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			doMovement(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			doMovement(RIGHT, deltaTime);
	}

	enum Player_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	void doMovement(Player_Movement direction, float deltaTime)
	{
		player_dx = MAX_SPEED * deltaTime;
		player_dz = MAX_SPEED * deltaTime;

		if (direction == FORWARD)
			playerPosition.z -= player_dz;
		if (direction == BACKWARD)
			playerPosition.z += player_dz;
		if (direction == LEFT)
			playerPosition.x -= player_dx;
		if (direction == RIGHT)
			playerPosition.x += player_dx;
	}

private:
	Shader playerShader;

	GeometryGenerator::MeshData player;
	GLuint playerVAO, playerVBO, playerEBO;

	const float MAX_SPEED = 2.5f;
	glm::vec3 playerPosition = glm::vec3(0.0f, 0.5f, 0.0f);
	float player_dx = 0;
	float player_dz = 0;

	const char *playerVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec2 aTexCoord; \n"
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

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = ourColor;\n"
		"}\n\0";
};

#endif	// PLAYER_H
