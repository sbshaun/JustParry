#include "particle_system.hpp"

ParticleSystem::ParticleSystem() {
    shader = new Shader("particle");
    std::cout << "Shader program ID: " << shader->m_shaderProgram << std::endl;
    init();
}

void ParticleSystem::init() {
    // Base particle quad vertices
    float quadVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Create buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    // Set up vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Set up element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Instance data buffer
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * 1000, nullptr, GL_DYNAMIC_DRAW);

    // Position attribute (per instance)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, x));
    glVertexAttribDivisor(1, 1);

    // Color attribute (per instance)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, r));
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime) {
    if (particles.empty()) {
        return;
    }

    const float GRAVITY = -9.8f;

    for (auto& particle : particles) {
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
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );
}

void ParticleSystem::render() {
    if (particles.empty()) {
        return;
    }

    shader->use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * particles.size(), particles.data());

    // Draw particles using instancing
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, particles.size());

    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void ParticleSystem::emit(float x, float y, float z, bool direction) {
    const int NUM_PARTICLES = 20;
    const float SPRAY_ANGLE = 90.0f;
    const float MIN_VELOCITY = 1.0f;
    const float MAX_VELOCITY = 3.0f;
    const float SPAWN_RADIUS = 0.001f;
    const float VERTICAL_OFFSET = 0.1f;

    float baseAngle = direction ? 180.0f : 0.0f;
    float centerY = y + VERTICAL_OFFSET;

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle particle;

        // Controlled spawn position
        float theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float radius = ((float)rand() / RAND_MAX) * SPAWN_RADIUS;
        particle.x = x + radius * cos(theta);
        particle.y = centerY + radius * sin(theta);
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