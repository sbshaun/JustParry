#include "bot.hpp"
#include <iostream>

enum BotState {
    JUMPING,
    MOVING_LEFT,
    MOVING_RIGHT
};

static int actionCounter = 0;
static BotState currentState = JUMPING;

void Bot::pollBotRng(int timer, GlRender& renderer) {
    PlayerInput& player2Input = registry.playerInputs.get(renderer.m_player2);
    Motion& player2Motion = registry.motions.get(renderer.m_player2);

    // Reset inputs each frame
    player2Input = PlayerInput();

    // If the bot is in the air, skip movement logic
    if (player2Motion.inAir) {
        return;  
    }

    // Bot action state machine
    switch (currentState) {
    case JUMPING:
        if (actionCounter == 0 || actionCounter == 60) {
            player2Input.up = true;  // Trigger jump
            actionCounter = 60;  // Set duration for the jump state (1 second for the jump)
            std::cout << "Bot is jumping" << std::endl;
        }
        actionCounter--;

        if (actionCounter == 0) {
            // Transition to moving left after jumping
            currentState = MOVING_LEFT;
            actionCounter = 20;  // Move left for 0.3 seconds
            std::cout << "Bot will move left next" << std::endl;
        }
        break;

    case MOVING_LEFT:
        player2Input.left = true;  // Move left
        actionCounter--;

        if (actionCounter == 0) {
            // Transition to moving right after moving left
            currentState = MOVING_RIGHT;
            actionCounter = 20;  // Move right for 0.3 seconds
            std::cout << "Bot will move right next" << std::endl;
        }
        break;

    case MOVING_RIGHT:
        player2Input.right = true;  // Move right
        actionCounter--;

        if (actionCounter == 0) {
            // After moving right, transition back to jumping
            currentState = JUMPING;
            actionCounter = 90;  // Prepare for the next jump after 1.5 second
            std::cout << "Bot will jump again" << std::endl;
        }
        break;
    }
}
