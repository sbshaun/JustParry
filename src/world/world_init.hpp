#pragma once

#include <glm/vec2.hpp>
#include "../ecs/ecs.hpp"
#include "../renderer.hpp"

using namespace glm;

// Functions to create entities and init components
Entity createPlayer1(GlRender *renderer, vec2 pos, Fighters fighter);
Entity createPlayer2(GlRender *renderer, vec2 pos, Fighters fighter);
Entity createOpponent1(GlRender *renderer, vec2 pos);
Entity createBoundary(float val, int type);
Entity createPlayableArea(const vec2& pos, float width, float height);
Entity createFloor(float val, int type);

Entity createFloor(GlRender *renderer);
Entity createNotification(float counter_ms, bool player1Side, GLuint texture_id);

// Add cleanup function declaration
void cleanupShaders(Entity entity);