#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	Light();

public:
	glm::vec3 location;
	glm::vec3 direction;
	float intensityMultiplier;
};

