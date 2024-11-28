#include "blood_system.hpp"

BloodParticleSystem::BloodParticleSystem()
{
    shader = new Shader("particle");
    std::cout << "Shader program ID: " << shader->m_shaderProgram << std::endl;
    init();
}

void BloodParticleSystem::update(float deltaTime)
{
    if (particles.empty())
    {
        return;
    }

    const float GRAVITY = -9.8f;

    for (auto &particle : particles)
    {
        // Update position
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.z += particle.vz * deltaTime;

        // Apply gravity
        particle.vy += GRAVITY * deltaTime;

        // Fade out
        particle.a = std::max(0.0f, particle.a - (deltaTime * 0.5f));

        // Update lifetime
        particle.life -= deltaTime;
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle &p)
                       { return p.life <= 0.0f; }),
        particles.end());
}

void BloodParticleSystem::emit(float x, float y, float z, bool direction)
{
    const int NUM_PARTICLES = 20;
    const float SPRAY_ANGLE = 90.0f;
    const float MIN_VELOCITY = 1.0f;
    const float MAX_VELOCITY = 3.0f;
    const float SPAWN_RADIUS = 0.001f;
    const float VERTICAL_OFFSET = 0.1f;

    float baseAngle = direction ? 180.0f : 0.0f;
    float centerY = y + VERTICAL_OFFSET;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        Particle particle;

        // Controlled spawn position
        double theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        double radius = ((float)rand() / RAND_MAX) * SPAWN_RADIUS;
        particle.x = float(x + radius * cos(theta));
        particle.y = float(centerY + radius * sin(theta));
        particle.z = z;

        // Controlled velocity
        float randomAngle = ((float)rand() / RAND_MAX) * SPRAY_ANGLE;
        float angle = glm::radians(baseAngle + (direction ? -randomAngle : randomAngle));
        float velocity = MIN_VELOCITY + ((float)rand() / RAND_MAX) * (MAX_VELOCITY - MIN_VELOCITY);

        particle.vx = velocity * cos(angle);
        particle.vy = velocity * sin(angle);
        particle.vz = 0.0f;

        // Controlled color variation
        particle.r = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;
        particle.g = 0.0f;
        particle.b = 0.0f;
        particle.a = 0.8f + ((float)rand() / RAND_MAX) * 0.2f;

        particle.life = 0.8f + ((float)rand() / RAND_MAX) * 0.5f;

        particles.push_back(particle);
    }
}