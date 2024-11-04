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
            bindActionToCommand(Action::PARRY, std::make_unique<ParryCommand>());
        }

        // helper function to print actionBuffer, e.g. actionBuffer size n: [MOVE_LEFT, JUMP, PUNCH] 
        void printActionBufferHelper() {
            int n = actionBuffer.size();
            std::string actionList = "ActionBuffer size " + std::to_string(n) + ": [";
            for (const auto& item : actionBuffer) {
                actionList += ActionToString(item.action);
                actionList += ", ";
            }
            actionList += "]";
            std::cout << "actionBuffer: " << actionList << std::endl;
        }

        void processActionBuffer(Entity entity, StateMachine& state_machine) {
            for (int i = 0; i < actionBuffer.size(); i++) {
                actionBuffer[i].ttl -= PLAYER_STATE_TIMER_STEP;
                if (actionBuffer[i].ttl <= 0) {
                    actionBuffer.erase(actionBuffer.begin() + i);
                }
            }

            Motion& motion = registry.motions.get(entity);
            bool moving = false;

            for (int i = 0; i < actionBuffer.size(); i++) {
                Action action = actionBuffer[i].action;
                
                if (actionToCommandMapping.find(action) != actionToCommandMapping.end()) {
                    actionToCommandMapping[action]->execute(entity, state_machine);
                }
                if (action == Action::MOVE_LEFT || action == Action::MOVE_RIGHT) {
                    moving = true;
                }
            }

            if (!moving && !motion.inAir) {
                motion.velocity.x = 0.0f;
                if (state_machine.transition(entity, PlayerState::IDLE)) {
                    PlayerCurrentState& playerState = registry.playerCurrentStates.get(entity);
                }
            }
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

                    if (!actionBuffer.empty() && actionBuffer.back().action == action) continue;
                    if (actionBuffer.size() >= MAX_BUFFER_SIZE) continue;

                    if (!shouldAddActionToBuffer(action)) continue;
                    actionBuffer.push_back({action, TTL});
                }
            }

            // loop to see key release action 
            for (const auto& pair : inputMapping->getKeyToActionMap()) {
                if (isKeyReleased(pair.first)) {
                    Action action = pair.second;
                    if (action == Action::PUNCH) {
                        punchReleased = true;
                    }
                    if (action == Action::KICK) {
                        kickReleased = true;
                    }
                    if (action == Action::PARRY) {
                        parryReleased = true;
                    }
                }
            }

            // chek release key 

            processActionBuffer(entity, state_machine);
        }

        // functioin to determine if a key should be added: only add one action per press
        bool shouldAddActionToBuffer(Action action) {
            if (action == Action::PUNCH) {
                if (!punchReleased) {
                    // if punch is not released, return false 
                    return false;
                } else {
                    // if punch is released, return true and set punchReleased to false
                    punchReleased = false;
                    return true;
                }
            }
            if (action == Action::KICK) {
                if (!kickReleased) {
                    return false;
                } else {
                    kickReleased = false;
                    return true;
                }
            }
            if (action == Action::PARRY) {
                if (!parryReleased) {
                    return false;
                } else {
                    parryReleased = false;
                    return true;
                }
            }
            return true;
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
        struct actionBufferItem {
            Action action;
            float ttl; // time to live 
        };
        const float TTL = 300.f; // 100ms 
        const int MAX_BUFFER_SIZE = 10; // max number of buffered actions 
        std::vector<actionBufferItem> actionBuffer;

        bool punchReleased = true; // one action per press, next action requires to release the key first 
        bool kickReleased = true;
        bool parryReleased = true;
};