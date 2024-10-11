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
	Entity player = createPlayer1(renderer, { 0, 0 });
}