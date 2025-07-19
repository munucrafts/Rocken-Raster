#pragma once
#include "Primitives.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Fog.h"

struct Windmill : public Scene
{
	Windmill() { sceneName = "Windmill"; }
	void LoadIntoScene(Scene& activeScene) override
	{
		Mesh* island = new Mesh(STATIC);
		island->LoadObjectFile("Assets/Island.obj", "Assets/Island.png");
		island->transform.scale = glm::vec3(0.15f);
		island->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		island->transform.location = glm::vec3(-2.0f, -3.3f, -14.0f);
		activeScene.entities.push_back(island);

		Mesh* windmill = new Mesh(STATIC);
		windmill->LoadObjectFile("Assets/Windmill.obj", "Assets/Windmill.png");
		windmill->transform.scale = glm::vec3(2.5f);
		windmill->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		windmill->transform.location = glm::vec3(0.0f, -1.2f, -14.0f);
		activeScene.entities.push_back(windmill);

		Mesh* fan = new Mesh(MOVABLE);
		fan->LoadObjectFile("Assets/Fan.obj", "Assets/Fan.png");
		fan->transform.scale = glm::vec3(2.5f);
		fan->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		fan->transform.location = glm::vec3(0.0f, 2.85f, -13.97f);
		fan->speedComp.angularSpeed = glm::vec3(0.0f, 0.0f, -0.2f);;
		activeScene.entities.push_back(fan);

		ExponentialFog* atmFog = new ExponentialFog();
		activeScene.entities.push_back(atmFog);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(-20.0f, 20.f, 0.0f);
		dirLight->intensity = 2.0f;
		activeScene.entities.push_back(dirLight);

		activeScene.sceneSkyColor = { glm::vec4(0.22f, 0.71f, 1.0f, 1.0f), glm::vec4(0.83f, 0.84f, 0.85f, 1.0f) };
		activeScene.sceneName = sceneName;
	}
};

struct Space : public Scene
{
	Space() { sceneName = "Space"; }
	void LoadIntoScene(Scene& activeScene) override
	{
		Mesh* Earth = new Mesh(MOVABLE);
		Earth->LoadObjectFile("Assets/Earth.obj", "Assets/Earth.png");
		Earth->transform.scale = glm::vec3(0.25f);
		Earth->transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		Earth->transform.location = glm::vec3(0.0f, 0.0f, -28.0f);
		Earth->speedComp.angularSpeed = glm::vec3(0.2f, 0.1f, 0.14f);
		activeScene.entities.push_back(Earth);

		Mesh* Satellite = new Mesh(MOVABLE);
		Satellite->LoadObjectFile("Assets/Satellite.obj", "Assets/Satellite.png");
		Satellite->transform.scale = glm::vec3(1.0f);
		Satellite->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		Satellite->transform.location = glm::vec3(0.0f, 2.2f, -28.0f);
		Satellite->speedComp.angularSpeed = glm::vec3(0.2f, 0.1f, 0.14f);
		activeScene.entities.push_back(Satellite);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(30.0f, 20.f, 0.0f);
		dirLight->intensity = 2.0f;
		activeScene.entities.push_back(dirLight);

		activeScene.sceneSkyColor = { glm::vec4(0.0f), glm::vec4(0.0f) };
		activeScene.sceneName = sceneName;
	}
};

struct RetroKeyboard : public Scene
{
	RetroKeyboard() { sceneName = "Retro Keyboard"; }
	void LoadIntoScene(Scene& activeScene) override
	{
		Mesh* RetroKeyboard = new Mesh(MOVABLE);
		RetroKeyboard->LoadObjectFile("Assets/RetroKeyboard.obj", "Assets/RetroKeyboard.png");
		RetroKeyboard->transform.scale = glm::vec3(0.1f);
		RetroKeyboard->transform.rotation = glm::vec3(60.0f, 180.0f, 0.0f);
		RetroKeyboard->transform.location = glm::vec3(0.0f, 0.0f, -28.0f);
		RetroKeyboard->speedComp.angularSpeed = glm::vec3(0.0f, 0.1f, 0.0f);
		activeScene.entities.push_back(RetroKeyboard);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(0.0f, -20.f, -10.0f);
		dirLight->intensity = 0.5f;
		activeScene.entities.push_back(dirLight);

		activeScene.sceneSkyColor = { glm::vec4(0.17f, 0.74f, 0.58, 1.0f), glm::vec4(0.09f, 0.32f, 0.41f, 1.0f) };
		activeScene.sceneName = sceneName;
	}
};

struct Chestnut : public Scene
{
	Chestnut() { sceneName = "Chestnut"; }
	void LoadIntoScene(Scene& activeScene) override
	{
		Mesh* Chestnut = new Mesh(MOVABLE);
		Chestnut->LoadObjectFile("Assets/Chestnut.obj", "Assets/Chestnut.png");
		Chestnut->transform.scale = glm::vec3(1.5f);
		Chestnut->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		Chestnut->transform.location = glm::vec3(0.0f, 0.0f, -2.0f);
		Chestnut->speedComp.angularSpeed = glm::vec3(0.0f, 0.1f, 0.0f);
		activeScene.entities.push_back(Chestnut);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(30.0f, 20.f, 0.0f);
		dirLight->intensity = 2.0f;
		activeScene.entities.push_back(dirLight);

		activeScene.sceneSkyColor = { glm::vec4(0.9f, 0.85f, 0.95, 1.0f), glm::vec4(0.7f, 0.84f, 0.41f, 1.0f) };
		activeScene.sceneName = sceneName;
	}
};

struct StylizedGuitar : public Scene
{
	StylizedGuitar() { sceneName = "Stylized Guitar"; }
	void LoadIntoScene(Scene& activeScene) override
	{
		Mesh* StylizedGuitar = new Mesh(MOVABLE);
		StylizedGuitar->LoadObjectFile("Assets/StylizedGuitar.obj", "Assets/StylizedGuitar.png");
		StylizedGuitar->transform.scale = glm::vec3(10.0f);
		StylizedGuitar->transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		StylizedGuitar->transform.location = glm::vec3(0.0f, 0.0f, -2.0f);
		StylizedGuitar->speedComp.angularSpeed = glm::vec3(0.0f, 0.1f, 0.0f);
		activeScene.entities.push_back(StylizedGuitar);

		DirectionalLight* dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(30.0f, 20.f, 0.0f);
		dirLight->intensity = 2.0f;
		activeScene.entities.push_back(dirLight);

		activeScene.sceneSkyColor = { glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.15f, 0.0f, 1.0f, 1.0f) };
		activeScene.sceneName = sceneName;
	}
};