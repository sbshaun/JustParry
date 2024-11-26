#include "bot.hpp"
#include <iostream>
#include <functional>
#include <memory>
#include <cstdlib>

// Helper function to calculate utility for different actions
float calculateUtility(BotState state, float distance, PlayerState currentState, float health, float opponentHealth)
{
    float utility = 0.0f;

    switch (state)
    {
    case BotState::CHASE:
        utility = (distance > IDEAL_ATTACK_DISTANCE) ? 10.0f : -5.0f;
        break;
    case BotState::ATTACK:
        utility = (distance <= IDEAL_ATTACK_DISTANCE) ? 20.0f : -10.0f;
        break;
    case BotState::RETREAT:
        utility = (distance < TOO_CLOSE_DISTANCE) ? 21.0f : -5.0f;
        break;
    case BotState::IDLE:
        utility = 5.0f;
        break;
    case BotState::PARRY:
        utility = (currentState == PlayerState::ATTACKING) ? 25.0f : -5.0f; // Increased utility for parrying
        break;
    }

    if (health < opponentHealth)
    {
        utility -= 5.0f;
    }
    else
    {
        utility += 5.0f;
    }

    return utility;
}

// Helper function to pick next action based on utilities
BotState pickNextAction(float distance, PlayerState currentState, float health, float opponentHealth)
{
    std::vector<std::pair<BotState, float>> actions = {
        {BotState::CHASE, calculateUtility(BotState::CHASE, distance, currentState, health, opponentHealth)},
        {BotState::ATTACK, calculateUtility(BotState::ATTACK, distance, currentState, health, opponentHealth)},
        {BotState::RETREAT, calculateUtility(BotState::RETREAT, distance, currentState, health, opponentHealth)},
        {BotState::IDLE, calculateUtility(BotState::IDLE, distance, currentState, health, opponentHealth)},
        {BotState::PARRY, calculateUtility(BotState::PARRY, distance, currentState, health, opponentHealth)}};

    // Sort actions based on utilities
    std::sort(actions.begin(), actions.end(), [](const std::pair<BotState, float> &a, const std::pair<BotState, float> &b) {
        return a.second > b.second;
    });

    // Add randomness to the decision-making process
    //if (rand() % 100 < 20) // 20% chance to pick a different action
    //{
    //    return actions[rand() % actions.size()].first;
    //}
    if (actions.front().first == BotState::PARRY) {
		std::cout << "Bot decided to Parry" << std::endl;
    }

    // Return the action with the highest utility
    return actions.front().first;
}

void Bot::pollBotRng(GlRender &renderer, StateMachine &stateMachine)
{
    Entity player2 = renderer.m_player2;
    Entity player1 = renderer.m_player1;

    PlayerInput &player2Input = registry.playerInputs.get(player2);
    Motion &player2Motion = registry.motions.get(player2);
    Motion &player1Motion = registry.motions.get(player1);
    PlayerCurrentState &state = registry.playerCurrentStates.get(player2);
	PlayerCurrentState& player1State = registry.playerCurrentStates.get(player1);
    Health &player2Health = registry.healths.get(player2);
    Health &player1Health = registry.healths.get(player1);

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

    // Pick new action if counter expires
    if (actionCounter <= 0)
    {
        currentState = pickNextAction(distance, player1State.currentState, player2Health.currentHealth, player1Health.currentHealth);

        // Set appropriate duration for each action
        switch (currentState)
        {
        case BotState::ATTACK:
            actionCounter = 50;
            break;
        case BotState::CHASE:
        case BotState::RETREAT:
            actionCounter = 70;
            break;
        case BotState::IDLE:
            actionCounter = 45;
            break;
        case BotState::PARRY:
            actionCounter = 40; // Reduced duration for quicker parry reaction
            break;
        }
    }

    // Execute current action
    switch (currentState)
    {
    case BotState::CHASE:
        player2Input.left = moveLeft;
        player2Input.right = !moveLeft;
        break;

    case BotState::ATTACK:
        if (distance <= IDEAL_ATTACK_DISTANCE)
        {
            if (rand() % 2 == 0)
            {
                player2Input.kick = true;
            }
            else
            {
                player2Input.punch = true;
            }
        }
        else
        {
            currentState = BotState::CHASE;
        }
        break;

    case BotState::RETREAT:
        player2Input.right = moveLeft;
        player2Input.left = !moveLeft;
        break;

    case BotState::IDLE:
        break;

    case BotState::PARRY:
        player2Input.parry = true;
        break;
    }

    // Emergency chase if player gets too far away
    if (distance > TOO_FAR_DISTANCE && state.currentState != PlayerState::ATTACKING)
    {
        player2Input.left = moveLeft;
        player2Input.right = !moveLeft;
        currentState = BotState::CHASE;
    }

    // Update state machine based on inputs
    if (player2Input.left || player2Input.right)
        stateMachine.transition(player2, PlayerState::WALKING);
    if (player2Input.punch)
        stateMachine.transition(player2, PlayerState::ATTACKING);
    if (player2Input.kick)
        stateMachine.transition(player2, PlayerState::KICKING);
    if (player2Input.parry)
        stateMachine.transition(player2, PlayerState::PARRYING);

    // Update motion based on inputs
    if (player2Input.left)
        player2Motion.velocity.x = -MOVE_SPEED;
    else if (player2Input.right)
        player2Motion.velocity.x = MOVE_SPEED;
    else
        player2Motion.velocity.x = 0;
}
