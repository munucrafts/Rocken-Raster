#pragma once
#include "Primitives.h"

struct Stick
{
	glm::vec3 pointA;
	glm::vec3 pointB;
	float length;
	float restLength;

	Stick()
	{
		restLength = glm::length(pointA - pointB);
	}
};

class ClothMesh : public StaticMesh
{
private:
	std::vector<Stick> sticks;
	glm::vec3 force;
	float drag;;
	float elasticity;

public:
	ClothMesh();
	void SimulateCloth(const float deltaTime);
	void InitCloth();
};

