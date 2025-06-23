#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Walnut/Random.h>

struct ParticleProperties
{
	int particleCount;
	float rate;
	glm::vec2 location;
	glm::vec2 velocity, velocityVariation;
	glm::vec4 colorBegin, colorEnd;
	float sizeBegin, sizeEnd, sizeVariation;
	float lifetime;
};

struct Particle
{
	glm::vec2 location;
	float rotation;
	glm::vec2 velocity;
	glm::vec4 colorBegin, colorEnd;
	float sizeBegin, sizeEnd;
	float lifetime;
	float lifeRemaining;
	bool active;
};

class ParticleSystem
{
public:
	ParticleSystem() = default;
	void InitializeParticleSystem();
	void Emit(float deltaTime, glm::vec2& screenResolution);
	void ResetParticle(Particle& particle);
	void DrawParticle(glm::vec2 location, float size, glm::vec4 color);

public:
	ParticleProperties particlesProps;

private:
	std::vector<Particle> particles;
};


