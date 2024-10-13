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

const float NDC_WIDTH = PLAYER_1_BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f;
const float NDC_HEIGHT = PLAYER_1_BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f;

// AI opponent 
const float OPPONENT_1_BB_WIDTH   = 1.0f * 100.f;
const float OPPONENT_1_BB_HEIGHT  = 1.0f * 50.f;

// movement
// const float JUMP_HEIGHT = 0.5f;
// const float JUMP_DURATION = 1.0f;
// const float GRAVITY = 0.1f;
const float MOVE_SPEED = 0.005f;

const float JUMP_VELOCITY = 0.015f;
const float GRAVITY = 0.0005f;

const float TIME_STEP = 1.0f / 60.0f;


//BOUNDING
const float BOUND_SIZE_L = -0.75;
const float BOUND_SIZE_R = 0.75;
const float FLOOR_Y = -0.5f;
const int RIGHT = 1;
const int LEFT = 2;
const int FLOOR = 3;

// Hitbox 
// activie duration 
const int PLAYER_1_HITBOX_DURATION = 500; // 30 frames = 500ms 
// punch box offset 
const float PLAYER_1_PUNCH_X_OFFSET = .5f * PLAYER_1_BB_WIDTH;
const float PLAYER_1_PUNCH_Y_OFFSET = .5f * PLAYER_1_BB_HEIGHT; // height of the left upper corner of the hitbox 
// punch box size
const float PLAYER_1_PUNCH_WIDTH = 1.0f * PLAYER_1_BB_WIDTH; 
const float PLAYER_1_PUNCH_HEIGHT = 0.333f * PLAYER_1_BB_HEIGHT; // 1/3 the player height 

const float PLAYER_2_PUNCH_X_OFFSET = .5f * PLAYER_2_BB_WIDTH;
const float PLAYER_2_PUNCH_Y_OFFSET = .5f * PLAYER_2_BB_HEIGHT; // height of the left upper corner of the hitbox
const float PLAYER_2_PUNCH_WIDTH = 1.0f * PLAYER_2_BB_WIDTH;
const float PLAYER_2_PUNCH_HEIGHT = 0.333f * PLAYER_2_BB_HEIGHT; // 1/3 the player height

const float PLAYER_STATE_TIMER_STEP = 1000 * 1 / 60.f; // 60 fps, in ms 

const float PLAYER_1_DAMAGE = 10.f;
const float PLAYER_2_DAMAGE = 10.f;

// TODO: decrease value. Intentionally set it to longer for development. 
const float PLAYER_1_STUN_DURATION = 3000.f; // 1 second 
const float PLAYER_2_STUN_DURATION = 3000.f; // 1 second