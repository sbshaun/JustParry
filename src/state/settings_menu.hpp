#pragma once
#include <string>
#include "game.hpp"
#include "../settings.hpp"
#include "../renderer.hpp"
#include <fstream>
#include <iostream>
#include <string>

extern int timer;

namespace SettingsMenu
{
    void initializeButtons();
    void renderSettingsScreen(GlRender &renderer, Game &game);
    void renderWindowSettings(GlRender &renderer, double mx, double my);
    void handleControlsSettingsInput(GLFWwindow *window);
    void renderControlsSettings(GlRender &renderer, bool isPlayer1Selected,
                                bool isPlayer2Selected);
    void renderAudioSettings(GlRender &renderer, double mouseX, double mouseY, GLFWwindow *window, Game &game);

    // utilities
    bool isHovered(double mouseX, double mouseY, Button button);
    bool shouldClose();

    // Declare all variables as extern
    extern Button closeButton1;
    extern Button generalButton;
    extern Button controlsButton;
    extern Button windowButton;
    extern Button audioButton;
    extern Button player1Button;
    extern Button player2Button;
    extern Button resetButton;

    extern Button windowButton1;
    extern Button windowButton2;
    extern Button windowButton3;
    extern Button windowButton4;
    extern Button windowButton5;

    extern Button audioButton1;
    extern Button audioButton2;
    extern Button audioButton3;
    extern Button audioButton4;

    extern Button playerButton1;
    extern Button playerButton2;
    extern Button playerButton3;
    extern Button playerButton4;
    extern Button playerButton5;
    extern Button playerButton6;
    extern Button playerButton7;

    extern bool isGeneralSelected;
    extern bool isControlsSelected;
    extern bool isWindowSelected;
    extern bool isAudioSelected;
    extern bool isPlayer1Selected;
    extern bool isPlayer2Selected;

    extern bool showFps;

    extern bool isRebinding;
    extern int *currentlyRebindingKey;

    extern float errorMessageTimer;
    extern int errorButtonIndex;
    extern bool showErrorMessage;

    void cleanup();
    void resetButtons();
}