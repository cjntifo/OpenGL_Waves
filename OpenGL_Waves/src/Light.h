#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include "GL_Util.h"

class Light
{
public:
	
	Light() {}

	//Light(LightingHandler::SpotLight)	{}
	//Light(LightingHandler::DirLight)	{}

	Light(glm::vec3 lightPos)
	{
		lightPos = lightPos;
	}

	~Light()
	{
		glDeleteVertexArrays(1, &lightVAO);
		glDeleteBuffers(1, &lightVBO);
	}

	void init(glm::vec3 lightPos)
	{
		lightPos = lightPos;
		
		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = lightVertexShaderSource;
		code.fragmentCode = lightFragmentShaderSource;
		lightShader = Shader(code);

		GeometryGenerator geoGen = GeometryGenerator();
		geoGen.CreateSphere(0.5f, 20, 20, light);

		// create player VAO
		glGenVertexArrays(1, &lightVAO);
		glGenBuffers(1, &lightVBO);
		glGenBuffers(1, &lightEBO);
		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GeometryGenerator::Vertex) * light.Vertices.size(), light.Vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * light.Indices.size(), light.Indices.data(), GL_STATIC_DRAW);

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
		lightShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightShader.setMat4("projection", projection);

		// pass camera/view transformation matrix to shader 
		glm::mat4 view = camera.GetViewMatrix();
		lightShader.setMat4("view", view);

		// pass model matrix to shader 
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, lightPos);
		lightShader.setMat4("model", model);

		// set grid color
		glm::vec4 black = glm::vec4(0, 0, 0, 1);
		lightShader.setVec4("ourColor", black);

		// Set Render Mode
		GLenum GL_RENDER_MODE = GL_TRIANGLES;	// GL_LINES or GL_TRIANGLES

		// bind and draw grid element buffer
		glBindVertexArray(lightVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_RENDER_MODE, light.Indices.size(), GL_UNSIGNED_INT, 0);
	}

	// TODO: Move to static light Handler
	void setLightingUniforms(Shader* shader, std::vector<glm::vec3> pointLightPositions, Camera camera)
	{
		Shader lightingShader = *shader;
		//Camera camera = *cam;

		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point lights
		for (int i = 0; i < pointLightPositions.size(); i++)
		{
			std::string pointLightNum = "pointLights[" + std::to_string(i);
			std::string test = std::string(pointLightNum + "].position");
			
			lightingShader.setVec3(std::string(pointLightNum + "].position"), pointLightPositions[i]);
			lightingShader.setVec3(std::string(pointLightNum + "].ambient"), 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3(std::string(pointLightNum + "].diffuse"), 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3(std::string(pointLightNum + "].specular"), 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat(std::string(pointLightNum + "].constant"), 1.0f);
			lightingShader.setFloat(std::string(pointLightNum + "].linear"), 0.09);
			lightingShader.setFloat(std::string(pointLightNum + "].quadratic"), 0.032);
		}
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	}

	void setTresLightingUniforms(Shader* shader, std::vector<glm::vec3> pointLightPositions, Camera* cam)
	{
		Shader lightingShader = *shader;
		Camera camera = *cam;
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	}

private:
	Shader lightShader;

	GeometryGenerator::MeshData light;
	GLuint lightVAO, lightVBO, lightEBO;

	glm::vec3 lightPos = glm::vec3(0.0f, 0.5f, 0.0f);

	const char *lightVertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 normal; \n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"void main()\n"
		"{\n"
		"	gl_Position = projection * view * model * vec4(position, 1.0f); \n"
		"}\0";

	const char *lightFragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0); // set all 4 vector values to 1.0\n"
		"}\n\0";
};

#endif	// LIGHT_H
