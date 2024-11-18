#include "smoke_system.hpp"

SmokeParticleSystem::SmokeParticleSystem() {
    shader = new Shader("particle");
    std::cout << "Shader program ID: " << shader->m_shaderProgram << std::endl;
    init();
}

void SmokeParticleSystem::update(float deltaTime) {
    if (particles.empty()) {
        return;
    }

    for (auto& particle : particles) {
        particle.x += particle.vx * deltaTime;
        particle.y += particle.vy * deltaTime;
        particle.z += particle.vz * deltaTime;

        particle.a = std::max(0.0f, particle.a - (deltaTime * 0.1f));

        particle.life -= deltaTime;

        particle.vy += 1.0f * deltaTime;
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );

    // Ensure we only have a maximum of 40 particles
    if (particles.size() > 40) {
        particles.erase(particles.begin(), particles.begin() + (particles.size() - 40));
    }
}

void SmokeParticleSystem::emit(float x, float y, float z, bool direction) {
	if (particles.size() > 40) {
		return;
	}

    const int NUM_PARTICLES = 10;
    const float MIN_VELOCITY = 0.1f;
    const float MAX_VELOCITY = 0.5f;
    const float SPAWN_RADIUS = 0.05f;

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle particle;

        float theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float radius = ((float)rand() / RAND_MAX) * SPAWN_RADIUS;
        particle.x = x + radius * cos(theta);
        particle.y = y + radius * sin(theta);
        particle.z = z;

        float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float velocity = MIN_VELOCITY + ((float)rand() / RAND_MAX) * (MAX_VELOCITY - MIN_VELOCITY);

        particle.vx = velocity * cos(angle) * 0.1f;
        particle.vy = velocity * sin(angle) * 0.1f + 0.1f;
        particle.vz = 0.0f;

        float gray = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
        particle.r = gray;
        particle.g = gray;
        particle.b = gray;
        particle.a = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;

        particle.life = 2.0f + ((float)rand() / RAND_MAX) * 1.0f;

        particles.push_back(particle);
    }
}