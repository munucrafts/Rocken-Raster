#include "ParticleSystem.h"

void ParticleSystem::ResetParticle(Particle& particle)
{
    particle.active = true;
    particle.location = particlesProps.location;

    glm::vec2 randomVel = particlesProps.velocityVariation * (glm::vec2)Walnut::Random::Vec3();
    particle.velocity = particlesProps.velocity + randomVel;

    particle.colorBegin = particlesProps.colorBegin;
    particle.colorEnd = particlesProps.colorEnd;

    float randomSize = particlesProps.sizeVariation * Walnut::Random::Float();
    particle.sizeBegin = particlesProps.sizeBegin + randomSize;
    particle.sizeEnd = particlesProps.sizeEnd;

    particle.lifetime = particlesProps.lifetime;
    particle.lifeRemaining = particlesProps.lifetime;

    particle.rotation = Walnut::Random::Float() * 2.0f * 3.1415926f;
}

void ParticleSystem::DrawParticle(glm::vec2 location, float size, glm::vec4 color)
{

}

void ParticleSystem::InitializeParticleSystem()
{
    particles.resize(particlesProps.particleCount);

    for (Particle& particle : particles)
    {
        particle.active = false;
    }
}

void ParticleSystem::Emit(float deltaTime, glm::vec2& screenResolution)
{
    for (Particle& particle : particles)
    {
        if (!particle.active)
        {
            ResetParticle(particle);
            continue;
        }

        if (particle.lifeRemaining <= 0.0f)
        {
            particle.active = false;
            continue;
        }

        deltaTime *= 0.1f;

        particle.lifeRemaining -= deltaTime;
        particle.location += particle.velocity * deltaTime;

        float lifeRatio = particle.lifeRemaining / particle.lifetime;
        glm::vec4 color = glm::mix(particle.colorEnd, particle.colorBegin, lifeRatio);
        float size = glm::mix(particle.sizeEnd, particle.sizeBegin, lifeRatio);

        DrawParticle(particle.location, size, color);
    }
}

