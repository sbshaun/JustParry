#pragma once

// GLWindow dimension, game window size.
const int M_WINDOW_WIDTH_PX = 1024;
const int M_WINDOW_HEIGHT_PX = 768;

const float PLAYER_BB_WIDTH = 2.5f * 80.f;
const float PLAYER_BB_HEIGHT = 2.5f * 100.f;

const bool BOT_ENABLED = false;

const float PERFECT_PARRY_TIME = 200.0f;

// LOOP LOGIC
// 1000hz = 1ms
// 240hz = 4> X >1
// 120hz = 4ms
// Conservative estimate on 10 year old cpu: 4ms per iteration(incl render). = 125hz -> 120hz logic rate + 60fps cap
// 60hz = 16.66ms
const static int TARGET_LOGIC_RATE = 120;
const static int TARGET_FPS = 60; // IDEALLY WE CHANGE 60 TO THE MONITOR REFRESH RATE BUT NOT SURE HOW TO GET THAT AND CANNOT BE LARGER THAN LOGIC RATE WITH CURRENT IMPLEMENTATION
const static float FPS_LOGIC_FACTOR = TARGET_LOGIC_RATE / TARGET_FPS;

// TIMERS
const int timer_length = 100;

// movement
// const float JUMP_HEIGHT = 0.5f;
// const float JUMP_DURATION = 1.0f;
const float MOVE_SPEED = 0.02f / FPS_LOGIC_FACTOR;

const float JUMP_VELOCITY = 0.065f / FPS_LOGIC_FACTOR;
const float GRAVITY = 0.001f / FPS_LOGIC_FACTOR;

const float TIME_STEP = 1.0f / 60.0f;

// BOUNDING
const float BOUND_SIZE_L = -1.4f;
const float BOUND_SIZE_R = 1.4f;
const float FLOOR_Y = -0.75f;
const int RIGHT = 1;
const int LEFT = 2;
const int FLOOR = 3;

const float PLAYER_STATE_TIMER_STEP = 1000 * 1 / 60.f; // 60 fps, in ms