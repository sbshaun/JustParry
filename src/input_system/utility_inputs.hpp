#pragma once
#include <GLFW/glfw3.h>
#include "../renderer.hpp"
#include "../fps/fps.hpp"
#include "../window.hpp"

// Forward declarations
class WorldSystem;

// Utility key definitions
struct UtilityKeys
{
    static constexpr int DEBUG_TOGGLE = GLFW_KEY_H; // Toggle debug visualization
    static constexpr int FPS_TOGGLE = GLFW_KEY_F;   // Toggle FPS display
    static constexpr int BOT_TOGGLE = GLFW_KEY_B;   // Toggle bot mode
    static constexpr int EXIT = GLFW_KEY_ESCAPE;    // Exit game
    static constexpr int CONFIRM = GLFW_KEY_ENTER;  // Confirm/Restart
};

// Utility input handling functions
inline void handleUtilityInputs(GlRender &renderer, bool &showFPS, bool &botEnabled,
                                bool &fKeyPressed, bool &bKeyPressed, bool &hKeyPressed,
                                GLWindow &glWindow, FPSCounter &fpsCounter, bool &shouldExit,
                                WorldSystem &worldSystem)
{
    // Debug mode toggle
    if (glfwGetKey(glWindow.window, UtilityKeys::DEBUG_TOGGLE) == GLFW_PRESS)
    {
        if (!hKeyPressed)
        {
            renderer.debugMode = !renderer.debugMode;
            hKeyPressed = true;
            std::cout << "Debug mode " << (renderer.debugMode ? "enabled" : "disabled") << std::endl;
        }
    }
    else
    {
        hKeyPressed = false;
    }

    // FPS display toggle
    if (glfwGetKey(glWindow.window, UtilityKeys::FPS_TOGGLE) == GLFW_PRESS)
    {
        if (!fKeyPressed)
        {
            showFPS = !showFPS;
            fKeyPressed = true;
            std::cout << "FPS display " << (showFPS ? "enabled" : "disabled") << std::endl;
        }
    }
    else
    {
        fKeyPressed = false;
    }
    fpsCounter.update(renderer, showFPS);

    // Bot mode toggle
    if (glfwGetKey(glWindow.window, UtilityKeys::BOT_TOGGLE) == GLFW_PRESS)
    {
        if (!bKeyPressed)
        {
            botEnabled = !botEnabled;
            worldSystem.botEnabled = botEnabled;
            bKeyPressed = true;
            std::cout << "Bot mode " << (botEnabled ? "enabled" : "disabled") << std::endl;
        }
    }
    else
    {
        bKeyPressed = false;
    }

    // Exit game
    if (glfwGetKey(glWindow.window, UtilityKeys::EXIT) == GLFW_PRESS)
    {
        shouldExit = true;
    }
}