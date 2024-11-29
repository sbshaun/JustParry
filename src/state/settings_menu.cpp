#include "settings_menu.hpp"

namespace SettingsMenu
{
    // Define all variables here
    Button closeButton1;
    Button generalButton;
    Button controlsButton;
    Button windowButton;
    Button audioButton;
    Button player1Button;
    Button player2Button;
    Button resetButton;

    Button windowButton1;
    Button windowButton2;
    Button windowButton3;
    Button windowButton4;
    Button windowButton5;

    Button audioButton1;
    Button audioButton2;
    Button audioButton3;
    Button audioButton4;

    Button playerButton1;
    Button playerButton2;
    Button playerButton3;
    Button playerButton4;
    Button playerButton5;
    Button playerButton6;
    Button playerButton7;

    bool isGeneralSelected = true;
    bool isControlsSelected = false;
    bool isWindowSelected = true;
    bool isAudioSelected = false;
    bool isPlayer1Selected = true;
    bool isPlayer2Selected = false;

    bool showFps = false;

    bool isRebinding = false;
    int *currentlyRebindingKey = nullptr;

    float errorMessageTimer = 0.0f;
    int errorButtonIndex = -1;
    bool showErrorMessage = false;

    void initializeButtons()
    {
        generalButton = {
            M_WINDOW_WIDTH_PX / 2.0f - 295.0f,  // x position
            M_WINDOW_HEIGHT_PX / 2.0f - 217.5f, // y position
            275.0f,                             // width
            45.0f,                              // height
            "GENERAL"};

        controlsButton = {
            M_WINDOW_WIDTH_PX / 2.0f + 30.f,    // x position
            M_WINDOW_HEIGHT_PX / 2.0f - 217.5f, // y position
            275.0f,                             // width
            45.0f,                              // height
            "CONTROLS"};

        // Add smaller toggle buttons
        windowButton = {
            M_WINDOW_WIDTH_PX / 2.0f - 210.0f, // Same x as generalButton
            M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // 50px below generalButton
            190.0f,                            // Smaller width
            35.0f,                             // Smaller height
            "WINDOW"};

        audioButton = {
            M_WINDOW_WIDTH_PX / 2.0f + 30.0f,  // Next to windowButton
            M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as windowButton
            190.0f,                            // Same width
            35.0f,                             // Same height
            "AUDIO"};

        player1Button = {
            M_WINDOW_WIDTH_PX / 2.0f - 210.0f, // Same x as controlsButton
            M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as other toggle buttons
            190.0f,                            // Same width
            35.0f,                             // Same height
            "PLAYER 1"};

        player2Button = {
            M_WINDOW_WIDTH_PX / 2.0f + 30.0f,  // Next to player1Button
            M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as other toggle buttons
            190.0f,                            // Same width
            35.0f,                             // Same height
            "PLAYER 2"};

        // Initialize reset button (bottom left of settings)
        resetButton = {
            125.0f,                      // x - align with labels
            M_WINDOW_HEIGHT_PX - 160.0f, // y - 100px from bottom
            175.0f,                      // width
            60.0f,                       // height
            "RESET"};

        // Add new button definitions
        windowButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 65.0f, 210.0f, 40.0f, "Placeholder 2"};
        windowButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 15.0f, 210.0f, 40.0f, "Placeholder 3"};
        windowButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 40.0f, "Placeholder 4"};
        windowButton5 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 85.0f, 210.0f, 40.0f, "Placeholder 5"};
        windowButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 135.0f, 210.0f, 40.0f, "Placeholder 1"};

        audioButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 65.0f, 210.0f, 40.0f, "Placeholder 2"};
        audioButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 15.0f, 210.0f, 40.0f, "Placeholder 3"};
        audioButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 40.0f, "Placeholder 4"};
        audioButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 85.0f, 210.0f, 40.0f, "Placeholder 5"};

        playerButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 70.f, 210.0f, 30.0f, "Placeholder 1"};
        playerButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 35.0f, 210.0f, 30.0f, "Placeholder 2"};
        playerButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 0.0f, 210.0f, 30.0f, "Placeholder 3"};
        playerButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 30.0f, "Placeholder 4"};
        playerButton5 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 90.0f, 210.0f, 30.0f, "Placeholder 5"};
        playerButton6 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 125.0f, 210.0f, 30.0f, "Placeholder 6"};
        playerButton7 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 160.0f, 210.0f, 30.0f, "Placeholder 7"};
    }

    bool shouldClose()
    {
        GLFWwindow *window = glfwGetCurrentContext();
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        bool closeHovered = isHovered(mouseX, mouseY, closeButton1);
        bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        return closePressed;
    }

    bool isHovered(double mouseX, double mouseY, Button button)
    {
        return mouseX >= button.x &&
               mouseX <= button.x + button.width &&
               mouseY >= button.y &&
               mouseY <= button.y + button.height;
    }

    void renderWindowSettings(GlRender &renderer, double mx, double my)
    {
        // Window settings labels and buttons
        float labelX = 215;
        float buttonX = M_WINDOW_WIDTH_PX / 2.0f + 60.0f;
        float startY = 350;
        float spacing = 50;
        float buttonWidth = 210.0f;
        float buttonHeight = 40.0f;

        // Labels - gray out the disabled options
        renderer.renderText("WINDOW MODE", labelX, startY, 0.24f, glm::vec3(0.5f, 0.5f, 0.5f));          // Grayed out
        renderer.renderText("RESOLUTION", labelX, startY + spacing, 0.24f, glm::vec3(0.5f, 0.5f, 0.5f)); // Grayed out
        renderer.renderText("SHOW FPS COUNTER [F]", labelX, startY + spacing * 2, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("ENABLE DEBUG MODE [D]", labelX, startY + spacing * 4, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));

        // button positions
        float buttonY = startY - 22;
        windowButton1 = {buttonX, buttonY, buttonWidth, buttonHeight, "WINDOWED"};             // Fixed text
        windowButton2 = {buttonX, buttonY + spacing, buttonWidth, buttonHeight, "1024 x 768"}; // Fixed text
        windowButton3 = {buttonX, buttonY + spacing * 2, buttonWidth, buttonHeight, Settings::windowSettings.show_fps ? "ON" : "OFF"};
        windowButton5 = {buttonX, buttonY + spacing * 4, buttonWidth, buttonHeight, Settings::windowSettings.enable_debug ? "ON" : "OFF"};

        // Check hover states (only for enabled buttons)
        bool wButton1Hovered = false; // Disabled
        bool wButton2Hovered = false; // Disabled
        bool wButton3Hovered = isHovered(mx, my, windowButton3);
        bool wButton5Hovered = isHovered(mx, my, windowButton5);

        // Handle clicks (only for enabled buttons)
        static bool wasPressed = false;
        GLFWwindow *window = glfwGetCurrentContext();
        bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (isPressed && !wasPressed)
        {
            if (wButton3Hovered) // FPS Counter
            {
                Settings::windowSettings.show_fps = !Settings::windowSettings.show_fps;
                showFps = Settings::windowSettings.show_fps;
                Settings::saveSettings();
                std::cout << "FPS display " << (showFps ? "enabled" : "disabled") << std::endl;
            }
            else if (wButton5Hovered) // Debug Mode
            {
                Settings::windowSettings.enable_debug = !Settings::windowSettings.enable_debug;
                renderer.debugMode = Settings::windowSettings.enable_debug;
                Settings::saveSettings();
                std::cout << "Debug mode " << (renderer.debugMode ? "enabled" : "disabled") << std::endl;
            }
        }
        wasPressed = isPressed;

        // Render buttons with hover effect
        const glm::vec3 disabledColor(0.4f, 0.4f, 0.4f); // Darker gray for disabled buttons
        const glm::vec3 placeholderColor(0.6f, 0.6f, 0.6f);
        const glm::vec3 white(1.0f, 1.0f, 1.0f);
        const glm::vec3 grayText(0.7f, 0.7f, 0.7f); // Gray text for disabled buttons

        // Render disabled buttons with gray color
        renderer.renderSimpleButton(windowButton1.x, windowButton1.y, windowButton1.width, windowButton1.height,
                                    true, false, false, disabledColor);
        renderer.renderSimpleButton(windowButton2.x, windowButton2.y, windowButton2.width, windowButton2.height,
                                    true, false, false, disabledColor);

        // Render enabled buttons normally
        renderer.renderSimpleButton(windowButton3.x, windowButton3.y, windowButton3.width, windowButton3.height,
                                    true, wButton3Hovered, false, placeholderColor);
        renderer.renderSimpleButton(windowButton5.x, windowButton5.y, windowButton5.width, windowButton5.height,
                                    true, wButton5Hovered, false, placeholderColor);

        // Render button text (grayed out for disabled buttons)
        renderer.renderText(windowButton1.text, windowButton1.x + 45.f, windowButton1.y + 30.f, 0.25f, grayText);
        renderer.renderText(windowButton2.text, windowButton2.x + 40.f, windowButton2.y + 30.f, 0.25f, grayText);
        if (strcmp(windowButton3.text, "ON") == 0)
        {
            renderer.renderText(windowButton3.text, windowButton3.x + 85.f, windowButton3.y + 30.f, 0.25f, white);
        }
        else
        {
            renderer.renderText(windowButton3.text, windowButton3.x + 80.f, windowButton3.y + 30.f, 0.25f, white);
        }

        if (strcmp(windowButton5.text, "ON") == 0)
        {
            renderer.renderText(windowButton5.text, windowButton5.x + 85.f, windowButton5.y + 30.f, 0.25f, white);
        }
        else
        {
            renderer.renderText(windowButton5.text, windowButton5.x + 80.f, windowButton5.y + 30.f, 0.25f, white);
        }
    }

    void renderAudioSettings(GlRender &renderer, double mouseX, double mouseY, GLFWwindow *window, Game &game)
    {
        // Audio settings labels and buttons
        float labelX = 215;
        float buttonX = M_WINDOW_WIDTH_PX / 2.0f + 60.0f;
        float startY = 350;
        float spacing = 50;
        float buttonWidth = 210.0f;
        float buttonHeight = 40.0f;

        // Labels
        renderer.renderText("ENABLE SOUND EFFECTS", labelX, startY, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("ENABLE MUSIC", labelX, startY + spacing, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("OVERALL VOLUME", labelX, startY + spacing * 2, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("MUSIC VOLUME", labelX, startY + spacing * 3, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));

        // Update button positions and texts
        float buttonY = startY - 22;
        audioButton1 = {buttonX, buttonY, buttonWidth, buttonHeight,
                        Settings::audioSettings.enable_sound_effects ? "ON" : "OFF"};
        audioButton2 = {buttonX, buttonY + spacing, buttonWidth, buttonHeight,
                        Settings::audioSettings.enable_music ? "ON" : "OFF"};
        audioButton3 = {buttonX, buttonY + spacing * 2, buttonWidth, buttonHeight,
                        (std::to_string(int(Settings::audioSettings.overall_volume * 100)) + "%").c_str()};
        audioButton4 = {buttonX, buttonY + spacing * 3, buttonWidth, buttonHeight,
                        (std::to_string(int(Settings::audioSettings.music_volume * 100)) + "%").c_str()};

        // Check hover states
        bool aButton1Hovered = mouseX >= audioButton1.x && mouseX <= audioButton1.x + audioButton1.width &&
                               mouseY >= audioButton1.y && mouseY <= audioButton1.y + audioButton1.height;
        bool aButton2Hovered = mouseX >= audioButton2.x && mouseX <= audioButton2.x + audioButton2.width &&
                               mouseY >= audioButton2.y && mouseY <= audioButton2.y + audioButton2.height;
        bool aButton3Hovered = mouseX >= audioButton3.x && mouseX <= audioButton3.x + audioButton3.width &&
                               mouseY >= audioButton3.y && mouseY <= audioButton3.y + audioButton3.height;
        bool aButton4Hovered = mouseX >= audioButton4.x && mouseX <= audioButton4.x + audioButton4.width &&
                               mouseY >= audioButton4.y && mouseY <= audioButton4.y + audioButton4.height;

        static bool isDraggingOverall = false;
        static bool isDraggingMusic = false;

        // Handle clicks and drags
        bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        static bool wasPressed = false;

        if (isPressed)
        {
            if (!wasPressed)
            {
                // Initial click
                if (aButton1Hovered)
                {
                    Settings::audioSettings.enable_sound_effects = !Settings::audioSettings.enable_sound_effects;
                    Settings::saveSettings();
                    WorldSystem::updateVolume(); // Add this line
                }
                else if (aButton2Hovered)
                {
                    Settings::audioSettings.enable_music = !Settings::audioSettings.enable_music;
                    Settings::saveSettings();

                    // Only update audio state immediately if we came from pause menu
                    if (game.getPreviousState() == GameState::PAUSED)
                    {
                        WorldSystem::updateAudioState();
                    }
                }
                else if (aButton3Hovered)
                {
                    isDraggingOverall = true;
                }
                else if (aButton4Hovered)
                {
                    isDraggingMusic = true;
                }
            }

            // Handle dragging for volume sliders
            if (isDraggingOverall)
            {
                float relativeX = std::max(0.0f, std::min(1.0f, (float)(mouseX - audioButton3.x) / audioButton3.width));
                Settings::audioSettings.overall_volume = relativeX;
                Settings::saveSettings();
                WorldSystem::updateVolume(); // Add this line
            }
            else if (isDraggingMusic)
            {
                float relativeX = std::max(0.0f, std::min(1.0f, (float)(mouseX - audioButton4.x) / audioButton4.width));
                Settings::audioSettings.music_volume = relativeX;
                Settings::saveSettings();
                WorldSystem::updateVolume(); // Add this line
            }
        }
        else
        {
            isDraggingOverall = false;
            isDraggingMusic = false;
        }
        wasPressed = isPressed;

        // Render buttons
        const glm::vec3 placeholderColor(0.6f, 0.6f, 0.6f);
        // Use the theme color for sliders
        const glm::vec3 sliderColor(139.0f / 255.0f, 169.0f / 255.0f, 174.0f / 255.0f);
        const glm::vec3 white(1.0f, 1.0f, 1.0f);

        // Render ON/OFF buttons
        renderer.renderSimpleButton(audioButton1.x, audioButton1.y, audioButton1.width, audioButton1.height,
                                    true, aButton1Hovered, false, placeholderColor);
        renderer.renderSimpleButton(audioButton2.x, audioButton2.y, audioButton2.width, audioButton2.height,
                                    true, aButton2Hovered, false, placeholderColor);

        // Render volume sliders
        // Background
        renderer.renderSimpleButton(audioButton3.x, audioButton3.y, audioButton3.width, audioButton3.height,
                                    true, aButton3Hovered, false, placeholderColor);
        renderer.renderSimpleButton(audioButton4.x, audioButton4.y, audioButton4.width, audioButton4.height,
                                    true, aButton4Hovered, false, placeholderColor);

        // Filled portion for overall volume
        float fillWidth3 = audioButton3.width * Settings::audioSettings.overall_volume;
        renderer.renderSimpleButton(audioButton3.x, audioButton3.y, fillWidth3, audioButton3.height,
                                    false, false, false, sliderColor);

        // Filled portion for music volume
        float fillWidth4 = audioButton4.width * Settings::audioSettings.music_volume;
        renderer.renderSimpleButton(audioButton4.x, audioButton4.y, fillWidth4, audioButton4.height,
                                    false, false, false, sliderColor);

        // Render button text
        if (strcmp(audioButton1.text, "ON") == 0)
        {
            renderer.renderText(audioButton1.text, audioButton1.x + 85.f, audioButton1.y + 30.f, 0.25f, white);
        }
        else
        {
            renderer.renderText(audioButton1.text, audioButton1.x + 80.f, audioButton1.y + 30.f, 0.25f, white);
        }

        if (strcmp(audioButton2.text, "ON") == 0)
        {
            renderer.renderText(audioButton2.text, audioButton2.x + 85.f, audioButton2.y + 30.f, 0.25f, white);
        }
        else
        {
            renderer.renderText(audioButton2.text, audioButton2.x + 80.f, audioButton2.y + 30.f, 0.25f, white);
        }

        // Render volume percentages centered on the sliders
        std::string overallVolText = std::to_string(int(Settings::audioSettings.overall_volume * 100)) + "%";
        std::string musicVolText = std::to_string(int(Settings::audioSettings.music_volume * 100)) + "%";

        renderer.renderText(overallVolText.c_str(),
                            audioButton3.x + (audioButton3.width - overallVolText.length() * 15) / 2,
                            audioButton3.y + 30.f, 0.25f, white);

        renderer.renderText(musicVolText.c_str(),
                            audioButton4.x + (audioButton4.width - musicVolText.length() * 15) / 2,
                            audioButton4.y + 30.f, 0.25f, white);
    }

    void handleControlsSettingsInput(GLFWwindow *window)
    {
        // Get mouse position for reset button
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Check if reset button is clicked
        bool resetHovered = mouseX >= resetButton.x &&
                            mouseX <= resetButton.x + resetButton.width &&
                            mouseY >= resetButton.y &&
                            mouseY <= resetButton.y + resetButton.height;

        static bool wasResetPressed = false;
        bool isResetPressed = resetHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Only trigger reset if settings are not default and button is pressed
        if (isResetPressed && !wasResetPressed && !Settings::areSettingsDefault())
        {
            Settings::resetToDefaults();
        }
        wasResetPressed = isResetPressed;

        if (isRebinding)
        {
            // Check for any key press
            for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++)
            {
                if (glfwGetKey(window, key) == GLFW_PRESS)
                {
                    if (currentlyRebindingKey != nullptr)
                    {
                        // Check if this is the same key that's currently bound
                        if (*currentlyRebindingKey == key)
                        {
                            isRebinding = false;
                            currentlyRebindingKey = nullptr;
                            return;
                        }

                        // Check if this key is already bound
                        bool keyAlreadyBound = false;
                        PlayerControls &p1 = Settings::p1Controls;
                        PlayerControls &p2 = Settings::p2Controls;

                        // Check P1 controls
                        if (key == p1.down || key == p1.left || key == p1.right ||
                            key == p1.punch || key == p1.kick || key == p1.parry)
                        {
                            keyAlreadyBound = true;
                        }

                        // Check P2 controls
                        if (key == p2.down || key == p2.left || key == p2.right ||
                            key == p2.punch || key == p2.kick || key == p2.parry)
                        {
                            keyAlreadyBound = true;
                        }

                        if (keyAlreadyBound)
                        {
                            // Show error feedback
                            showErrorMessage = true;
                            errorMessageTimer = 0.0f;

                            // Find which button index we're on
                            PlayerControls &controls = isPlayer1Selected ? Settings::p1Controls : Settings::p2Controls;
                            int *keys[] = {&controls.down, &controls.left, &controls.right,
                                           &controls.punch, &controls.kick, &controls.parry};

                            for (int i = 0; i < 7; i++)
                            {
                                if (currentlyRebindingKey == keys[i])
                                {
                                    errorButtonIndex = i;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // Update the key binding
                            *currentlyRebindingKey = key;
                            Settings::saveSettings();
                            Settings::loadSettings();
                        }
                    }

                    isRebinding = false;
                    currentlyRebindingKey = nullptr;
                    break;
                }
            }

            // Allow canceling with backspace
            if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
            {
                isRebinding = false;
                currentlyRebindingKey = nullptr;
            }
        }
    }

    void renderControlsSettings(GlRender &renderer, bool isPlayer1Selected,
                                bool isPlayer2Selected)
    {
        // Get mouse position
        GLFWwindow *window = glfwGetCurrentContext();
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Define key button dimensions - make them wider
        float buttonWidth = 215.0f;
        float buttonHeight = 35.0f; // Increased from 30.0f
        float buttonX = 570.0f;     // Adjusted from 665.0f to align with previous buttons
        float labelX = 300.0f;      // Position for labels
        float startY = 330.0f;      // Starting Y position
        float spacing = 40.0f;      // Increased spacing between buttons

        // Render control labels - adjust kick label to show combination
        renderer.renderText("DUCK", labelX, startY, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("MOVE LEFT", labelX - 25.0f, startY + spacing, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("MOVE RIGHT", labelX - 32.0f, startY + spacing * 2, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("PUNCH", labelX, startY + spacing * 3, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("KICK (DUCK + PUNCH)", labelX - 65.0f, startY + spacing * 4, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("PARRY", labelX, startY + spacing * 5, 0.24f,
                            glm::vec3(0.0f, 0.0f, 0.0f));

        PlayerControls &controls =
            isPlayer1Selected ? Settings::p1Controls : Settings::p2Controls;
        float yPositions[] = {startY - 22,
                              startY - 22 + spacing,
                              startY - 22 + spacing * 2,
                              startY - 22 + spacing * 3,
                              startY - 22 + spacing * 4,
                              startY - 22 + spacing * 5};

        int *keys[] = {&controls.down, &controls.left, &controls.right,
                       &controls.punch, &controls.kick, &controls.parry};

        // Render each key binding button
        for (int i = 0; i < 6; i++)
        {
            float y = yPositions[i];
            bool isHovered = mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
                             mouseY >= y && mouseY <= y + buttonHeight;
            bool isSelected = currentlyRebindingKey == keys[i];
            bool isError = (i == errorButtonIndex && showErrorMessage);
            bool isKickButton = (i == 4); // Index 4 is the kick button

            // Skip interaction checks for kick button
            if (isKickButton) {
                // Render disabled button for kick
                renderer.renderSimpleButton(buttonX, y, buttonWidth, buttonHeight,
                                            true, false, false,
                                            glm::vec3(0.4f, 0.4f, 0.4f)); // Gray color for disabled state

                // Show the key combination instead of a single key
                std::string comboText = std::string(Settings::getKeyName(controls.down)) + 
                                      " + " + 
                                      std::string(Settings::getKeyName(controls.punch));
                float textWidth = comboText.length() * 8.0f;
                float textX = buttonX + (buttonWidth - textWidth) / 2.0f;
                float textY = y + (buttonHeight - 8.0f) / 2.0f;

                renderer.renderText(comboText, textX - 10.f, textY + 10.f,
                                    0.24f, glm::vec3(0.7f, 0.7f, 0.7f)); // Gray text for disabled state
                continue;
            }

            // Calculate error alpha for fade effect
            float errorAlpha = 0.0f;
            if (isError)
            {
                if (errorMessageTimer < 1.25f)
                {
                    // Stay solid for 1.25 seconds
                    errorAlpha = 1.0f;
                }
                else
                {
                    // Fade out over 0.5 seconds (from 1.25s to 1.75s)
                    errorAlpha = 1.0f - ((errorMessageTimer - 1.25f) / 0.5f);
                }
            }

            // Render button background with error tint if needed
            if (isError)
            {
                // Render red tinted button with fade
                glm::vec3 buttonColor = glm::vec3(1.0f, 0.0f, 0.0f);
                renderer.renderSimpleButton(buttonX, y, buttonWidth, buttonHeight,
                                            true, isHovered, isSelected,
                                            glm::vec3(
                                                buttonColor.r * errorAlpha + 0.6f * (1.0f - errorAlpha),
                                                buttonColor.g * errorAlpha + 0.6f * (1.0f - errorAlpha),
                                                buttonColor.b * errorAlpha + 0.6f * (1.0f - errorAlpha)));

                // Render error icon with fade
                if (errorAlpha > 0.0f)
                {
                    renderer.renderText("!", buttonX + buttonWidth + 10, y + buttonHeight / 2 + 10,
                                        0.35f, glm::vec3(1.0f * errorAlpha, 0.0f, 0.0f));
                }
            }
            else
            {
                // Normal button render
                renderer.renderSimpleButton(buttonX, y, buttonWidth, buttonHeight,
                                            true, isHovered, isSelected);
            }

            // Render key text
            std::string keyText = isSelected ? "___" : Settings::getKeyName(*keys[i]);
            float textWidth = keyText.length() * 8.0f;
            float textX = buttonX + (buttonWidth - textWidth) / 2.0f;
            float textY = y + (buttonHeight - 8.0f) / 2.0f;

            renderer.renderText(keyText, textX - 10.f, textY + 10.f,
                                0.24f, glm::vec3(1.0f, 1.0f, 1.0f));

            // Handle click
            if (isHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
                !isRebinding && !currentlyRebindingKey)
            {
                currentlyRebindingKey = keys[i];
                isRebinding = true;
            }
        }

        // Update error message timer
        if (showErrorMessage)
        {
            errorMessageTimer += 0.016f; // Assuming 60 FPS
            if (errorMessageTimer >= 1.75f)
            { // Total duration: 1.25s solid + 0.5s fade
                showErrorMessage = false;
                errorMessageTimer = 0.0f;
                errorButtonIndex = -1;
            }
        }
    }

    void renderSettingsScreen(GlRender &renderer, Game &game)
    {
        // First render the appropriate background based on where we came from
        if (game.getPreviousState() == GameState::PAUSED)
        {
            // If from pause menu, render the game state
            renderer.render();
            renderer.renderUI(::timer);
            game.renderPauseButton(renderer);
        }
        else if (game.getPreviousState() == GameState::MENU)
        {
            // If from main menu, render the menu background first
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.renderTexturedQuadScaled(
                renderer.m_menuTexture,
                0, 0,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f // Full brightness
            );
        }

        // Render the semi-transparent black overlay
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture, // Use any texture
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            0.0f, // Set brightness to 0 to make it black
            0.5f  // 50% opacity for the black overlay
        );

        // Calculate dimensions for the settings screen image
        float settingsBoxWidth = 850.0f;
        float settingsBoxHeight = 600.0f;
        float settingsBoxX = (M_WINDOW_WIDTH_PX - settingsBoxWidth) / 2.0f;
        float settingsBoxY = (M_WINDOW_HEIGHT_PX - settingsBoxHeight) / 2.0f;

        // Render the settings screen image in the center
        if (renderer.m_settingsTexture != 0)
        {
            renderer.renderTexturedQuadScaled(
                renderer.m_settingsTexture,
                settingsBoxX, settingsBoxY,
                settingsBoxWidth, settingsBoxHeight,
                1.0f, // Full brightness
                1.0f  // Full opacity
            );
        }

        renderer.renderText("SETTINGS", 410, 140, 0.55f, glm::vec3(0.0f, 0.0f, 0.0f));

        // Position and size the close button (make it square)
        closeButton1 = {
            settingsBoxX + settingsBoxWidth - 75.f, // 10px padding from right
            settingsBoxY + 10.0f,                   // 10px padding from top
            65.0f,                                  // Square width
            50.0f,                                  // Square height
            "X"};

        // Get mouse position and check hover/press state
        GLFWwindow *window = glfwGetCurrentContext();
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        bool closeHovered = isHovered(mouseX, mouseY, closeButton1);
        bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Render close button
        renderer.renderButton(
            closeButton1.x, closeButton1.y,
            closeButton1.width, closeButton1.height,
            closeButton1.text,
            closeHovered, closePressed,
            glm::vec3(0.7f, 0.1f, 0.1f) // Dark red color
        );

        // Get mouse position and check hover/press states
        bool generalHovered = isHovered(mouseX, mouseY, generalButton);
        bool controlsHovered = isHovered(mouseX, mouseY, controlsButton);

        // Add hover checks for the toggle buttons
        bool windowHovered = isHovered(mouseX, mouseY, windowButton);
        bool audioHovered = isHovered(mouseX, mouseY, audioButton);

        bool player1Hovered = isHovered(mouseX, mouseY, player1Button);
        bool player2Hovered = isHovered(mouseX, mouseY, player2Button);

        // Handle button clicks
        static bool wasPressed = false;
        bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool generalPressed = generalHovered && isPressed;
        bool controlsPressed = controlsHovered && isPressed;
        bool windowPressed = windowHovered && isPressed;
        bool audioPressed = audioHovered && isPressed;
        bool player1Pressed = player1Hovered && isPressed;
        bool player2Pressed = player2Hovered && isPressed;

        if (isPressed && !wasPressed)
        {
            if (generalHovered)
            {
                isGeneralSelected = true;
                isControlsSelected = false;
            }
            else if (controlsHovered)
            {
                isGeneralSelected = false;
                isControlsSelected = true;
            }
            else if (windowHovered && isGeneralSelected)
            {
                isWindowSelected = true;
                isAudioSelected = false;
            }
            else if (audioHovered && isGeneralSelected)
            {
                isWindowSelected = false;
                isAudioSelected = true;
            }
            else if (player1Hovered && isControlsSelected)
            {
                isPlayer1Selected = true;
                isPlayer2Selected = false;
            }
            else if (player2Hovered && isControlsSelected)
            {
                isPlayer1Selected = false;
                isPlayer2Selected = true;
            }
        }
        wasPressed = isPressed;

        // Save current OpenGL state
        GLboolean depthTestEnabled;
        glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
        GLint blendSrc, blendDst;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

        // Disable depth testing and enable blending for all 2D elements
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Original color (normalized)
        glm::vec3 color(139.0f / 255.0f, 169.0f / 255.0f, 174.0f / 255.0f);
        glm::vec3 darkerColor = color * 1.075f;

        // GENERAL
        renderer.renderSimpleButton(
            generalButton.x, generalButton.y,
            generalButton.width, generalButton.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);
        // CONTROLS
        renderer.renderSimpleButton(
            controlsButton.x, controlsButton.y,
            controlsButton.width, controlsButton.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);

        // Then render the button text with hover effect
        renderer.renderText("GENERAL",
                            generalButton.x + 50.0f, generalButton.y + 35.0f, 0.4f,
                            isGeneralSelected || generalHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

        renderer.renderText("CONTROLS",
                            controlsButton.x + 40.0f, controlsButton.y + 35.0f, 0.4f,
                            isControlsSelected || controlsHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

        // Render content based on which button is selected
        if (isGeneralSelected)
        {
            // WINDOW SETTINGS
            renderer.renderSimpleButton(
                windowButton.x, windowButton.y,
                windowButton.width, windowButton.height,
                false, // No selection animation
                false, // No hover animation for button
                false, // No pressed animation
                darkerColor);

            // AUDIO SETTINGS
            renderer.renderSimpleButton(
                audioButton.x, audioButton.y,
                audioButton.width, audioButton.height,
                false, // No selection animation
                false, // No hover animation for button
                false, // No pressed animation
                darkerColor);

            renderer.renderText("WINDOW",
                                windowButton.x + 33.5f, windowButton.y + 27.5f, 0.3f,
                                isWindowSelected || windowHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("AUDIO",
                                audioButton.x + 47.5f, audioButton.y + 27.5f, 0.3f,
                                isAudioSelected || audioHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

            if (isWindowSelected)
            {
                renderWindowSettings(renderer, mouseX, mouseY);
            }
            else if (isAudioSelected)
            {
                renderAudioSettings(renderer, mouseX, mouseY, window, game);
            }
        }
        else if (isControlsSelected)
        {
            renderer.renderSimpleButton(
                player1Button.x, player1Button.y,
                player1Button.width, player1Button.height,
                false, // No selection animation
                false, // No hover animation for button
                false, // No pressed animation
                darkerColor);

            renderer.renderSimpleButton(
                player2Button.x, player2Button.y,
                player2Button.width, player2Button.height,
                false, // No selection animation
                false, // No hover animation for button
                false, // No pressed animation
                darkerColor);

            renderer.renderText("PLAYER 1",
                                player1Button.x + 30.0f, player1Button.y + 27.5f, 0.3f,
                                isPlayer1Selected || player1Hovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("PLAYER 2",
                                player2Button.x + 27.5f, player2Button.y + 27.5f, 0.3f,
                                isPlayer2Selected || player2Hovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

            // render controls
            renderControlsSettings(renderer, isPlayer1Selected, isPlayer2Selected);
        }

        // Restore previous OpenGL state
        if (depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        glBlendFunc(blendSrc, blendDst);

        // Check if settings are at default values
        bool isDefaultSettings = Settings::areSettingsDefault();

        // Get mouse position for reset button
        bool resetHovered = isHovered(mouseX, mouseY, resetButton);
        bool resetPressed = resetHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Handle reset button click
        static bool wasResetPressed = false;
        if (resetPressed && !wasResetPressed && !isDefaultSettings)
        {
            Settings::resetToDefaults();
            // Update local state
            showFps = Settings::windowSettings.show_fps;
            renderer.debugMode = Settings::windowSettings.enable_debug;
            std::cout << "Settings reset to defaults" << std::endl;
        }
        wasResetPressed = resetPressed;

        // Render the reset button based on current settings state
        if (isDefaultSettings)
        {
            // Render disabled reset button (grayed out)
            renderer.renderButton(
                resetButton.x, resetButton.y,
                resetButton.width, resetButton.height,
                resetButton.text,
                false,                      // Not hoverable
                false,                      // Not pressable
                glm::vec3(0.4f, 0.4f, 0.4f) // Gray color for disabled state
            );
        }
        else
        {
            // Render normal reset button
            renderer.renderButton(
                resetButton.x, resetButton.y,
                resetButton.width, resetButton.height,
                resetButton.text,
                resetHovered,
                resetPressed,
                glm::vec3(0.6f, 0.2f, 0.2f) // Normal red color
            );
        }
    }

    void resetButtons()
    {
        // Reset all buttons to default state
        closeButton1 = {};
        generalButton = {};
        controlsButton = {};
        windowButton = {};
        audioButton = {};
        player1Button = {};
        player2Button = {};
        resetButton = {};

        windowButton1 = {};
        windowButton2 = {};
        windowButton3 = {};
        windowButton4 = {};
        windowButton5 = {};

        audioButton1 = {};
        audioButton2 = {};
        audioButton3 = {};
        audioButton4 = {};

        playerButton1 = {};
        playerButton2 = {};
        playerButton3 = {};
        playerButton4 = {};
        playerButton5 = {};
        playerButton6 = {};
        playerButton7 = {};
    }

    void cleanup()
    {
        // Reset all buttons
        resetButtons();

        // Reset all state variables
        isGeneralSelected = true;
        isControlsSelected = false;
        isWindowSelected = true;
        isAudioSelected = false;
        isPlayer1Selected = true;
        isPlayer2Selected = false;

        showFps = false;

        // Reset rebinding state
        isRebinding = false;
        currentlyRebindingKey = nullptr; // Don't delete as this points to Settings data

        // Reset error state
        errorMessageTimer = 0.0f;
        errorButtonIndex = -1;
        showErrorMessage = false;

        std::cout << "Settings menu resources cleaned up." << std::endl;
    }
}
