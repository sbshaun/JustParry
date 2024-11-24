#include "smoke_system.hpp"

SmokeParticleSystem::SmokeParticleSystem()
{
    shader = new Shader("particle");
    std::cout << "Shader program ID: " << shader->m_shaderProgram << std::endl;
    init();
}

void SmokeParticleSystem::update(float deltaTime)
{
    if (particles.empty())
    {
        return;
    }

    for (auto &particle : particles)
    {
        // Update position
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.z += particle.vz * deltaTime;

        // Fade out
        particle.r = std::min(0.52f, particle.r + (deltaTime * 0.05f));
        particle.g = std::min(0.53f, particle.g + (deltaTime * 0.05f));
        particle.b = std::min(0.52f, particle.b + (deltaTime * 0.05f));

        // Update lifetime
        particle.life -= deltaTime;

        // Apply upward force to simulate rising smoke
        particle.vy += 2.0f * deltaTime;
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle &p)
                       { return p.life <= 0.0f; }),
        particles.end());
}

void SmokeParticleSystem::emit(float x, float y, float z, bool direction)
{
    if (particles.size() >= MAX_PARTICLES)
    {
        return;
    }

    const int NUM_PARTICLES = 10;
    const float MIN_VELOCITY = -0.1f;
    const float MAX_VELOCITY = 0.1f;
    const float SPAWN_RADIUS = 0.05f;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        Particle particle;

        // Random spawn position within a small radius
        double theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        double radius = ((float)rand() / RAND_MAX) * SPAWN_RADIUS;
        particle.x = float(x + radius * cos(theta));
        particle.y = float(y + radius * sin(theta));
        particle.z = z;

        // Random velocity
        particle.vx = ((float)rand() / RAND_MAX) * (MAX_VELOCITY - MIN_VELOCITY) + MIN_VELOCITY;
        particle.vy = ((float)rand() / RAND_MAX) * (MAX_VELOCITY - MIN_VELOCITY) + MIN_VELOCITY;
        particle.vz = 0.0f;

        particle.r = 0.13f;
        particle.g = 0.13f;
        particle.b = 0.13f;
        particle.a = 1.0f;

        particle.life = 0.5f + ((float)rand() / RAND_MAX) * 0.2f;

        particles.push_back(particle);

        if (particles.size() >= MAX_PARTICLES)
        {
            break;
        }
    }
}