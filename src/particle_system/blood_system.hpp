#include "particle_system.hpp"

class BloodParticleSystem : public ParticleSystem {
public:
    BloodParticleSystem();
    void update(float deltaTime) override;
    void emit(float x, float y, float z, bool direction) override;
};