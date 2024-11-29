#include "bot.hpp"
#include <iostream>
#include <functional>
#include <memory>
#include <cstdlib>

// Helper function to calculate utility for different actions
float calculateUtility(
    BotState state, float distance, PlayerState currentState, float health, 
    float opponentHealth, float postureBar, int currentLevel)
{
    float utility = 0.0f;

    switch (state)
    {
    case BotState::CHASE:
        utility = (distance > IDEAL_ATTACK_DISTANCE) ? 10.0f + currentLevel : -5.0f;
        if (currentState == PlayerState::ATTACKING) {
            utility -= 10.0f;
        }
        break;
    case BotState::ATTACK:
        if (distance <= IDEAL_ATTACK_DISTANCE) {
			utility = 20.0f + currentLevel + (currentState == PlayerState::STUNNED ? 15.0f : 0.0f);
        }
        break;
    case BotState::RETREAT:
        utility = (distance < TOO_CLOSE_DISTANCE || 
            (distance > IDEAL_ATTACK_DISTANCE && currentState == PlayerState::ATTACKING)) 
            ? 21.0f : -5.0f;
        break;
    case BotState::IDLE:
        utility = 5.0f;
        break;
    case BotState::PARRY:
        if (distance <= IDEAL_ATTACK_DISTANCE && postureBar > 0 && parryCounter <= 0)
        {
			float parryMultiplier = (currentLevel <= 2) ? 0.5f : (currentLevel >= 4) ? 1.5f : 1.0f;
            utility = (17.0f + (postureBar * 0.5f) + 
                        ((currentState == PlayerState::ATTACKING) ? 5.0f : -1.0f)
                      ) * parryMultiplier;
        }
        break;
    }

    return utility;
}

// Helper function to pick next action based on utilities
BotState pickNextAction(
    float distance, PlayerState currentState, float health, 
    float opponentHealth, float postureBar, int currentLevel
)
{
    std::vector<std::pair<BotState, float>> actions = {
        {BotState::CHASE, calculateUtility(BotState::CHASE, distance, currentState, health, opponentHealth, postureBar, currentLevel)},
        {BotState::ATTACK, calculateUtility(BotState::ATTACK, distance, currentState, health, opponentHealth, postureBar, currentLevel)},
        {BotState::RETREAT, calculateUtility(BotState::RETREAT, distance, currentState, health, opponentHealth, postureBar, currentLevel)},
        {BotState::IDLE, calculateUtility(BotState::IDLE, distance, currentState, health, opponentHealth, postureBar, currentLevel)},
        {BotState::PARRY, calculateUtility(BotState::PARRY, distance, currentState, health, opponentHealth, postureBar, currentLevel)}
    };

    // Calculate the total utility
    float totalUtility = 0.0f;
    for (const auto& action : actions)
    {
        totalUtility += action.second;
    }

    // Generate a random number between 0 and totalUtility
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, totalUtility);

    float randomValue = dis(gen);

    // Select an action based on the random value
    float cumulativeUtility = 0.0f;
    for (const auto& action : actions)
    {
        cumulativeUtility += action.second;
        if (randomValue <= cumulativeUtility)
        {
            return action.first;
        }
    }

    // Fallback to the action with the highest utility (shouldn't reach here)
    return actions.front().first;
}

void Bot::pollBotRng(GlRender &renderer, StateMachine &stateMachine, int currentLevel)
{
    Entity player2 = renderer.m_player2;
    Entity player1 = renderer.m_player1;

    PlayerInput &player2Input = registry.playerInputs.get(player2);
    Motion &player2Motion = registry.motions.get(player2);
    Motion &player1Motion = registry.motions.get(player1);
    PlayerCurrentState &state = registry.playerCurrentStates.get(player2);
	PlayerCurrentState& player1State = registry.playerCurrentStates.get(player1);

    Health &player2Health = registry.healths.get(player2);
    PostureBar& player2Posture = registry.postureBars.get(player2);
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
	parryCounter--;

    // Pick new action if counter expires
    if (actionCounter <= 0)
    {
        currentState = pickNextAction(
            distance, player1State.currentState, player2Health.currentHealth, 
            player1Health.currentHealth, player2Posture.currentBar,
            currentLevel
        );

        // Set appropriate duration for each action
        switch (currentState)
        {
        case BotState::ATTACK:
            actionCounter = 50;
            break;
        case BotState::CHASE:
        case BotState::RETREAT:
            actionCounter = 10;
            break;
        case BotState::IDLE:
            actionCounter = 45;
            break;
        case BotState::PARRY:
            actionCounter = 40;
			parryCounter = BOT_PARRY_COOLDOWN;
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
