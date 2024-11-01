#include "bot.hpp"
#include <iostream>

enum BotState
{
    IDLE,
    MOVE_LEFT,
    MOVE_RIGHT,
    ATTACK,
    JUMP
};

static int actionCounter = 0;
static BotState currentState = IDLE;

BotState pickRandomAction()
{
    int rng = rand() % 100;

    // More aggressive action distribution
    if (rng < 35)
    { // 35% chance to move
        return (rand() % 2 == 0) ? MOVE_LEFT : MOVE_RIGHT;
    }
    else if (rng < 65)
    { // 30% chance to attack
        return ATTACK;
    }
    else if (rng < 85)
    { // 20% chance to jump
        return JUMP;
    }
    else
    { // 15% chance to idle
        return IDLE;
    }
}

void Bot::pollBotRng(GlRender &renderer)
{
    Entity player2 = renderer.m_player2;
    PlayerInput &player2Input = registry.playerInputs.get(player2);
    Motion &player2Motion = registry.motions.get(player2);
    PlayerCurrentState &state = registry.playerCurrentStates.get(player2);

    // Reset inputs
    player2Input = PlayerInput();

    // Don't take actions if stunned
    if (state.currentState == PlayerState::STUNNED)
    {
        return;
    }

    // Get player positions for distance-based decisions
    Motion &player1Motion = registry.motions.get(renderer.m_player1);
    float distance = abs(player1Motion.position.x - player2Motion.position.x);

    // Decrement action counter
    actionCounter--;

    // Pick new action if counter expires
    if (actionCounter <= 0)
    {
        // More likely to attack when close to player
        if (distance < 0.3f && rand() % 2 == 0)
        {
            currentState = ATTACK;
            actionCounter = 20;
        }
        else
        {
            currentState = pickRandomAction();
            actionCounter = 30 + (rand() % 30); // Random duration between 30-60 frames
        }
    }

    // Execute current action
    switch (currentState)
    {
    case MOVE_LEFT:
        player2Input.left = true;
        // Sometimes attack while moving
        if (distance < 0.3f && rand() % 20 == 0)
        {
            player2Input.punch = true;
        }
        break;

    case MOVE_RIGHT:
        player2Input.right = true;
        // Sometimes attack while moving
        if (distance < 0.3f && rand() % 20 == 0)
        {
            player2Input.kick = true;
        }
        break;

    case ATTACK:
        if (state.currentState != PlayerState::ATTACKING)
        {
            // Mix up attacks
            if (rand() % 3 == 0)
            {
                player2Input.kick = true;
            }
            else
            {
                player2Input.punch = true;
            }
            actionCounter = 15; // Short attack duration
        }
        break;

    case JUMP:
        if (!player2Motion.inAir)
        {
            player2Input.up = true;
            // Sometimes attack while jumping
            if (rand() % 3 == 0)
            {
                player2Input.punch = (rand() % 2 == 0);
                player2Input.kick = !player2Input.punch;
            }
        }
        break;

    case IDLE:
        // During idle, check if should chase player
        if (distance > 0.4f)
        {
            currentState = (player2Motion.position.x < player1Motion.position.x) ? MOVE_RIGHT : MOVE_LEFT;
            actionCounter = 30;
        }
        break;
    }

    // Override behavior to chase if too far from player
    if (distance > 0.5f && state.currentState != PlayerState::ATTACKING)
    {
        player2Input.left = (player2Motion.position.x > player1Motion.position.x);
        player2Input.right = !player2Input.left;

        // Maybe jump while chasing
        if (!player2Motion.inAir && rand() % 20 == 0)
        {
            player2Input.up = true;
        }
    }
}
