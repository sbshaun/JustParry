#pragma once
#include "../graphics/renderer.hpp"
#include "../state/game.hpp"

void interp_moveEntitesToScreen(GlRender &renderer, Game &game);
void resetInterpVariables();
void placePlayersOnScreen(GlRender& renderer);

// Declare isLoading as extern
extern bool isLoading;