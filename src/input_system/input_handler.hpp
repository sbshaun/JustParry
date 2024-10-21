#pragma once
#include <unordered_map>
#include "command.hpp"
#include "input_utils.hpp"
#include "input_mapping.hpp"
#include "state_machine.hpp"

class InputHandler {
    public: 

        InputHandler(InputMapping* inputMapping) : inputMapping(inputMapping) {}

        void bindActionToCommand(Action action, Command* command) {
            ActionToCommandMapping[action] = std::move(command);
        }

        void initDefaultActionToCommandMapping() {
            bindActionToCommand(Action::MOVE_LEFT, new MoveLeftCommand());
            bindActionToCommand(Action::MOVE_RIGHT, new MoveRightCommand());
            bindActionToCommand(Action::JUMP, new JumpCommand());
            bindActionToCommand(Action::PUNCH, new PunchCommand());
            bindActionToCommand(Action::KICK, new KickCommand());
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
        std::unordered_map<Action, Command*> ActionToCommandMapping; 
};