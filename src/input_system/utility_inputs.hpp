#pragma once
#include <GLFW/glfw3.h>
#include "../renderer.hpp"
#include "../fps/fps.hpp"
#include "../window.hpp"
#include "../common.hpp"

// Forward declarations
class WorldSystem;

// Utility key definitions
struct UtilityKeys
{
    static constexpr int DEBUG_TOGGLE = GLFW_KEY_H; // Toggle debug visualization
    static constexpr int FPS_TOGGLE = GLFW_KEY_F;   // Toggle FPS display
    // static constexpr int EXIT = SDLK_ESCAPE;        // Exit game
    static constexpr int PAUSE = GLFW_KEY_ESCAPE;   // Toggle FPS display
    static constexpr int CONFIRM = GLFW_KEY_ENTER;  // Confirm/Restart
};

// Utility input handling functions
inline void handleUtilityInputs(GlRender &renderer, bool &showFPS, 
                                bool &fKeyPressed, bool &bKeyPressed, bool &hKeyPressed,
                                GLWindow &glWindow, FPSCounter &fpsCounter, bool &shouldExit,
                                WorldSystem &worldSystem)
{
    // Debug mode toggle
    if (isKeyPressed(UtilityKeys::DEBUG_TOGGLE))
    {
        if (!hKeyPressed)
        {
            Settings::windowSettings.enable_debug = !Settings::windowSettings.enable_debug;
            renderer.debugMode = Settings::windowSettings.enable_debug;
            Settings::saveSettings();
            hKeyPressed = true;
            std::cout << "Debug mode " << (renderer.debugMode ? "enabled" : "disabled") << std::endl;
        }
    }
    else
    {
        hKeyPressed = false;
    }

    // FPS display toggle
    if (isKeyPressed(UtilityKeys::FPS_TOGGLE))
    {
        if (!fKeyPressed)
        {
            Settings::windowSettings.show_fps = !Settings::windowSettings.show_fps;
            showFPS = Settings::windowSettings.show_fps;
            Settings::saveSettings();
            fKeyPressed = true;
            std::cout << "FPS display " << (showFPS ? "enabled" : "disabled") << std::endl;
        }
    }
    else
    {
        fKeyPressed = false;
    }
    fpsCounter.update(renderer, Settings::windowSettings.show_fps);

    if(isKeyPressed(UtilityKeys::PAUSE)){
        Game* game = renderer.getGameInstance();
        game->attemptPause();
    }
    // Bot mode toggle
    // if (isKeyPressed(UtilityKeys::BOT_TOGGLE))
    // {
    //     if (!bKeyPressed)
    //     {
    //         // Settings::windowSettings.enable_bot = !Settings::windowSettings.enable_bot;
    //         botEnabled = false;
    //         worldSystem.botEnabled = botEnabled;
    //         Settings::saveSettings();
    //         bKeyPressed = true;
    //         std::cout << "Bot mode " << (botEnabled ? "enabled" : "disabled") << std::endl;
    //     }
    // }
    // else
    // {
    //     bKeyPressed = false;
    // }

    // Exit game
    // if (isKeyPressed(UtilityKeys::EXIT))
    // {
    //     shouldExit = true;
    // }
}