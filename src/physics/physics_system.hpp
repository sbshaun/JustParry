#pragma once
#include "../common.hpp"
#include "../ecs/ecs_registry.hpp"

class PhysicsSystem
{
public:
	void step();

	PhysicsSystem()
	{
	}
};
/*
	Physics System:
	- Handle Collision Check
	- Knockback
	- Apply gravity and momentum calculations
*/