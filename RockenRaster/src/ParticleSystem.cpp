#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
    mobility = MOVABLE;
    isMoving = true;
}

void ParticleSystem::EmitParticles(float deltaTime)
{
    if (particles.size() != particlesProps.particleCount)
    {
        particles.resize(particlesProps.particleCount);
        triangles.clear();
    }
}

void Particle::ResetParticle(ParticleProperties& props)
{
    active = true;
}

