#pragma once
#include <glm/vec2.hpp>				// vec2
using namespace glm;

enum class PlayerState { 
    IDLE,
    WALKING,
    JUMPING,
    CROUCHING,
    ATTACKING,
    PARRYING,
    PERFECT_PARRYING,
    COUNTER_ATTACKING,
    STUNNED,
    RECOVERING
};

// Player component, from A1 
struct Player {
    int id; // used to separate players, 1 and 2.
};

// AI opponent 
struct Opponent {
    int level; // increase difficulty by increasing level. 
};

struct Health {
    float maxHealth = 100.f;
    float currentHealth = 100.f; 
    // float recoverRate; // optional, health recover after x seconds of not being hit. 
};

struct Motion {
    vec2 position = {0, 0};
    vec2 velocity = {0, 0};
    bool direction = true; // true is facing right, false is facing left. 
    // vec2 acceleration = {0, 0}; // TODO: if we want to add acceleration during initial movement after idle. 
};

struct StationaryTimer {
    float counter_ms = 0.f; // period of time of a player can't move because of stun, or recovery time after actions (e.g. an attack)... 
};

struct PostureBar {
    int maxBar =  10; // max number of bars. 
    int currentBar = 10; // remaining bars.  
    int recoverRate = 3; // how many seconds to recover 1 bar. use int for implicity. 
};

struct StateTimer {
    float duration = 0.f; // time length that this state will last.
    float elapsedTime = 0.f;  // time elapsed since this state started. 
};

struct PlayerInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool jump = false;
    bool crouch = false;
    bool punch = false;
    bool kick = false;
    bool parry = false;
};

// TODO: how do we move the box along the player? 
struct HitBox {
    float x, y; 
    float width, height; 
};

struct HurtBox {
    float x, y; 
    float width, height; 
};

struct ParryBox {
    // thinking maybe when parrying, we can add a ParryBox for 2 frames, if hitbox hits ParryBox, the attack is parried. 
    float x, y; 
    float width, height; 
    bool active = false; // active for 12 frames 
};

struct PerfectParryBox {
    float x, y;
    float width, height;
    bool active = false; // active for 3 frames 
};

struct Sprite {
    // the sprite file to use for the entity. 
};


// TODO: add render related components 

// How do we implement boundries, i.e. player can't move beyond boundriess 
// 1. what components are needed 
// 2. how does system logic work on these components 