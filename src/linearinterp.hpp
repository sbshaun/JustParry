#pragma once
#include "renderer.hpp"

void interp_moveEntitesToScreen(GlRender &renderer);
void resetInterpVariables();

// Declare isLoading as extern
extern bool isLoading;