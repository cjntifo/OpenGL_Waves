#pragma once

#ifndef WORLD_H
#define WORLD_H

#include "GL_Util.h"
#include "Terrain.h"
#include "Light.h"

class World
{
public:
	World()
	{		
		earth.init();
		light.init(lightPos);
		
		//Setup shader program
		Shader::ShaderCode code;
		code.vertexCode = vertexShaderSource;
		//code.fragmentCode = fragmentShaderSource;
		code.fragmentCode = newFragmentShaderSource;
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
		light.update(window, camera, deltaTime, SCR_WIDTH, SCR_HEIGHT);

		// ativate shader program
		shaderProgram.use();

		light.setLightingUniforms(&shaderProgram, pointLightPositions, camera);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("projection", projection);

		// pass camera/view transformation matrix to shader 
		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		// pass model matrix to shader 
		glm::mat4 model = glm::mat4();
		shaderProgram.setMat4("model", model);

		//shaderProgram.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shaderProgram.setVec3("lightPos", lightPos);
		shaderProgram.setVec3("viewPos", camera.Position);

		// Set Render Mode
		GLenum GL_RENDER_MODE = GL_LINES; // GL_LINES or GL_TRIANGLES		

		// set color for cylinders
		glm::vec3 blue = glm::vec3(0.5, 0.5, 1);
		shaderProgram.setVec3("objectColor", blue);

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

		glm::vec3 red = glm::vec3(1, 0.5, 0.5);
		shaderProgram.setVec3("objectColor", red);

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
	Light light;

	Shader shaderProgram;

	GeometryGenerator::MeshData cylinder;
	GLuint cylinderVAO, cylinderVBO, cylinderEBO;

	GeometryGenerator::MeshData sphere;
	GLuint sphereVAO, sphereVBO, sphereEBO;

	// lighting
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(22.0f,  1.5f,  20.0f),
		glm::vec3(-13.0f,  1.5f, -10.0f),
		glm::vec3(-13.0f, 1.5f, 0.0f) };
	//std::vector<glm::vec3> pointLightPositions = { glm::vec3(1.2f, 1.0f, 2.0f) };

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

	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 normal; \n"
		"layout(location = 2) in vec2 aTexCoord; \n"
		"uniform mat4 model; \n"
		"uniform mat4 view; \n"
		"uniform mat4 projection; \n"
		"out vec3 FragPos; \n"
		"out vec3 Normal; \n"
		"out vec2 TexCoord; \n"
		"void main()\n"
		"{\n"
		"	FragPos = vec3(model * vec4(position, 1.0)); \n"
		"	Normal = mat3(transpose(inverse(model))) * normal; \n"
		"	gl_Position = projection * view * vec4(FragPos, 1.0); \n"
		"	TexCoord = aTexCoord; \n"
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

	const char *texFragmentShaderSource = "#version 330 core\n"
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

		"struct SpotLight {\n"
		"	vec3 position;\n"
		"	vec3 direction;\n"
		"	float cutOff;\n"
		"	float outerCutOff;\n"

		"	float constant;\n"
		"	float linear;\n"
		"	float quadratic;\n"

		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"};\n"

		"#define NR_POINT_LIGHTS 4\n"

		"in vec3 FragPos;\n"
		"in vec3 Normal;\n"
		"in vec2 TexCoords;\n"

		"uniform vec3 viewPos;\n"
		"uniform DirLight dirLight;\n"
		"uniform PointLight pointLights[NR_POINT_LIGHTS];\n"
		"uniform SpotLight spotLight;\n"
		"uniform Material material;\n"

		"// function prototypes\n"
		"vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
		"vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
		"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"

		"void main()\n"
		"{\n"
		"	// properties\n"
		"	vec3 norm = normalize(Normal);\n"
		"	vec3 viewDir = normalize(viewPos - FragPos);\n"

		"	// == =====================================================\n"
		"	// Our lighting is set up in 3 phases: directional, point lights and an optional flashlight\n"
		"	// For each phase, a calculate function is defined that calculates the corresponding color\n"
		"	// per lamp. In the main() function we take all the calculated colors and sum them up for\n"
		"	// this fragment's final color.\n"
		"	// == =====================================================\n"
		"	// phase 1: directional lighting\n"
		"	vec3 result = CalcDirLight(dirLight, norm, viewDir);\n"
		"	// phase 2: point lights\n"
		"	for (int i = 0; i < NR_POINT_LIGHTS; i++)\n"
		"		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);\n"
		"	// phase 3: spot light\n"
		"	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);\n"

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
		"	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
		"	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
		"	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
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
		"	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); \n"
		"	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); \n"
		"	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
		"	ambient *= attenuation; \n"
		"	diffuse *= attenuation; \n"
		"	specular *= attenuation; \n"
		"	return (ambient + diffuse + specular); \n"
		"}\n"

		"// calculates the color when using a spot light.\n"
		"vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)\n"
		"{\n"
		"	vec3 lightDir = normalize(light.position - fragPos);\n"
		"	// diffuse shading\n"
		"	float diff = max(dot(normal, lightDir), 0.0);\n"
		"	// specular shading\n"
		"	vec3 reflectDir = reflect(-lightDir, normal);\n"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
		"	// attenuation\n"
		"	float distance = length(light.position - fragPos);\n"
		"	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"
		"	// spotlight intensity\n"
		"	float theta = dot(lightDir, normalize(-light.direction));\n"
		"	float epsilon = light.cutOff - light.outerCutOff;\n"
		"	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n"
		"	// combine results\n"
		"	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));\n"
		"	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));\n"
		"	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));\n"
		"	ambient *= attenuation * intensity;\n"
		"	diffuse *= attenuation * intensity;\n"
		"	specular *= attenuation * intensity;\n"
		"	return (ambient + diffuse + specular);\n"
		"}\n\0";
};

#endif	// WORLD_H
