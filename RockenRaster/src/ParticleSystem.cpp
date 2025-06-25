#include "ParticleSystem.h"

void ParticleSystem::EmitParticles(float deltaTime)
{
    if (particles.size() != particlesProps.particleCount)
    {
        particles.resize(particlesProps.particleCount);
        triangles.clear();

        for (int i = 0; i < particlesProps.particleCount; i++)
        {
            Triangle tri;
            Vertex v0, v1, v2;

            v0.vert = glm::vec3(0.0f, 1.0f, 0.0f);
            v1.vert = glm::vec3(-1.0f, -1.0f, 0.0f);
            v2.vert = glm::vec3(1.0f, -1.0f, 0.0f);

            v0.uv = glm::vec2(0.5f, 1.0f);
            v1.uv = glm::vec2(0.0f, 0.0f);
            v2.uv = glm::vec2(1.0f, 0.0f);

            glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
            v0.normal = v1.normal = v2.normal = normal;

            tri.vertices = { v0, v1, v2 };
            triangles.push_back(tri);
        }
    }


}

void Particle::ResetParticle(ParticleProperties& particleProps)
{
    transform = particleProps.transformBegin;
    speedComp = particleProps.speedCompBegin;
    color = particleProps.colorBegin;
    lifeRemaining = particleProps.lifetime;
    active = true;
}
