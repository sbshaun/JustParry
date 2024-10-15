#pragma once

// GLWindow dimension, game window size. 
const int M_WINDOW_WIDTH_PX  = 1024;
const int M_WINDOW_HEIGHT_PX = 768;

// hardcoded to the dimensions of the entity texture (size of the mesh/texture)
// BB = bounding box
// player 1 
const float PLAYER_1_BB_WIDTH  = 3.0f * 80.f; // A typical height is 100px, reference; http://petesqbsite.com/sections/tutorials/tuts/tsugumo/chapter9.htm 
const float PLAYER_1_BB_HEIGHT = 3.0f * 100.f; 
// player 2 
const float PLAYER_2_BB_WIDTH  = 3.0f * 80.f;
const float PLAYER_2_BB_HEIGHT = 3.0f * 100.f; 

const float NDC_WIDTH = PLAYER_1_BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f;
const float NDC_HEIGHT = PLAYER_1_BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f;


// AI opponent 
const float OPPONENT_1_BB_WIDTH   = 3.0f * 80.f;
const float OPPONENT_1_BB_HEIGHT  = 3.0f * 100.f;
const bool BOT_ENABLED = true;

//TIMERS
const int timer_length = 100;

// movement
// const float JUMP_HEIGHT = 0.5f;
// const float JUMP_DURATION = 1.0f;
const float MOVE_SPEED = 0.005f;

const float JUMP_VELOCITY = 0.02f;
const float GRAVITY = 0.001f;

const float TIME_STEP = 1.0f / 60.0f;


//BOUNDING
const float BOUND_SIZE_L = -1.0f;
const float BOUND_SIZE_R = 1.0f;
const float FLOOR_Y = -0.75f;
const int RIGHT = 1;
const int LEFT = 2;
const int FLOOR = 3;

// Hitbox 
// active duration 
const int PLAYER_1_HITBOX_DURATION = 500; // 30 frames = 500ms 
// punch box offset 
const float PLAYER_1_PUNCH_X_OFFSET = 0.f;
const float PLAYER_1_PUNCH_Y_OFFSET = 0.15f * NDC_HEIGHT; // height of the left upper corner of the hitbox
// punch box size
const float PLAYER_1_PUNCH_WIDTH = 0.50f * NDC_WIDTH;
const float PLAYER_1_PUNCH_HEIGHT = 0.20f * NDC_HEIGHT; // 1/3 the player height 

const float PLAYER_2_PUNCH_X_OFFSET = 0.5f * NDC_WIDTH;
const float PLAYER_2_PUNCH_Y_OFFSET = 0.10f * NDC_HEIGHT;

const float PLAYER_2_PUNCH_WIDTH = 0.50f * NDC_WIDTH;
const float PLAYER_2_PUNCH_HEIGHT = 0.20f * NDC_HEIGHT; // 1/3 the player height

const float PLAYER_STATE_TIMER_STEP = 1000 * 1 / 60.f; // 60 fps, in ms 

const float PLAYER_1_DAMAGE = 10.f;
const float PLAYER_2_DAMAGE = 10.f;

// TODO: decrease value. Intentionally set it to longer for development. 
const float PLAYER_1_STUN_DURATION = 3000.f; // 1 second 
const float PLAYER_2_STUN_DURATION = 3000.f; // 1 second

//Game Component Init values
const float MAX_HEALTH = 100.f;
const int POSTURE_MAX = 10;
const int POSTURE_REGEN = 3;