#ifndef BOT_HPP
#define BOT_HPP

#include "../common.hpp"
#include <random>
#include "../graphics/renderer.hpp"
#include "../input_system/state_machine.hpp"

// Bot states with clear priorities
enum class BotState
{
    CHASE,   // Primary state: get in range
    ATTACK,  // Attack when in range
    RETREAT, // Back off if too close
    IDLE,    // Brief pauses between actions
    PARRY,   // Parry incoming attacks
};

class Bot
{
public:
    void pollBotRng(GlRender &renderer, StateMachine &stateMachine, int currentLevel);
};

static BotState currentState = BotState::CHASE;
static int actionCounter = 0;
static int parryCounter = 0;

// Constants for positioning
const float IDEAL_ATTACK_DISTANCE = 0.18f; // Best distance for attacking
const float TOO_CLOSE_DISTANCE = 0.15f;    // Distance to start backing off
const float TOO_FAR_DISTANCE = 0.35f;      // Distance to start chasing
const int BOT_PARRY_COOLDOWN = 1000;  // Cooldown for parry

#endif
