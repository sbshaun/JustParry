#include "particle_system.hpp"

class SmokeParticleSystem : public ParticleSystem {
public:
    SmokeParticleSystem();
    void update(float deltaTime) override;
    void emit(float x, float y, float z, bool direction) override;
};