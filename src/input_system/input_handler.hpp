#pragma once
#include <unordered_map>
#include "command.hpp"
#include "input_utils.hpp"
#include "input_mapping.hpp"
#include "state_machine.hpp"
#include "../ecs/components.hpp"

class InputHandler {
    public: 

        InputHandler(std::unique_ptr<InputMapping> inputMapping) : inputMapping(std::move(inputMapping)) {}

        void bindActionToCommand(Action action, std::unique_ptr<Command> command) {
            actionToCommandMapping[action] = std::move(command);
        }

        void initDefaultActionToCommandMapping() {
            bindActionToCommand(Action::MOVE_LEFT, std::make_unique<MoveLeftCommand>());
            bindActionToCommand(Action::MOVE_RIGHT, std::make_unique<MoveRightCommand>());
            bindActionToCommand(Action::JUMP, std::make_unique<JumpCommand>());
            bindActionToCommand(Action::CROUCH, std::make_unique<CrouchCommand>());
            bindActionToCommand(Action::PUNCH, std::make_unique<PunchCommand>());
            bindActionToCommand(Action::KICK, std::make_unique<KickCommand>());
        }

        /*
        * check if a key is pressed, execute the respective command for the key 
        */
       //CHANGE THIS TO CHECK IF ITS A SYSTEM BUTTON OR A FIGHT INPUT
       //IF A FIGHT INPUT QUEUE IT FOR RESOLUTION
       //NEED A WAY TO DETERMINE P1 OR P2 INPUTS OR ACCEPT SOME AMOUNT OF EACH PER LOGIC LOOP
       //Deal with predetermined jump arc by allowing you to hold up but change directions on landing
        void handleInput(Entity entity, StateMachine& state_machine) {
            Motion& motion = registry.motions.get(entity);
            bool moving = false;

            for (const auto& pair : inputMapping->getKeyToActionMap()) {
                // check if any key is pressed 
                if (isKeyPressed(pair.first)) {
                    // get the corresponded action from the key 
                    Action action = pair.second;
                    // execute the command for the action. 
                    if (actionToCommandMapping.find(action) != actionToCommandMapping.end()) {
                        actionToCommandMapping[action]->execute(entity, state_machine);
                    }

                    if (action == Action::MOVE_LEFT || action == Action::MOVE_RIGHT) {
                        moving = true;
                    }
                }
            }

            if (!moving) {
                motion.velocity.x = 0.0f;
                if (state_machine.transition(entity, PlayerState::IDLE)) {
                    PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
                    // std::cout << "playerState is now " << PlayerStateToString(playerState.currentState) << std::endl;
                    // std::cout << "Player is now IDLE" << std::endl;
                }
            }
        }

        Action getActionFromKey(int key) const {
            return inputMapping->getActionFromKey(key);
        }

        int getKeyFromAction(Action action) const {
            return inputMapping->getKeyFromAction(action);
        }

    private: 
        std::unique_ptr<InputMapping> inputMapping;
        std::unordered_map<Action, std::unique_ptr<Command>> actionToCommandMapping; 
        // unique_ptr: https://www.geeksforgeeks.org/unique_ptr-in-cpp/ 
};