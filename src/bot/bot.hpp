#ifndef BOT_HPP
#define BOT_HPP

#include "../common.hpp"
#include <random>
#include "../graphics/renderer.hpp"

// Bot states with clear priorities
enum class BotState
{
    CHASE,   // Primary state: get in range
    ATTACK,  // Attack when in range
    RETREAT, // Back off if too close
    JUMP,    // Jump occasionally to mix up movement
    IDLE     // Brief pauses between actions
};

class Bot
{
public:
    void pollBotRng(GlRender &renderer);
};

static BotState currentState = BotState::CHASE;
static int actionCounter = 0;

// Constants for positioning
const float IDEAL_ATTACK_DISTANCE = 0.3f; // Best distance for attacking
const float TOO_CLOSE_DISTANCE = 0.2f;    // Distance to start backing off
const float TOO_FAR_DISTANCE = 0.4f;      // Distance to start chasing

#endif
