#pragma once

/*
Used for kep mapping: key -> action. 
*/
enum class Action {
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    CROUCH,
    PUNCH, 
    KICK,
    NONE // default do nothing  
};

constexpr const char* ActionToString(Action action) {
    switch (action) {
        case Action::MOVE_LEFT: return "MOVE_LEFT";
        case Action::MOVE_RIGHT: return "MOVE_RIGHT";
        case Action::JUMP: return "JUMP";
        case Action::CROUCH: return "CROUCH";
        case Action::PUNCH: return "PUNCH";
        case Action::KICK: return "KICK";
        case Action::NONE: return "NONE";
        default: return "UNKNOWN";
    }
}
