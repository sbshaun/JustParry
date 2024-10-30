#pragma once

#include <glm/vec2.hpp>
#include "../ecs/ecs.hpp"
#include "../renderer.hpp"
#include "../ecs/ecs.hpp"

using namespace glm; 

// Functions to create entities and init components 
Entity createPlayer1(GlRender* renderer, vec2 pos, Fighters fighter);
Entity createPlayer2(GlRender* renderer, vec2 pos, Fighters fighter);
Entity createOpponent1(GlRender* renderer, vec2 pos);
Entity createBoundary(float val, int type);
Entity createFloor(float val, int type);

Entity createFloor(GlRender* renderer);