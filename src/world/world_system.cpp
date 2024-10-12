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
	Entity player1 = createPlayer1(renderer, { -0.4, -0.5 });
	Entity player2 = createPlayer2(renderer, { 0.4, -0.5 });

	renderer->m_player1 = player1;
	renderer->m_player2 = player2;
}