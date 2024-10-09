#pragma once

#include <glm/vec2.hpp>
#include "../ecs/ecs.hpp"
#include "../renderer.hpp"

// hardcoded to the dimensions of the entity texture 
// BB = bounding box 
const float PLAYER_1_BB_WIDTH  = 1.0f * 100.f; // A typical height is 100px, reference; http://petesqbsite.com/sections/tutorials/tuts/tsugumo/chapter9.htm 
const float PLAYER_1_BB_HEIGHT = 1.0f * 50.f; 

const float PLAYER_2_BB_WIDTH  = 1.0f * 100.f; 
const float PLAYER_2_BB_HEIGHT = 1.0f * 50.f; 

const float OPPONENT_1_BB_WIDTH   = 1.0f * 100.f;
const float OPPONENT_1_BB_HEIGHT  = 1.0f * 50.f;

// Functions to create entities and init components 
Entity createPlayer1(GlRender* renderer, vec2 pos);
Entity createPlayer2(GlRender* renderer, vec2 pos);
Entity createOpponent1(GlRender* renderer, vec2 pos);

Entity createBoundary(vec2 pos, vec2 size);