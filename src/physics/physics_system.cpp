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
		FighterConfig config = FighterManager::getFighterConfig(current_char);

		// std::cout << playerMotion.position[0] << std::endl;

		for (uint j = 0; j < playableAreaContainer.components.size(); j++) {
			PlayableArea& playableArea = playableAreaContainer.components[j];
			float playerPosLeft = playerMotion.position.x - config.NDC_WIDTH / 2.0f;
			float playerPosRight = playerMotion.position.x + config.NDC_WIDTH / 2.0f;
			float playableAreaLeft = playableArea.position.x - playableArea.width / M_WINDOW_WIDTH_PX;
			float playableAreaRight = playableArea.position.x + playableArea.width / M_WINDOW_WIDTH_PX;

			bool rightCollision = playerPosRight > playableAreaRight;
			bool leftCollision = playerPosLeft < playableAreaLeft;

			// std::cout << playerPosLeft << std::endl;

			if (rightCollision || leftCollision)
			{
				playerMotion.position.x = playerMotion.position.x - playerMotion.velocity.x;
			}
			
		}
		float movespeed = FighterManager::getFighterConfig(registry.players.get(playerEntity).current_char).MOVESPEED;
		for (uint j = 0; j < boundaryContainer.components.size(); j++) { //iterate through the boundaries: check and resolve collisions with current player
			Boundary& boundary = boundaryContainer.components[j];
			// Case: Right Wall 
			if (boundary.dir == RIGHT) {
				float playerPos = playerMotion.position.x + config.NDC_WIDTH / 2.0f; 
				
				if (playerPos > boundary.val) 
				{
					//if (!registry.boundaryCollisions.has(playerEntity)) {
					playerMotion.position.x = playerMotion.position.x - movespeed;
					if (playerMotion.wasAbove) {
						playerMotion.position.x -= movespeed;
						playerMotion.velocity.y += GRAVITY;
					}
				}
			}
			// Case: Left Wall
			else if (boundary.dir == LEFT) {
				float playerPos = playerMotion.position.x - config.NDC_WIDTH / 2.0f;
				if (playerPos < boundary.val) {
					playerMotion.position.x = playerMotion.position.x + movespeed;
					if (playerMotion.wasAbove) {
						playerMotion.position.x += movespeed;
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

	// Handle knock-back physics
	for (Entity entity : registry.knockbacks.entities) {
		KnockBack& knockBack = registry.knockbacks.get(entity);
		if (knockBack.active) {
			// std::cout << knockBack.active << std::endl;
			Motion& motion = registry.motions.get(entity);

			// Debug print before setting velocity
			//std::cout << "Knockback Physics:" << std::endl;
			//std::cout << "Force: (" << knockBack.force.x << ", " << knockBack.force.y << ")" << std::endl;
			//std::cout << "Old Velocity: (" << motion.velocity.x << ", " << motion.velocity.y << ")" << std::endl;

			// Apply knock-back force
			motion.position += knockBack.force;
			//motion.velocity = knockBack.force;

			// Reduce knock-back duration
			knockBack.duration -= PLAYER_STATE_TIMER_STEP;

			// If knock-back duration is over, reset
			if (knockBack.duration <= 0) {
				knockBack.active = false;
				knockBack.duration = 0;
				motion.velocity = { 0.0f, 0.0f };
			}

			// Apply gravity during knock-back
			if (motion.inAir) {
				motion.position.y -= GRAVITY;
			}
		}
	}

}

