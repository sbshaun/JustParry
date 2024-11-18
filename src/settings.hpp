#pragma once
#include <GLFW/glfw3.h>
#include <string>

// Control scheme structure
struct PlayerControls
{
    int up;
    int down;
    int left;
    int right;
    int punch;
    int kick;
    int parry;
};

// Add resolution structure
struct Resolution
{
    int width;
    int height;
};

// Add window settings structure
struct WindowSettings
{
    std::string window_mode; // "Windowed", "Fullscreen", "Borderless"
    Resolution resolution;   // Current resolution
    bool show_fps;
    bool enable_bot;
    bool enable_debug;
};

// Add audio settings structure
struct AudioSettings
{
    bool enable_sound_effects;
    bool enable_music;
    float overall_volume; // 0.0f to 1.0f
    float music_volume;   // 0.0f to 1.0f
};

namespace Settings
{
    // Current control settings
    extern PlayerControls p1Controls;
    extern PlayerControls p2Controls;
    extern WindowSettings windowSettings;
    extern AudioSettings audioSettings; // Add audio settings

    // Helper functions
    const char *getKeyName(int key);
    int getKeyFromName(const std::string &keyName);
    void saveSettings();
    void loadSettings();
    void resetToDefaults();

    // Add new function to check if current settings match defaults
    bool areSettingsDefault();
}