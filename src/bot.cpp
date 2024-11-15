#include "bot.hpp"
#include <iostream>
#include <functional>
#include <memory>
#include <cstdlib>

// Helper function to pick next action based on distance
BotState pickNextAction(float distance)
{
    // Always prioritize getting into position
    if (distance > TOO_FAR_DISTANCE)
    {
        return BotState::CHASE;
    }
    else if (distance < TOO_CLOSE_DISTANCE)
    {
        return BotState::RETREAT;
    }
    // When in ideal range, choose an action
    else
    {
        int rng = rand() % 100;
        if (rng < 50)
        { // 50% chance to attack when in range (reduced from 60%)
            return BotState::ATTACK;
        }
        else if (rng < 70)
        { // 20% chance to adjust position
            return (distance > IDEAL_ATTACK_DISTANCE) ? BotState::CHASE : BotState::RETREAT;
        }
        //else if (rng < 75)
        //{ // 15% chance to jump
        //    return BotState::JUMP;
        //}
        else
        { // 30% chance to pause (increased from 10%)
            return BotState::IDLE;
        }
    }
}

void Bot::pollBotRng(GlRender &renderer)
{
    Entity player2 = renderer.m_player2;
    Entity player1 = renderer.m_player1;

    PlayerInput &player2Input = registry.playerInputs.get(player2);
    Motion &player2Motion = registry.motions.get(player2);
    Motion &player1Motion = registry.motions.get(player1);
    PlayerCurrentState &state = registry.playerCurrentStates.get(player2);

    // Reset inputs
    player2Input = PlayerInput();

    // Don't take actions if stunned
    if (state.currentState == PlayerState::STUNNED)
    {
        return;
    }

    // Calculate distance and direction to player 1
    float distance = abs(player1Motion.position.x - player2Motion.position.x);
    bool moveLeft = (player2Motion.position.x > player1Motion.position.x);

    // Decrement action counter
    actionCounter--;

    // Pick new action if counter expires or distance changes significantly
    if (actionCounter <= 0 ||
        (currentState == BotState::ATTACK && distance > IDEAL_ATTACK_DISTANCE * 1.2f))
    {
        currentState = pickNextAction(distance);

        // Set appropriate duration for each action (increased durations)
        switch (currentState)
        {
        case BotState::ATTACK:
            actionCounter = 45; // Increased from 20
            break;
        case BotState::CHASE:
        case BotState::RETREAT:
            actionCounter = 60; // Increased from 30
            break;
        case BotState::JUMP:
            actionCounter = 50; // Increased from 40
            break;
        case BotState::IDLE:
            actionCounter = 35; // Increased from 15
            break;
        }
    }

    // Execute current action
    switch (currentState)
    {
    case BotState::CHASE:
        // Move towards player
        player2Input.left = moveLeft;
        player2Input.right = !moveLeft;

        // Maybe jump to close distance faster (reduced frequency)
        if (!player2Motion.inAir && rand() % 45 == 0)
        { // Increased from 30
            player2Input.up = true;
        }
        break;

    case BotState::ATTACK:
        // Only attack if in good range
        if (distance <= IDEAL_ATTACK_DISTANCE * 1.2f)
        {
            if (rand() % 4 == 0)
            { // Reduced attack frequency
                player2Input.kick = true;
            }
            else if (rand() % 3 == 0)
            {
                player2Input.punch = true;
            }
        }
        else
        {
            // If target moved out of range, go back to chasing
            currentState = BotState::CHASE;
        }
        break;

    case BotState::RETREAT:
        // Move away from player
        player2Input.right = moveLeft;
        player2Input.left = !moveLeft;

        // Maybe attack while retreating (reduced frequency)
        if (rand() % 30 == 0)
        { // Increased from 20
            player2Input.punch = true;
        }
        break;

    //case BotState::JUMP:
    //    if (!player2Motion.inAir)
    //    {
    //        player2Input.up = true;
    //        // Attack during jump if in range (reduced frequency)
    //        if (distance <= IDEAL_ATTACK_DISTANCE && rand() % 3 == 0)
    //        {
    //            player2Input.punch = (rand() % 2 == 0);
    //            player2Input.kick = !player2Input.punch;
    //        }
    //    }
    //    break;

    case BotState::IDLE:
        // Longer idle periods
        break;
    }

    // Emergency chase if player gets too far away (reduced urgency)
    if (distance > TOO_FAR_DISTANCE * 1.8f && // Increased from 1.5f
        state.currentState != PlayerState::ATTACKING 
        //&& state.currentState != PlayerState::JUMPING
    )
    {
        player2Input.left = moveLeft;
        player2Input.right = !moveLeft;
        currentState = BotState::CHASE;
    }
}
