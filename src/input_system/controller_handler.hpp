#pragma once
#include "actions.hpp"
#include <unordered_map> 
#include <unordered_set>
#include <iostream>
#include "input_handler.hpp"

class ControllerHandler : public InputHandler {
    public:
    public:
    ControllerHandler(std::unique_ptr<ControllerMapping> controllerMapping) : InputHandler(nullptr, std::move(controllerMapping)) {}


    void handleInput(Entity entity, StateMachine &state_machine, int cid) //THIS CAUSES A CRASH 
    {
        // int size;
        // std::cout << glfwGetJoystickButtons(cid, &size) << std::endl;
        Motion &motion = registry.motions.get(entity);
        bool moving = false;
        // int size;

        for (const auto &pair : controllerMapping->getKeyToActionMap())
        {
            // check if any key is pressed
            if (isControllerKeyPressed(cid, pair.first))
            {
                // get the corresponded action from the key
                Action action = pair.second;

                if (!actionBuffer.empty() && actionBuffer.back().action == action)
                    continue;
                if (actionBuffer.size() >= MAX_BUFFER_SIZE)
                    continue;

                if (!shouldAddActionToBuffer(entity, action))
                    continue;
                actionBuffer.push_back({action, TTL});
            }
        }

        // loop to see key release action
        for (const auto &pair : controllerMapping->getKeyToActionMap())
        {
            if (isControllerKeyReleased(cid, pair.first))
            {
                Action action = pair.second;
                if (action == Action::PUNCH)
                {
                    punchReleased = true;
                }
                if (action == Action::KICK)
                {
                    kickReleased = true;
                }
                if (action == Action::PARRY)
                {
                    parryReleased = true;
                }
                if (action == Action::CROUCH)
                {
                    crouchReleased = true;
                }
            }
        }

        // chek release key

        processActionBuffer(entity, state_machine);
    }

    private:
    std::unique_ptr<ControllerMapping> controllerMapping;
};