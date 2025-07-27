#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
    mobility = MOVABLE;
    isMoving = true;
}

void ParticleSystem::EmitParticles(const float deltaTime)
{
    // Note for Sarthak - Will write emit fucntionality later.
    if (particles.size() != particlesProps.particleCount)
    {
        particles.resize(particlesProps.particleCount);
        triangles.clear();
    }
}

void Particle::ResetParticle(const ParticleProperties& props)
{
    active = true;
}

