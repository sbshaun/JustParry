#pragma once

// GLWindow dimension, game window size. 
const int M_WINDOW_WIDTH_PX  = 1024;
const int M_WINDOW_HEIGHT_PX = 768;

const bool BOT_ENABLED = false;

//TIMERS
const int timer_length = 100;

// movement
// const float JUMP_HEIGHT = 0.5f;
// const float JUMP_DURATION = 1.0f;
const float MOVE_SPEED = 0.005f;

const float JUMP_VELOCITY = 0.045f;
const float GRAVITY = 0.001f;

const float TIME_STEP = 1.0f / 60.0f;

//BOUNDING
const float BOUND_SIZE_L = -1.0f;
const float BOUND_SIZE_R = 1.0f;
const float FLOOR_Y = -0.75f;
const int RIGHT = 1;
const int LEFT = 2;
const int FLOOR = 3;

const float PLAYER_STATE_TIMER_STEP = 1000 * 1 / 60.f; // 60 fps, in ms 