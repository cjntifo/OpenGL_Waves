#pragma once

#ifndef LIGHTINGHANDLER_H
#define LIGHTINGHANDLER_H

#include "GL_Util.h"
#include "Light.h"

class LightingHandler
{
public:
	struct DirLight 
	{
		DirLight() {}
		DirLight(const glm::vec3& dir, const glm::vec3& a, const glm::vec3& d, const glm::vec3& s)
			: direction(dir), ambient(a), diffuse(d), specular(s) {}
		
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct PointLight 
	{
		PointLight() {}
		PointLight(const glm::vec3& pos,
			float c, float l, float q,
			const glm::vec3& a, const glm::vec3& d, const glm::vec3& s)
			: position(pos), constant(c), linear(l), quadratic(q), ambient(a), diffuse(d), specular(s) {}

		PointLight(const glm::vec3& pos)
			: position(pos), constant(1.0f), linear(0.09f), quadratic(0.032f),
			ambient(glm::vec3(0.05f, 0.05f, 0.05f)), diffuse(glm::vec3(0.8f, 0.8f, 0.8f)), specular(glm::vec3(1.0f, 1.0f, 1.0f)) {}


		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct SpotLight 
	{
		SpotLight() {}
		SpotLight(const glm::vec3& pos, const glm::vec3& dir, float cO, float oC,
					float c, float l, float q,
					const glm::vec3& a, const glm::vec3& d, const glm::vec3& s)
			: position(pos), direction(dir), cutOff(cO), outerCutOff(oC), 
				constant(c), linear(l), quadratic(q),
				ambient(a), diffuse(d), specular(s) {}
		
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct SceneLights
	{
		std::vector<PointLight> pointLights;
		std::vector<DirLight> dirLights;
		std::vector<SpotLight> spotLights;
	};

	LightingHandler() {}

	/*LightingHandler(Camera* cam)
	{
		createDefaultLights(cam);
	}*/

	void init()
	{

	}

	// TODO: Move to static light Handler
	void setLightingUniforms(Shader* shader, std::vector<glm::vec3> pointLightPositions, Camera* cam)
	{
		Shader lightingShader = *shader;
		Camera camera = *cam;

		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		// directional light
		for (int i = 0; i < sceneLights.dirLights.size(); i++)
		{
			// TODO: Fix shader to accept multiple DirLights
			//std::string dirLight_ = "dirLights[" + std::to_string(i);	
			//lightingShader.setVec3(std::string(dirLight_ + "].direction"), sceneLights.dirLights[i].direction);

			lightingShader.setVec3("dirLight.direction", sceneLights.dirLights[i].direction);
			lightingShader.setVec3("dirLight.ambient", sceneLights.dirLights[i].ambient);
			lightingShader.setVec3("dirLight.diffuse", sceneLights.dirLights[i].diffuse);
			lightingShader.setVec3("dirLight.specular", sceneLights.dirLights[i].specular);
		}

		// point lights
		for (int i = 0; i < sceneLights.pointLights.size(); i++)
		{
			std::string pointLight_ = "pointLights[" + std::to_string(i);

			lightingShader.setVec3(std::string(pointLight_ + "].position"), sceneLights.pointLights[i].position);
			lightingShader.setVec3(std::string(pointLight_ + "].ambient"), sceneLights.pointLights[i].ambient);
			lightingShader.setVec3(std::string(pointLight_ + "].diffuse"), sceneLights.pointLights[i].diffuse);
			lightingShader.setVec3(std::string(pointLight_ + "].specular"), sceneLights.pointLights[i].specular);
			lightingShader.setFloat(std::string(pointLight_ + "].constant"), sceneLights.pointLights[i].constant);
			lightingShader.setFloat(std::string(pointLight_ + "].linear"), sceneLights.pointLights[i].linear);
			lightingShader.setFloat(std::string(pointLight_ + "].quadratic"), sceneLights.pointLights[i].quadratic);
		}

		// spotLight
		for (int i = 0; i < sceneLights.spotLights.size(); i++)
		{
			// TODO: Fix shader to accept multiple SpotLights
			//std::string spotLight_ = "spotLights[" + std::to_string(i);	
			//lightingShader.setVec3(std::string(spotLight_ + "].position"), sceneLights.spotLights[i].position);

			lightingShader.setVec3("spotLight.position", sceneLights.spotLights[i].position);
			lightingShader.setVec3("spotLight.direction", sceneLights.spotLights[i].direction);
			lightingShader.setVec3("spotLight.ambient", sceneLights.spotLights[i].ambient);
			lightingShader.setVec3("spotLight.diffuse", sceneLights.spotLights[i].diffuse);
			lightingShader.setVec3("spotLight.specular", sceneLights.spotLights[i].specular);
			lightingShader.setFloat("spotLight.constant", sceneLights.spotLights[i].constant);
			lightingShader.setFloat("spotLight.linear", sceneLights.spotLights[i].linear);
			lightingShader.setFloat("spotLight.quadratic", sceneLights.spotLights[i].quadratic);
			lightingShader.setFloat("spotLight.cutOff", sceneLights.spotLights[i].cutOff);
			lightingShader.setFloat("spotLight.outerCutOff", sceneLights.spotLights[i].outerCutOff);
		}
	}

	void updateMainSpotLight(Camera* cam)
	{
		Camera camera = *cam;
		
		sceneLights.spotLights[0].position = camera.Position;
		sceneLights.spotLights[0].direction = camera.Front;
	}

	void createDefaultLights(Camera camera)
	{
		//Camera camera = *cam;
		
		// directional light
		glm::vec3 dir = glm::vec3(-0.2f, -1.0f, -0.3f);

		glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		glm::vec3 diff = glm::vec3(0.4f, 0.4f, 0.4f);
		glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f);

		DirLight dl(dir, ambient, diff, spec);
		addDirLight(dl);

		// point lights
		glm::vec3 pos = glm::vec3(-13.0f, 1.5f, 0.0f);

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diff = glm::vec3(0.8f, 0.8f, 0.8f);
		spec = glm::vec3(1.0f, 1.0f, 1.0f);

		/*for (int i = 0; i < pointLightPositions.size(); i++) {
			pos = glm::vec3(pointLightPositions[i]);
			PointLight pl(pos, constant, linear, quadratic, ambient, diff, spec);
			addPointLight(pl);
		}*/

		PointLight pl(pos, constant, linear, quadratic, ambient, diff, spec);
		addPointLight(pl);

		// spotLight
		pos = glm::vec3(camera.Position);
		dir = glm::vec3(camera.Front);
		float cutOff = glm::cos(glm::radians(12.5f));
		float outerCutOff = glm::cos(glm::radians(15.0f));

		ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		diff = glm::vec3(1.0f, 1.0f, 1.0f);
		spec = glm::vec3(1.0f, 1.0f, 1.0f);

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;

		SpotLight sl(pos, dir, cutOff, outerCutOff, constant, linear, quadratic, ambient, diff, spec);
		addSpotLight(sl);
	}

	SceneLights getSceneLights()
	{
		return sceneLights;
	}

	void addPointLight(glm::vec3 lightPos)
	{
		Light light(PointLight(lightPos));
	}

	void addPointLight(PointLight pl)
	{
		sceneLights.pointLights.push_back(pl);
	}

	void addDirLight(DirLight dl)
	{
		sceneLights.dirLights.push_back(dl);
	}

	void addSpotLight(SpotLight sl)
	{
		sceneLights.spotLights.push_back(sl);
	}

	void removePointLight(PointLight pl)
	{
		// add code here...
	}

	void removeDirLight(DirLight dl)
	{
		// add code here...
	}

	void removeSpotLight(SpotLight sl)
	{
		// add code here...
	}

private:
	SceneLights sceneLights;
};

#endif	// LIGHTINGHANDLER_H
