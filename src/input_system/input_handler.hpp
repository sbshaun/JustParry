#pragma once
#include <unordered_map>
#include "command.hpp"
#include "input_utils.hpp"
#include "input_mapping.hpp"
#include "state_machine.hpp"

class InputHandler {
    public: 

        InputHandler(InputMapping* inputMapping) : inputMapping(inputMapping) {}

        void bindActionToCommand(Action action, std::unique_ptr<Command> command) {
            ActionToCommandMapping[action] = std::move(command);
        }

        void initDefaultActionToCommandMapping() {
            bindActionToCommand(Action::MOVE_LEFT, std::make_unique<MoveLeftCommand>());
            bindActionToCommand(Action::MOVE_RIGHT, std::make_unique<MoveRightCommand>());
            bindActionToCommand(Action::JUMP, std::make_unique<JumpCommand>());
            bindActionToCommand(Action::PUNCH, std::make_unique<PunchCommand>());
            bindActionToCommand(Action::KICK, std::make_unique<KickCommand>());
        }

        /*
        if the key is pressed, execute the respective command 
        */
        void handleInput(Entity entity, StateMachine& state_machine) {
            for (const auto& pair : inputMapping->getKeyToActionMap()) {
                // check if any key is pressed 
                if (isKeyPressed(pair.first)) {
                    // get the corresponded action from the key 
                    Action action = pair.second;
                    // execute the command for the action. 
                    if (ActionToCommandMapping.find(action) != ActionToCommandMapping.end()) {
                        ActionToCommandMapping[action]->execute(entity, state_machine);
                    }
                }
            }
        }

    private: 
        InputMapping* inputMapping;
        std::unordered_map<Action, std::unique_ptr<Command>> ActionToCommandMapping; 
        // unique_ptr: https://www.geeksforgeeks.org/unique_ptr-in-cpp/ 
};