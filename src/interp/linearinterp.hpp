#pragma once
#include "../graphics/renderer.hpp"
#include "../state/game.hpp"

void interp_moveEntitesToScreen(GlRender &renderer, Game &game);
void resetInterpVariables();

// Declare isLoading as extern
extern bool isLoading;