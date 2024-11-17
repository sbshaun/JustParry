#include <vector>
#include "../shader.hpp"
#include "../common.hpp"

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float r, g, b, a; // Color and alpha
};

class ParticleSystem {
public:
    ParticleSystem();
    void update(float deltaTime);
    void render();
    void emit(float x, float y, float z, bool direction);

private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    Shader* shader;
    void init();
};
