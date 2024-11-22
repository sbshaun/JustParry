#include "sparkle_system.hpp"

SparkleParticleSystem::SparkleParticleSystem() {
    shader = new Shader("particle");
    std::cout << "Shader program ID: " << shader->m_shaderProgram << std::endl;
    init();
}

void SparkleParticleSystem::update(float deltaTime) {
    if (particles.empty()) {
        return;
    }

    for (auto& particle : particles) {
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.z += particle.vz * deltaTime;

        particle.a = std::max(0.0f, particle.a - (deltaTime * 0.5f));

        particle.life -= deltaTime;
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );
}

void SparkleParticleSystem::emit(float x, float y, float z, bool direction) {
    const int NUM_PARTICLES = 30;
    const float MIN_VELOCITY = 0.5f;
    const float MAX_VELOCITY = 2.0f;
    const float SPAWN_RADIUS = 0.001f;

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle particle;

        float theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float radius = ((float)rand() / RAND_MAX) * SPAWN_RADIUS;
        particle.x = x + radius * cos(theta);
        particle.y = y + radius * sin(theta);
        particle.z = z;

        float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float velocity = MIN_VELOCITY + ((float)rand() / RAND_MAX) * (MAX_VELOCITY - MIN_VELOCITY);

        particle.vx = velocity * cos(angle);
        particle.vy = velocity * sin(angle);
        particle.vz = 0.0f;

        particle.r = 1.0f;
        particle.g = 1.0f;
        particle.b = 0.0f;
        particle.a = 1.0f;

        particle.life = 0.8f + ((float)rand() / RAND_MAX) * 0.5f;

        particles.push_back(particle);
    }
}