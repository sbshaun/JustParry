#include "world_system.hpp"
#include "world_init.hpp"

#include "../physics/physics_system.hpp"

WorldSystem::WorldSystem() {};

WorldSystem::~WorldSystem() {
	registry.clear_all_components();
}

void WorldSystem::init(GlRender *renderer) {
	this->renderer = renderer;

	// Create entities
	Entity player = createPlayer1(renderer, { -0.4, -0.5 });
	Entity player2 = createPlayer1(renderer, { 0.4, -0.5 });

	// Create boundaries
	Entity boundaryRightWall = createBoundary(1.5, 1); // Right Side Wall
	Entity boundaryLeftWall = createBoundary(-1.5, 2); // Left Side Wall
	Entity boundaryFloor = createBoundary(-0.5, 3); // Floor
}

void WorldSystem::handle_collisions() {
	auto& collisionsRegistry = registry.boundaryCollisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		Entity player = collisionsRegistry.entities[i];
	}
}