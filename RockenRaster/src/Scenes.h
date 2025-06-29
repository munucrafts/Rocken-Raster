#pragma once
#include "Primitives.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Fog.h"

struct Windmill : public Scene
{
	Windmill(Scene& activeScene)
	{
		Mesh* island = new Mesh(Static);
		island->LoadObjectFile("../Assets/island.obj", "../Assets/island.png");
		island->transform.scale = glm::vec3(0.15f);
		island->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		island->transform.location = glm::vec3(-2.0f, -3.3f, -14.0f);
		island->mat.hasTex = true;
		activeScene.entities.push_back(island);

		Mesh* windmill = new Mesh(Static);
		windmill->LoadObjectFile("../Assets/Windmill.obj", "../Assets/Windmill.png");
		windmill->transform.scale = glm::vec3(2.5f);
		windmill->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		windmill->transform.location = glm::vec3(0.0f, -1.2f, -14.0f);
		windmill->mat.hasTex = true;
		activeScene.entities.push_back(windmill);

		Mesh* fan = new Mesh(Movable);
		fan->LoadObjectFile("../Assets/Fan.obj", "../Assets/Fan.png");
		fan->transform.scale = glm::vec3(2.5f);
		fan->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		fan->transform.location = glm::vec3(0.0f, 2.85f, -13.97f);
		fan->speedComp.angularSpeed = glm::vec3(0.0f, 0.0f, -0.2f);
		fan->mat.hasTex = true;
		activeScene.entities.push_back(fan);

		ExponentialFog* atmFog = new ExponentialFog();
		activeScene.entities.push_back(atmFog);

		//ParticleSystem* fire = new ParticleSystem();
		//fire->transform.location = glm::vec3(0.0f, 2.85f, -10.97f);
		//fire->speedComp.angularSpeed = glm::vec3(0.0f, 0.0f, 5.0f);
		//fire->mat.tex = false;
		//fire->mat.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//fire->particlesProps.particleCount = 10;
		//fire->particlesProps.rate = 10.0f;
		//fire->particlesProps.transformBegin = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)};
		//fire->particlesProps.transformEnd = {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)};
		//fire->particlesProps.speedCompBegin = {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)};
		//fire->particlesProps.speedCompEnd = {glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)};
		//fire->particlesProps.colorBegin = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//fire->particlesProps.colorEnd = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		//fire->particlesProps.lifetime = 2.0f;
		//activeScene.entities.push_back(fire);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(-20.0f, 20.f, 0.0f);
		dirLight->intensity = 1.0f;
		activeScene.entities.push_back(dirLight);
	}
};