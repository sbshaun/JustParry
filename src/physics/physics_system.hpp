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
	- Handle Stage Collision Check
	- Knockback
	- Apply gravity and momentum calculations
*/