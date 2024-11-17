#include <vector>
#include <algorithm>
#include "../shader.hpp"
// #include "../common.hpp"
#include "../ecs/ecs_registry.hpp"

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float r, g, b, a; // Color and alpha
    float size;
    float rotation;
};

class ParticleSystem {
public:
    ParticleSystem();
    void update(float deltaTime);
    void render(const glm::mat4& worldModel);
    void emit(float x, float y, float z, bool direction);

private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO, EBO, instanceVBO;
    Shader* shader;
    void init();
};
