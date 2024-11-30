#pragma once
#include "actions.hpp"
#include <unordered_map> 
#include <unordered_set>
#include <iostream>

/*
1. each player has an instance of this class, mapping key to action, 
2. player can change the key binding. 
 //Keys are respresented as the index of the button when glfwGetJoystickButtons is called (needs to dynamically defined)
*/
class ControllerMapping : InputMapping {
    private:
        std::unordered_map<int, Action> keyToActionMap; // key to action mapping 
        std::unordered_map<Action, int> actionToKeyMap; // action to key mapping, used for easy reverse lookup 

    public:
        int CONTROLLER_ID;
        ControllerMapping(int id) { //WHENEVER A MAPPING IS CREATED PASS IN THE CONTROLLER ID OF THIS PLAYER
            // Initialize key mapping
            CONTROLLER_ID = id;
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

        Action getActionFromKey(int key) const {
            if (keyToActionMap.empty()) {
                // error 
                std::cerr << "Key mapping is empty, please bind keys first" << std::endl; 
            }

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
        std::unordered_map<Action, int> getActionToKeyMap() const { return actionToKeyMap; } // may be handy when doing: if (isKeyPressed(getKeyFromAction(Action::MOVE_LEFT))) { // do something } 
};