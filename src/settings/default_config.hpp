#pragma once
#include <GLFW/glfw3.h>
#include "settings.hpp"

namespace DefaultConfig
{
    // Player 1 default controls (WASD + RTG)
    const PlayerControls P1_CONTROLS = {
        GLFW_KEY_S, // Down
        GLFW_KEY_A, // Left
        GLFW_KEY_D, // Right
        GLFW_KEY_R, // Punch
        GLFW_KEY_G, // Kick (deprecated, not bound to actions anymore) 
        GLFW_KEY_T  // Parry
    };

    // Player 2 default controls (Arrow keys + .,M)
    const PlayerControls P2_CONTROLS = {
        GLFW_KEY_DOWN,   // Down
        GLFW_KEY_LEFT,   // Left
        GLFW_KEY_RIGHT,  // Right
        GLFW_KEY_COMMA,  // Punch
        GLFW_KEY_PERIOD, // Kick (deprecated, not bound to actions anymore) 
        GLFW_KEY_M       // Parry
    };

    // Default window settings
    const WindowSettings WINDOW_SETTINGS = {
        "Windowed",  // window_mode
        {1024, 768}, // resolution (width, height)
        false,       // show_fps
        false        // enable_debug
    };

    // Default audio settings
    const AudioSettings AUDIO_SETTINGS = {
        true, // enable_sound_effects
        true, // enable_music
        1.0f, // overall_volume (100%)
        0.75f // music_volume (75%)
    };
}