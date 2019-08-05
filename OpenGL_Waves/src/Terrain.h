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
		//code.fragmentCode = fragmentShaderSource;
		code.fragmentCode = newFragmentShaderSource;
		shaderProgram = Shader(code);

		//Create grid
		GeometryGenerator geoGen;
		geoGen.CreateGrid(100.0f, 100.0f, 100, 100, grid);
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

		setLightingUniforms(&shaderProgram, pointLightPositions, camera);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("projection", projection);

		// pass camera/view transformation matrix to shader 
		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		// pass model matrix to shader 
		glm::mat4 model = glm::mat4();
		shaderProgram.setMat4("model", model);

		shaderProgram.setVec3("objectColor", 0.0f, 0.8f, 0.0f);
		shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shaderProgram.setVec3("lightPos", lightPos);
		shaderProgram.setVec3("viewPos", camera.Position);

		// Set Render Mode
		GLenum GL_RENDER_MODE = GL_LINES; // GL_LINES or GL_TRIANGLES

		// bind and draw grid element buffer
		glBindVertexArray(gridVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_RENDER_MODE, grid.Indices.size(), GL_UNSIGNED_INT, 0);
	}

	void setLightingUniforms(Shader* shader, std::vector<glm::vec3> pointLightPositions, Camera camera)
	{
		Shader lightingShader = *shader;

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

private:
	Shader shaderProgram;

	GeometryGenerator::MeshData grid;
	GLuint gridVAO, gridVBO, gridEBO;

	//lighting
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(20.0f,  1.5f,  20.0f),
		glm::vec3(-10.0f,  1.5f, -10.0f),
		glm::vec3(-15.0f, 1.5f, 0.0f) 
	};

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
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec2 aTexCoord; \n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"out vec3 FragPos; \n"
		"out vec3 Normal; \n"
		"out vec2 TexCoords;\n"
		"void main()\n"
		"{\n"
		"	FragPos = vec3(model * vec4(position, 1.0)); \n"
		"	Normal = mat3(transpose(inverse(model))) * normal; \n"
		"	gl_Position = projection * view * vec4(FragPos, 1.0); \n"
		"	TexCoords = aTexCoord;\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"in vec3 Normal; \n"
		"in vec3 FragPos; \n"
		"out vec4 FragColor;\n"
		"uniform vec3 lightPos;\n"
		"uniform vec3 viewPos;\n"
		"uniform vec3 lightColor;\n"
		"uniform vec3 objectColor;\n"
		"void main()\n"
		"{\n"
		"	// ambient \n"
		"	float ambientStrength = 0.1; \n"
		"	vec3 ambient = ambientStrength * lightColor; \n"
		"	// diffuse \n"
		"	vec3 norm = normalize(Normal); \n"
		"	vec3 lightDir = normalize(lightPos - FragPos); \n"
		"	float diff = max(dot(norm, lightDir), 0.0); \n"
		"	vec3 diffuse = diff * lightColor; \n"
		"	// specular \n"
		"	float specularStrength = 0.5; \n"
		"	vec3 viewDir = normalize(viewPos - FragPos); \n"
		"	vec3 reflectDir = reflect(-lightDir, norm); \n"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); \n"
		"	vec3 specular = specularStrength * spec * lightColor; \n"

		"	vec3 result = (ambient + diffuse + specular) * objectColor; \n"
		"	FragColor = vec4(result, 1.0); \n"
		"}\n\0";

		const char *newFragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"

			"struct Material {\n"
			"	sampler2D diffuse;\n"
			"	sampler2D specular;\n"
			"	float shininess;\n"
			"};\n"

			"struct DirLight {\n"
			"	vec3 direction;\n"

			"	vec3 ambient;\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n"
			"};\n"

			"struct PointLight {\n"
			"	vec3 position;\n"

			"	float constant;\n"
			"	float linear;\n"
			"	float quadratic;\n"

			"	vec3 ambient;\n"
			"	vec3 diffuse;\n"
			"	vec3 specular;\n"
			"};\n"

			"#define NR_POINT_LIGHTS 3\n"

			"in vec3 FragPos;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoords;\n"

			"uniform vec3 objectColor;\n"

			"uniform vec3 viewPos;\n"
			"uniform DirLight dirLight;\n"
			"uniform PointLight pointLights[NR_POINT_LIGHTS];\n"
			"uniform Material material;\n"

			"// function prototypes\n"
			"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
			"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"

			"void main()\n"
			"{\n"
			"	// properties\n"
			"	vec3 norm = normalize(Normal);\n"
			"	vec3 viewDir = normalize(viewPos - FragPos);\n"

			"	// phase 1: directional lighting\n"
			"	vec3 result = CalcDirLight(dirLight, norm, viewDir);\n"

			"	// phase 2: point lights\n"
			"	for (int i = 0; i < NR_POINT_LIGHTS; i++)\n"
			"		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);\n"

			"	FragColor = vec4(result, 1.0);\n"
			"}\n"

			"// calculates the color when using a directional light.\n"
			"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n"
			"{\n"
			"	vec3 lightDir = normalize(-light.direction);\n"
			"	// diffuse shading\n"
			"	float diff = max(dot(normal, lightDir), 0.0);\n"
			"	// specular shading\n"
			"	vec3 reflectDir = reflect(-lightDir, normal);\n"
			"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
			"	// combine results\n"
			"	vec3 ambient = light.ambient * objectColor;\n"
			"	vec3 diffuse = light.diffuse * diff * objectColor;\n"
			"	vec3 specular = light.specular * spec * objectColor;\n"
			"	return (ambient + diffuse + specular);\n"
			"}\n"

			"// calculates the color when using a point light.\n"
			"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
			"{\n"
			"	vec3 lightDir = normalize(light.position - fragPos);\n"
			"	// diffuse shading\n"
			"	float diff = max(dot(normal, lightDir), 0.0); \n"
			"	// specular shading\n"
			"	vec3 reflectDir = reflect(-lightDir, normal); \n"
			"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); \n"
			"	// attenuation\n"
			"	float distance = length(light.position - fragPos); \n"
			"	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); \n"
			"	// combine results\n"
			"	vec3 ambient = light.ambient * objectColor; \n"
			"	vec3 diffuse = light.diffuse * diff * objectColor; \n"
			"	vec3 specular = light.specular * spec * objectColor;\n"
			"	ambient *= attenuation; \n"
			"	diffuse *= attenuation; \n"
			"	specular *= attenuation; \n"
			"	return (ambient + diffuse + specular); \n"
			"}\n\0";
};

#endif	// TERRAIN_H
