#pragma once

// GLWindow dimension, game window size. 
const int M_WINDOW_WIDTH_PX  = 1024;
const int M_WINDOW_HEIGHT_PX = 768;

// hardcoded to the dimensions of the entity texture (size of the mesh/texture)
// BB = bounding box 
// player 1 
const float PLAYER_1_BB_WIDTH  = 1.0f * 100.f; // A typical height is 100px, reference; http://petesqbsite.com/sections/tutorials/tuts/tsugumo/chapter9.htm 
const float PLAYER_1_BB_HEIGHT = 3.0f * 50.f; 
// player 2 
const float PLAYER_2_BB_WIDTH  = 1.0f * 100.f; 
const float PLAYER_2_BB_HEIGHT = 3.0f * 50.f; 
// AI opponent 
const float OPPONENT_1_BB_WIDTH   = 1.0f * 100.f;
const float OPPONENT_1_BB_HEIGHT  = 1.0f * 50.f;

// movement
const float JUMP_HEIGHT = 0.5f;
const float JUMP_DURATION = 1.0f;
const float GRAVITY = 0.1f;
const float MOVE_SPEED = 0.005f;

//BOUNDING
const float BOUND_SIZE_L = -0.5;
const float BOUND_SIZE_R = 0.75;
const int LEFT = 2;
const int RIGHT = 1;