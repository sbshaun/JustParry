#include "../physics/physics_system.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../world/world_init.hpp"



void PhysicsSystem::step(float elapsed_ms) {
	
	// Handling the collions
	ComponentContainer<Boundary>& boundaryContainer = registry.boundaries;
	ComponentContainer<Player>& playerContainer = registry.players;

	// Compare each player to each boundary 
	for (uint i = 0; i < playerContainer.components.size(); i++) {

		// get the Motion and HitBox associated to each player
		Entity& playerEntity = playerContainer.entities[i];
		Motion& playerMotion = registry.motions.get(playerEntity);
		HitBox& playerHitBox = registry.hitBoxes.get(playerEntity);

		for (uint j = 0; j < boundaryContainer.components.size(); j++) {
			Boundary& boundary = boundaryContainer.get(playerEntity);

			// Case: Right Wall 
			if (boundary.dir == 1) {
				float playerXRightPosition = playerHitBox.x;
				if (boundary.val > playerXRightPosition) {
					registry.boundaryCollisions.emplace(playerEntity, boundaryContainer.entities[i]);
				}

			}
			// Case: Left Wall
			else if (boundary.dir == 2) {
				float playerXLeftPosition = playerHitBox.x + playerHitBox.width;
				if (boundary.val < playerXLeftPosition) {
					registry.boundaryCollisions.emplace(playerEntity, boundaryContainer.entities[i]);
				}

			}
		}
	}
	// Case: Floor
	/*else if (boundary.dir == 3) {
		if (boundary.val > playerYPosition) {
			
		}
		
	}*/
}
/*
	Physics System:
	- Handle Collision Check
	- Knockback
	- Apply gravity and momentum calculations
*/
