#pragma once

#include <vector>
#include <algorithm>
#include "../graphics/shader.hpp"
#include "../ecs/ecs_registry.hpp"

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float r, g, b, a;
    float size;
    float rotation;
};

class ParticleSystem {
public:
    ParticleSystem();
    virtual ~ParticleSystem() = default;
    virtual void update(float deltaTime) = 0;
    void render(const glm::mat4& worldModel);
    virtual void emit(float x, float y, float z, bool direction) = 0;

protected:
    std::vector<Particle> particles;
    unsigned int VAO, VBO, EBO, instanceVBO;
    Shader* shader;
    void init();
};