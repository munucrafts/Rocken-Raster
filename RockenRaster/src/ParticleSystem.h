#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Walnut/Random.h>
#include "Primitives.h"

struct ParticleProperties
{
	int particleCount;
	float rate;
	Transform transformBegin, transformEnd;
	SpeedComponent speedCompBegin, speedCompEnd;
	glm::vec4 colorBegin, colorEnd;
	float lifetime;
};

struct Particle
{
	float age = 10.0f;
	Transform transform;
	SpeedComponent speedComp;
	glm::vec4 color;
	float lifeRemaining;
	bool active;

	void ResetParticle(ParticleProperties& particleProps);
};

class ParticleSystem : public Mesh
{
public:
	ParticleSystem() = default;
	~ParticleSystem() = default;
	void EmitParticles(float deltaTime);

public:
	ParticleProperties particlesProps;

private:
	std::vector<Particle> particles;
};
