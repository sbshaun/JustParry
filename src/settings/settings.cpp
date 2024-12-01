#include "settings.hpp"
#include "default_config.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring> //needed for linux
#include "../external/project_path.hpp"
// #include <direct.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <errno.h>
#define MKDIR(dir) mkdir(dir, 0777)
#endif

namespace Settings
{
    // Initialize current controls with defaults
    PlayerControls p1Controls = DefaultConfig::P1_CONTROLS;
    PlayerControls p2Controls = DefaultConfig::P2_CONTROLS;
    WindowSettings windowSettings = DefaultConfig::WINDOW_SETTINGS;
    AudioSettings audioSettings = DefaultConfig::AUDIO_SETTINGS;

    // Helper function to get config file path
    std::string getConfigPath()
    {
        return std::string(PROJECT_SOURCE_DIR) + "/config/controls.txt";
    }

    // Helper function to ensure config directory exists
    void ensureConfigDirectory()
    {
        std::string configDir = std::string(PROJECT_SOURCE_DIR) + "/config";
        MKDIR(configDir.c_str());
    }

    void resetToDefaults()
    {
        // Just reset the controls in memory
        p1Controls = DefaultConfig::P1_CONTROLS;
        p2Controls = DefaultConfig::P2_CONTROLS;
        windowSettings = DefaultConfig::WINDOW_SETTINGS;
        audioSettings = DefaultConfig::AUDIO_SETTINGS;

        // Save once after resetting
        saveSettings();
        std::cout << "Controls reset to defaults" << std::endl;
    }

    void saveSettings()
    {
        ensureConfigDirectory();
        std::ofstream file(getConfigPath());
        if (file.is_open())
        {
            // Save P1 controls
            file << "[Player1Controls]\n";
            file << "up=" << getKeyName(p1Controls.up) << "\n";
            file << "down=" << getKeyName(p1Controls.down) << "\n";
            file << "left=" << getKeyName(p1Controls.left) << "\n";
            file << "right=" << getKeyName(p1Controls.right) << "\n";
            file << "punch=" << getKeyName(p1Controls.punch) << "\n";
            file << "parry=" << getKeyName(p1Controls.parry) << "\n\n";

            // Save P2 controls
            file << "[Player2Controls]\n";
            file << "up=" << getKeyName(p1Controls.up) << "\n";
            file << "down=" << getKeyName(p2Controls.down) << "\n";
            file << "left=" << getKeyName(p2Controls.left) << "\n";
            file << "right=" << getKeyName(p2Controls.right) << "\n";
            file << "punch=" << getKeyName(p2Controls.punch) << "\n";
            file << "parry=" << getKeyName(p2Controls.parry) << "\n";

            // Add window settings section
            file << "\n[WindowSettings]\n";
            file << "window_mode=" << windowSettings.window_mode << "\n";
            file << "resolution=" << windowSettings.resolution.width << "x" << windowSettings.resolution.height << "\n";
            file << "show_fps=" << (windowSettings.show_fps ? "true" : "false") << "\n";
            file << "enable_debug=" << (windowSettings.enable_debug ? "true" : "false") << "\n";

            // Add audio settings section
            file << "\n[AudioSettings]\n";
            file << "enable_sound_effects=" << (audioSettings.enable_sound_effects ? "true" : "false") << "\n";
            file << "enable_music=" << (audioSettings.enable_music ? "true" : "false") << "\n";
            file << "overall_volume=" << audioSettings.overall_volume << "\n";
            file << "music_volume=" << audioSettings.music_volume << "\n";

            file.close();
        }
        else
        {
            std::cerr << "Failed to save settings to " << getConfigPath() << std::endl;
        }
    }

    // Helper function to convert key name to GLFW key code
    int getKeyFromName(const std::string &keyName)
    {
        // Letters
        if (keyName == "A")
            return GLFW_KEY_A;
        if (keyName == "B")
            return GLFW_KEY_B;
        if (keyName == "C")
            return GLFW_KEY_C;
        if (keyName == "D")
            return GLFW_KEY_D;
        if (keyName == "E")
            return GLFW_KEY_E;
        if (keyName == "F")
            return GLFW_KEY_F;
        if (keyName == "G")
            return GLFW_KEY_G;
        if (keyName == "H")
            return GLFW_KEY_H;
        if (keyName == "I")
            return GLFW_KEY_I;
        if (keyName == "J")
            return GLFW_KEY_J;
        if (keyName == "K")
            return GLFW_KEY_K;
        if (keyName == "L")
            return GLFW_KEY_L;
        if (keyName == "M")
            return GLFW_KEY_M;
        if (keyName == "N")
            return GLFW_KEY_N;
        if (keyName == "O")
            return GLFW_KEY_O;
        if (keyName == "P")
            return GLFW_KEY_P;
        if (keyName == "Q")
            return GLFW_KEY_Q;
        if (keyName == "R")
            return GLFW_KEY_R;
        if (keyName == "S")
            return GLFW_KEY_S;
        if (keyName == "T")
            return GLFW_KEY_T;
        if (keyName == "U")
            return GLFW_KEY_U;
        if (keyName == "V")
            return GLFW_KEY_V;
        if (keyName == "W")
            return GLFW_KEY_W;
        if (keyName == "X")
            return GLFW_KEY_X;
        if (keyName == "Y")
            return GLFW_KEY_Y;
        if (keyName == "Z")
            return GLFW_KEY_Z;

        // Arrow keys
        if (keyName == "UP")
            return GLFW_KEY_UP;
        if (keyName == "DOWN")
            return GLFW_KEY_DOWN;
        if (keyName == "LEFT")
            return GLFW_KEY_LEFT;
        if (keyName == "RIGHT")
            return GLFW_KEY_RIGHT;

        // Punctuation
        if (keyName == ".")
            return GLFW_KEY_PERIOD;
        if (keyName == ",")
            return GLFW_KEY_COMMA;
        if (keyName == ";")
            return GLFW_KEY_SEMICOLON;
        if (keyName == "'")
            return GLFW_KEY_APOSTROPHE;
        if (keyName == "/")
            return GLFW_KEY_SLASH;
        if (keyName == "\\")
            return GLFW_KEY_BACKSLASH;
        if (keyName == "-")
            return GLFW_KEY_MINUS;
        if (keyName == "=")
            return GLFW_KEY_EQUAL;

        // Numbers
        if (keyName == "0")
            return GLFW_KEY_0;
        if (keyName == "1")
            return GLFW_KEY_1;
        if (keyName == "2")
            return GLFW_KEY_2;
        if (keyName == "3")
            return GLFW_KEY_3;
        if (keyName == "4")
            return GLFW_KEY_4;
        if (keyName == "5")
            return GLFW_KEY_5;
        if (keyName == "6")
            return GLFW_KEY_6;
        if (keyName == "7")
            return GLFW_KEY_7;
        if (keyName == "8")
            return GLFW_KEY_8;
        if (keyName == "9")
            return GLFW_KEY_9;

        // Numpad
        if (keyName == "NUM 0")
            return GLFW_KEY_KP_0;
        if (keyName == "NUM 1")
            return GLFW_KEY_KP_1;
        if (keyName == "NUM 2")
            return GLFW_KEY_KP_2;
        if (keyName == "NUM 3")
            return GLFW_KEY_KP_3;
        if (keyName == "NUM 4")
            return GLFW_KEY_KP_4;
        if (keyName == "NUM 5")
            return GLFW_KEY_KP_5;
        if (keyName == "NUM 6")
            return GLFW_KEY_KP_6;
        if (keyName == "NUM 7")
            return GLFW_KEY_KP_7;
        if (keyName == "NUM 8")
            return GLFW_KEY_KP_8;
        if (keyName == "NUM 9")
            return GLFW_KEY_KP_9;

        // Function keys
        if (keyName == "F1")
            return GLFW_KEY_F1;
        if (keyName == "F2")
            return GLFW_KEY_F2;
        if (keyName == "F3")
            return GLFW_KEY_F3;
        if (keyName == "F4")
            return GLFW_KEY_F4;
        if (keyName == "F5")
            return GLFW_KEY_F5;
        if (keyName == "F6")
            return GLFW_KEY_F6;
        if (keyName == "F7")
            return GLFW_KEY_F7;
        if (keyName == "F8")
            return GLFW_KEY_F8;
        if (keyName == "F9")
            return GLFW_KEY_F9;
        if (keyName == "F10")
            return GLFW_KEY_F10;
        if (keyName == "F11")
            return GLFW_KEY_F11;
        if (keyName == "F12")
            return GLFW_KEY_F12;

        // Special keys
        if (keyName == "SPACE")
            return GLFW_KEY_SPACE;
        if (keyName == "TAB")
            return GLFW_KEY_TAB;
        if (keyName == "ENTER")
            return GLFW_KEY_ENTER;
        if (keyName == "ESC")
            return GLFW_KEY_ESCAPE;
        if (keyName == "BACKSPACE")
            return GLFW_KEY_BACKSPACE;
        if (keyName == "DELETE")
            return GLFW_KEY_DELETE;
        if (keyName == "INSERT")
            return GLFW_KEY_INSERT;
        if (keyName == "HOME")
            return GLFW_KEY_HOME;
        if (keyName == "END")
            return GLFW_KEY_END;
        if (keyName == "PG UP")
            return GLFW_KEY_PAGE_UP;
        if (keyName == "PG DN")
            return GLFW_KEY_PAGE_DOWN;

        // Modifier keys
        if (keyName == "L SHIFT")
            return GLFW_KEY_LEFT_SHIFT;
        if (keyName == "R SHIFT")
            return GLFW_KEY_RIGHT_SHIFT;
        if (keyName == "L CTRL")
            return GLFW_KEY_LEFT_CONTROL;
        if (keyName == "R CTRL")
            return GLFW_KEY_RIGHT_CONTROL;
        if (keyName == "L ALT")
            return GLFW_KEY_LEFT_ALT;
        if (keyName == "R ALT")
            return GLFW_KEY_RIGHT_ALT;

        return GLFW_KEY_UNKNOWN;
    }

    void loadSettings()
    {
        std::ifstream file(getConfigPath());
        if (file.is_open())
        {
            std::string line;
            std::string section;

            while (std::getline(file, line))
            {
                if (line.empty())
                    continue;

                if (line[0] == '[')
                {
                    section = line;
                    continue;
                }

                size_t delimPos = line.find('=');
                if (delimPos != std::string::npos)
                {
                    std::string key = line.substr(0, delimPos);
                    std::string value = line.substr(delimPos + 1);

                    if (section == "[WindowSettings]")
                    {
                        if (key == "window_mode")
                            windowSettings.window_mode = value;
                        else if (key == "resolution")
                        {
                            size_t xPos = value.find('x');
                            if (xPos != std::string::npos)
                            {
                                windowSettings.resolution.width = std::stoi(value.substr(0, xPos));
                                windowSettings.resolution.height = std::stoi(value.substr(xPos + 1));
                            }
                        }
                        else if (key == "show_fps")
                            windowSettings.show_fps = (value == "true");
                        else if (key == "enable_debug")
                            windowSettings.enable_debug = (value == "true");
                    }
                    else if (section == "[AudioSettings]")
                    {
                        if (key == "enable_sound_effects")
                            audioSettings.enable_sound_effects = (value == "true");
                        else if (key == "enable_music")
                            audioSettings.enable_music = (value == "true");
                        else if (key == "overall_volume")
                            audioSettings.overall_volume = std::stof(value);
                        else if (key == "music_volume")
                            audioSettings.music_volume = std::stof(value);
                    }
                    else
                    {
                        PlayerControls *currentControls = nullptr;

                        if (section == "[Player1Controls]")
                        {
                            currentControls = &p1Controls;
                        }
                        else if (section == "[Player2Controls]")
                        {
                            currentControls = &p2Controls;
                        }

                        if (currentControls != nullptr)
                        {
                            if (key == "up")
                                currentControls->up = getKeyFromName(value);
                            else if (key == "down")
                                currentControls->down = getKeyFromName(value);
                            else if (key == "left")
                                currentControls->left = getKeyFromName(value);
                            else if (key == "right")
                                currentControls->right = getKeyFromName(value);
                            else if (key == "punch")
                                currentControls->punch = getKeyFromName(value);
                            else if (key == "parry")
                                currentControls->parry = getKeyFromName(value);
                        }
                    }
                }
            }
            file.close();
            std::cout << "Settings loaded from " << getConfigPath() << std::endl;
        }
        else
        {
            std::cout << "No settings file found at " << getConfigPath() << ", using defaults" << std::endl;
            resetToDefaults();
        }
    }

    const char *getKeyName(int key)
    {
        // Convert GLFW key codes to readable names
        switch (key)
        {
        // Letters
        case GLFW_KEY_A:
            return "A";
        case GLFW_KEY_B:
            return "B";
        case GLFW_KEY_C:
            return "C";
        case GLFW_KEY_D:
            return "D";
        case GLFW_KEY_E:
            return "E";
        case GLFW_KEY_F:
            return "F";
        case GLFW_KEY_G:
            return "G";
        case GLFW_KEY_H:
            return "H";
        case GLFW_KEY_I:
            return "I";
        case GLFW_KEY_J:
            return "J";
        case GLFW_KEY_K:
            return "K";
        case GLFW_KEY_L:
            return "L";
        case GLFW_KEY_M:
            return "M";
        case GLFW_KEY_N:
            return "N";
        case GLFW_KEY_O:
            return "O";
        case GLFW_KEY_P:
            return "P";
        case GLFW_KEY_Q:
            return "Q";
        case GLFW_KEY_R:
            return "R";
        case GLFW_KEY_S:
            return "S";
        case GLFW_KEY_T:
            return "T";
        case GLFW_KEY_U:
            return "U";
        case GLFW_KEY_V:
            return "V";
        case GLFW_KEY_W:
            return "W";
        case GLFW_KEY_X:
            return "X";
        case GLFW_KEY_Y:
            return "Y";
        case GLFW_KEY_Z:
            return "Z";

        // Arrow keys
        case GLFW_KEY_UP:
            return "UP";
        case GLFW_KEY_DOWN:
            return "DOWN";
        case GLFW_KEY_LEFT:
            return "LEFT";
        case GLFW_KEY_RIGHT:
            return "RIGHT";

        // Punctuation
        case GLFW_KEY_PERIOD:
            return ".";
        case GLFW_KEY_COMMA:
            return ",";
        case GLFW_KEY_SEMICOLON:
            return ";";
        case GLFW_KEY_APOSTROPHE:
            return "'";
        case GLFW_KEY_SLASH:
            return "/";
        case GLFW_KEY_BACKSLASH:
            return "\\";
        case GLFW_KEY_MINUS:
            return "-";
        case GLFW_KEY_EQUAL:
            return "=";

        // Numbers (both regular and numpad)
        case GLFW_KEY_0:
            return "0";
        case GLFW_KEY_1:
            return "1";
        case GLFW_KEY_2:
            return "2";
        case GLFW_KEY_3:
            return "3";
        case GLFW_KEY_4:
            return "4";
        case GLFW_KEY_5:
            return "5";
        case GLFW_KEY_6:
            return "6";
        case GLFW_KEY_7:
            return "7";
        case GLFW_KEY_8:
            return "8";
        case GLFW_KEY_9:
            return "9";
        case GLFW_KEY_KP_0:
            return "NUM 0";
        case GLFW_KEY_KP_1:
            return "NUM 1";
        case GLFW_KEY_KP_2:
            return "NUM 2";
        case GLFW_KEY_KP_3:
            return "NUM 3";
        case GLFW_KEY_KP_4:
            return "NUM 4";
        case GLFW_KEY_KP_5:
            return "NUM 5";
        case GLFW_KEY_KP_6:
            return "NUM 6";
        case GLFW_KEY_KP_7:
            return "NUM 7";
        case GLFW_KEY_KP_8:
            return "NUM 8";
        case GLFW_KEY_KP_9:
            return "NUM 9";

        // Function keys
        case GLFW_KEY_F1:
            return "F1";
        case GLFW_KEY_F2:
            return "F2";
        case GLFW_KEY_F3:
            return "F3";
        case GLFW_KEY_F4:
            return "F4";
        case GLFW_KEY_F5:
            return "F5";
        case GLFW_KEY_F6:
            return "F6";
        case GLFW_KEY_F7:
            return "F7";
        case GLFW_KEY_F8:
            return "F8";
        case GLFW_KEY_F9:
            return "F9";
        case GLFW_KEY_F10:
            return "F10";
        case GLFW_KEY_F11:
            return "F11";
        case GLFW_KEY_F12:
            return "F12";

        // Special keys
        case GLFW_KEY_SPACE:
            return "SPACE";
        case GLFW_KEY_TAB:
            return "TAB";
        case GLFW_KEY_ENTER:
            return "ENTER";
        case GLFW_KEY_ESCAPE:
            return "ESC";
        case GLFW_KEY_BACKSPACE:
            return "BACKSPACE";
        case GLFW_KEY_DELETE:
            return "DELETE";
        case GLFW_KEY_INSERT:
            return "INSERT";
        case GLFW_KEY_HOME:
            return "HOME";
        case GLFW_KEY_END:
            return "END";
        case GLFW_KEY_PAGE_UP:
            return "PG UP";
        case GLFW_KEY_PAGE_DOWN:
            return "PG DN";

        // Modifier keys
        case GLFW_KEY_LEFT_SHIFT:
            return "L SHIFT";
        case GLFW_KEY_RIGHT_SHIFT:
            return "R SHIFT";
        case GLFW_KEY_LEFT_CONTROL:
            return "L CTRL";
        case GLFW_KEY_RIGHT_CONTROL:
            return "R CTRL";
        case GLFW_KEY_LEFT_ALT:
            return "L ALT";
        case GLFW_KEY_RIGHT_ALT:
            return "R ALT";

        default:
            return "???";
        }
    }

    bool areSettingsDefault()
    {
        // Check window settings separately first
        bool windowMatches =
            // Compare window mode strings properly
            windowSettings.window_mode == DefaultConfig::WINDOW_SETTINGS.window_mode &&
            // Compare resolution
            windowSettings.resolution.width == DefaultConfig::WINDOW_SETTINGS.resolution.width &&
            windowSettings.resolution.height == DefaultConfig::WINDOW_SETTINGS.resolution.height &&
            // Compare boolean flags
            windowSettings.show_fps == DefaultConfig::WINDOW_SETTINGS.show_fps &&
            windowSettings.enable_debug == DefaultConfig::WINDOW_SETTINGS.enable_debug;

        // For debugging
        // if (!windowMatches)
        // {
        //     std::cout << "Window settings mismatch:" << std::endl;
        //     std::cout << "Mode: current=" << windowSettings.window_mode
        //               << " default=" << DefaultConfig::WINDOW_SETTINGS.window_mode << std::endl;
        //     std::cout << "Resolution: current=" << windowSettings.resolution.width << "x" << windowSettings.resolution.height
        //               << " default=" << DefaultConfig::WINDOW_SETTINGS.resolution.width << "x" << DefaultConfig::WINDOW_SETTINGS.resolution.height << std::endl;
        //     std::cout << "Show FPS: current=" << windowSettings.show_fps
        //               << " default=" << DefaultConfig::WINDOW_SETTINGS.show_fps << std::endl;
        //     std::cout << "Enable Bot: current=" << windowSettings.enable_bot
        //               << " default=" << DefaultConfig::WINDOW_SETTINGS.enable_bot << std::endl;
        //     std::cout << "Enable Debug: current=" << windowSettings.enable_debug
        //               << " default=" << DefaultConfig::WINDOW_SETTINGS.enable_debug << std::endl;
        // }

        return (
            // Check window settings
            windowMatches &&
            // Check controls
            memcmp(&p1Controls, &DefaultConfig::P1_CONTROLS, sizeof(PlayerControls)) == 0 &&
            memcmp(&p2Controls, &DefaultConfig::P2_CONTROLS, sizeof(PlayerControls)) == 0 &&
            // Check audio settings
            memcmp(&audioSettings, &DefaultConfig::AUDIO_SETTINGS, sizeof(AudioSettings)) == 0);
    }
}