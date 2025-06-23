#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	virtual ~Light() = default;

public:
	float intensity;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();

public:
	glm::vec3 direction;
};
