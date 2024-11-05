#include "../physics/physics_system.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../world/world_init.hpp"
#include "../constants.hpp"


void PhysicsSystem::step() {
	
	// Handling the collions
	ComponentContainer<Boundary>& boundaryContainer = registry.boundaries;
	ComponentContainer<PlayableArea>& playableAreaContainer = registry.playableArea;
	ComponentContainer<Player>& playerContainer = registry.players;

	// Iterate through the players 
	for (uint i = 0; i < playerContainer.components.size(); i++) {

		// std::cout << playerContainer.components.size() << std::endl;
		// get the Motion and HitBox associated to each player
		Entity& playerEntity = playerContainer.entities[i];
		Motion& playerMotion = registry.motions.get(playerEntity);
		HitBox& playerHitBox = registry.hitBoxes.get(playerEntity);
		Fighters current_char = playerContainer.components[i].current_char;
		const FighterConfig& config = FighterManager::getFighterConfig(current_char);

		// std::cout << playerMotion.position[0] << std::endl;

		for (uint j = 0; j < playableAreaContainer.components.size(); j++) {
			PlayableArea& playableArea = playableAreaContainer.components[j];
			float playerPosLeft = playerMotion.position.x - config.NDC_WIDTH / 2.0f;
			float playerPosRight = playerMotion.position.x + config.NDC_WIDTH / 2.0f;
			float playableAreaLeft = playableArea.position.x - playableArea.width / M_WINDOW_WIDTH_PX;
			float playableAreaRight = playableArea.position.x + playableArea.width / M_WINDOW_WIDTH_PX;

			bool rightCollision = playerPosRight > playableAreaRight;
			bool leftCollision = playerPosLeft < playableAreaLeft;

			if (rightCollision || leftCollision)
			{
				if (rightCollision) {
					playerMotion.position.x = playableAreaRight - config.NDC_WIDTH / 2.0f;
					if (playerMotion.wasAbove) {
						playerMotion.position.x -= MOVE_SPEED;
						playerMotion.velocity.y += GRAVITY;
					}
				}
				if (leftCollision) {
					playerMotion.position.x = playableAreaLeft + config.NDC_WIDTH / 2.0f;
					if (playerMotion.wasAbove) {
						playerMotion.position.x += MOVE_SPEED;
						playerMotion.velocity.y += GRAVITY;
					}
				}
			}
			
		}

		for (uint j = 0; j < boundaryContainer.components.size(); j++) { //iterate through the boundaries: check and resolve collisions with current player
			Boundary& boundary = boundaryContainer.components[j];
			// Case: Right Wall 
			if (boundary.dir == RIGHT) {
				float playerPos = playerMotion.position.x + config.NDC_WIDTH / 2.0f; 
				if (playerPos > boundary.val) 
				{
					// std::cout << "Player: " << playerMotion.position.x << "Boundary: " << boundary.val << std::endl;
					//if (!registry.boundaryCollisions.has(playerEntity)) {
					playerMotion.position.x = playerMotion.position.x - playerMotion.velocity.x;
					if (playerMotion.wasAbove) {
						playerMotion.position.x -= MOVE_SPEED;
						playerMotion.velocity.y += GRAVITY;
					}
				}
			}
			// Case: Left Wall
			else if (boundary.dir == LEFT) {
				float playerPos = playerMotion.position.x - config.NDC_WIDTH / 2.0f;
				if (playerPos < boundary.val) {
					// std::cout << "Player: " << playerMotion.position.x << "Boundary: " << boundary.val << std::endl;

					playerMotion.position.x = playerMotion.position.x - playerMotion.velocity.x;
					if (playerMotion.wasAbove) {
						playerMotion.position.x += MOVE_SPEED;
						playerMotion.velocity.y += GRAVITY;
					}
					
				}

			}
			else if (boundary.dir == FLOOR) {
				float playerPos = playerMotion.position.y - config.NDC_HEIGHT / 2.0f;
				if (playerPos < boundary.val) {
					playerMotion.position = { playerMotion.position.x - playerMotion.velocity.x, boundary.val + config.NDC_HEIGHT / 2.0f};
					playerMotion.velocity.y = 0.0f;
					playerMotion.inAir = false;
					playerMotion.above = false;
				}
			}
		}
	}

	// Handling the movements (only handle jumping characters for now)
	ComponentContainer<Motion>& motionContainer = registry.motions;
	for (uint i = 0; i < motionContainer.size(); i++) {
		Motion& motion = registry.motions.components[i];
		if (motion.inAir) {
			motion.velocity.y -= GRAVITY;
		}
	}

}

