#include "../physics/physics_system.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../world/world_init.hpp"
#include "../constants.hpp"



void PhysicsSystem::step() {
	
	// Handling the collions
	ComponentContainer<Boundary>& boundaryContainer = registry.boundaries;
	ComponentContainer<Player>& playerContainer = registry.players;

	// Compare each player to each boundary 
	for (uint i = 0; i < playerContainer.components.size(); i++) {

		// std::cout << playerContainer.components.size() << std::endl;
		// get the Motion and HitBox associated to each player
		Entity& playerEntity = playerContainer.entities[i];
		Motion& playerMotion = registry.motions.get(playerEntity);
		HitBox& playerHitBox = registry.hitBoxes.get(playerEntity);

		// std::cout << playerMotion.position[0] << std::endl;

		for (uint j = 0; j < boundaryContainer.components.size(); j++) {
			Boundary& boundary = boundaryContainer.components[j];
			// Case: Right Wall 
			if (boundary.dir == RIGHT) {
				float playerPos = playerMotion.position.x + NDC_WIDTH / 2.0f; //changed from [0] to x for readability
				if (playerPos > boundary.val) 
				{
					// std::cout << "Player: " << playerMotion.position.x << "Boundary: " << boundary.val << std::endl;
					if (!registry.boundaryCollisions.has(playerEntity)) {
						// std::cout << "COLLIDED 1 (RIGHT)" << std::endl;
						registry.boundaryCollisions.emplace(playerEntity, boundaryContainer.entities[j]);
					}
				}
			}
			// Case: Left Wall
			else if (boundary.dir == LEFT) {
				float playerPos = playerMotion.position.x - NDC_WIDTH / 2.0f;
				if (playerPos < boundary.val) {
					// std::cout << "Player: " << playerMotion.position.x << "Boundary: " << boundary.val << std::endl;
					std::cout << boundary.val << std::endl;
					if (!registry.boundaryCollisions.has(playerEntity)) {
						// std::cout << "COLLIDED 2 (LEFT)" << std::endl;
						registry.boundaryCollisions.emplace(playerEntity, boundaryContainer.entities[j]);
					}
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
