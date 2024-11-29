#pragma once
#include "actions.hpp"
#include <unordered_map> 
#include <unordered_set>
#include <iostream>
#include "input_handler.hpp"

class ControllerHandler : public InputHandler {
    public:
    ControllerHandler(std::unique_ptr<ControllerMapping> controllerMapping) : InputHandler(nullptr, std::move(controllerMapping)) {}


    void handleInput(Entity entity, StateMachine &state_machine) override 
    {
        // int size;
        // std::cout << glfwGetJoystickButtons(cid, &size) << std::endl;
        Motion &motion = registry.motions.get(entity);
        bool moving = false;
        int cid = registry.players.get(entity).controller_id;

        for (const auto &pair : controllerMapping->getKeyToActionMap())
        {
            // check if any key is pressed
            if (isControllerKeyPressed(cid, pair.first))
            {
                // get the corresponded action from the key
                Action action = pair.second;

                bool actionInBuffer = false;
                for (int i = 0; i < actionBuffer.size(); i++)
                {
                    if (actionBuffer[i].action == action)
                    {
                        actionInBuffer = true;
                    }

                    if (actionBuffer[i].action == Action::MOVE_RIGHT)
                    {
                        if (action == Action::MOVE_RIGHT)
                        {
                            actionBuffer[i].ttl = 300.f;
                            break;
                        }
                        else
                        {
                            actionBuffer.erase(actionBuffer.begin() + i);
                            break;
                        }
                    }
                    if (actionBuffer[i].action == Action::MOVE_LEFT)
                    {
                        if (action == Action::MOVE_LEFT)
                        {
                            actionBuffer[i].ttl = 300.f;
                            break;
                        }
                        else
                        {
                            actionBuffer.erase(actionBuffer.begin() + i);
                            break;
                        }
                    }
                }
                if (!actionBuffer.empty() && actionInBuffer)
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

};