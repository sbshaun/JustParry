#include "physics_system.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../world/world_init.hpp"

bool edgeCollides(const HitBox& player, const Boundary& boundary) {
	float playerXLeftPosition = player.x;
	float playerXRightPosition = player.x + player.width;
	float playerYPosition = player.y + player.height;
	// Case: Right Wall 
	if (boundary.dir == 1) {
		if (boundary.val > playerXRightPosition) {
			registry.boundaryCollisions.emplace_with_duplicates();
		}
		else false;
	} 
	// Case: Left Wall
	else if (boundary.dir == 2) {
		if (boundary.val < playerXLeftPosition) {
			return true;
		}
		else false;
	}
	// Case: Floor
	else if (boundary.dir == 3) {
		if (boundary.val > playerYPosition) {
			return true;
		}
		else false;
	}
}
/*
	Physics System:
	- Handle Collision Check
	- Knockback
	- Apply gravity and momentum calculations
*/