#pragma once
#include <glm/glm.hpp>
#include "Primitives.h"

class Light : public Entity
{
public:
	Light();
	virtual ~Light() = default;

public:
	float intensity;
	glm::vec3 direction;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight() = default;
};
