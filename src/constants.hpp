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