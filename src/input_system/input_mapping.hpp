#pragma once
#include "actions.hpp"
#include <unordered_map> 
#include <unordered_set>
#include <iostream>

/*
1. each player has an instance of this class, mapping key to action, 
2. player can change the key binding. 
*/
class InputMapping {
    private:
        std::unordered_map<int, Action> keyToActionMap; // key to action mapping 
        std::unordered_map<Action, int> actionToKeyMap; // action to key mapping, used for easy reverse lookup 

    public:
        InputMapping() {
                // Initialize key mapping
            if (keyToActionMap.empty()) {
                // error 
                std::cerr << "Key mapping is empty, please bind keys first" << std::endl; 
            }
        };

        void bindKeyToAction(int key, Action action) {
            // make sure the key is not bound to other actions 
            if (keyToActionMap.find(key) != keyToActionMap.end()) {
                std::cerr << "Key: " << key << " already binds to action: " << ActionToString(keyToActionMap.at(key)) << std::endl;
                return;
            }
            keyToActionMap[key] = action;
            actionToKeyMap[action] = key;
        }; 

        void initDefaultMapping() {
            // player 1
            bindKeyToAction(GLFW_KEY_A, Action::MOVE_LEFT);
            bindKeyToAction(GLFW_KEY_D, Action::MOVE_RIGHT);
            bindKeyToAction(GLFW_KEY_W, Action::JUMP);
            bindKeyToAction(GLFW_KEY_S, Action::CROUCH);
            bindKeyToAction(GLFW_KEY_SPACE, Action::PUNCH);
            bindKeyToAction(GLFW_KEY_LEFT_SHIFT, Action::KICK);

            // player 2
            bindKeyToAction(GLFW_KEY_LEFT, Action::MOVE_LEFT);
            bindKeyToAction(GLFW_KEY_RIGHT, Action::MOVE_RIGHT);
            bindKeyToAction(GLFW_KEY_UP, Action::JUMP);
            bindKeyToAction(GLFW_KEY_DOWN, Action::CROUCH);
            bindKeyToAction(GLFW_KEY_COMMA, Action::PUNCH);
            bindKeyToAction(GLFW_KEY_PERIOD, Action::KICK);
        };

        Action getActionFromKey(int key) const {
            if (keyToActionMap.find(key) != keyToActionMap.end()) {
                return keyToActionMap.at(key);
            }
            return Action::NONE;
        }; 

        int getKeyFromAction(Action action) const {
            if (actionToKeyMap.find(action) != actionToKeyMap.end()) {
                return actionToKeyMap.at(action);
            }
            return -1;
        };

        void printBindings() const {
            // print current key bindings
            for (const auto &pair : keyToActionMap) {
                std::cout << "Key: " << pair.first << " is bound to action: " << ActionToString(pair.second) << std::endl;
            }
        }; 

        std::unordered_map<int, Action> getKeyToActionMap() const { return keyToActionMap; }
        std::unordered_map<Action, int> getActionToKeyMap() const { return actionToKeyMap; }
};