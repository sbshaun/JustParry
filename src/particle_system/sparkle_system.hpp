#pragma once

#include "particle_system.hpp"

class SparkleParticleSystem : public ParticleSystem {
public:
    SparkleParticleSystem();
    void update(float deltaTime) override;
    void emit(float x, float y, float z, bool direction) override;
};