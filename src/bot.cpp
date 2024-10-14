#include "bot.hpp"
#include <iostream>

enum BotState {
    JUMPING,
    MOVING_LEFT,
    MOVING_RIGHT,
    STAND
};

static int actionCounter = 0;
static int cooldown = 0;
static BotState currentState = JUMPING;

BotState randNextState(){
    int rng = rand() % 4;
    switch (rng){
        case 0:
            return JUMPING;
        case 1:
            return MOVING_RIGHT;
        case 2:
            return MOVING_LEFT;
        case 3:
        default: 
            return STAND;
    }
}

void Bot::pollBotRng(GlRender& renderer) {
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
            //randomly choose a state and a duration
            actionCounter = 10 + rand() % 60;
            currentState = randNextState();
            std::cout << "Bot will "<< currentState <<" next" << std::endl;
        }
        break;

    case MOVING_LEFT:
        player2Input.left = true;  // Move left
        actionCounter--;

        if (actionCounter == 0) {
            actionCounter = 10 + rand() % 60;
            currentState = randNextState();
            std::cout << "Bot will "<< currentState <<" next" << std::endl;
        }
        break;

    case MOVING_RIGHT:
        player2Input.right = true;  // Move right
        actionCounter--;

        if (actionCounter == 0) {
            actionCounter = 10 + rand() % 60;
            currentState = randNextState(); 
            std::cout << "Bot will "<< currentState <<" next" << std::endl;
        }
        break;
    case STAND:
        player2Input.right = false;
        player2Input.left = false;
        player2Input.up = false;
        actionCounter--;
        if (actionCounter == 0) {
            actionCounter = 10 + rand() % 60;
            currentState = randNextState(); 
            std::cout << "Bot will "<< currentState <<" next" << std::endl;
        }
    }
}

