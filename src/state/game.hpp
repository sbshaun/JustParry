#pragma once
#include "../constants.hpp"
#include "../renderer.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../common.hpp"
#include "stb_image/stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "../window.hpp"
#include "../world/world_system.hpp"
#include "../world/world_init.hpp"
#include <string>
#include <GLFW/glfw3.h>

enum class GameState
{
    MENU,
    ARCADE_MENU,
    LOADING,
    PLAYING,
    HELP,
    CHARACTER_SELECT,
    ARCADE_PREFIGHT,
    ARCADE_WIN,
    ARCADE_LOSE,
    SETTINGS,
    ROUND_START,
    ROUND_OVER,
    PAUSED,
    INIT
};

class Game
{
public:
    Game();
    ~Game();

    void setState(GameState newState);
    GameState getState() const;
    bool isRunning() const;
    void update();
    void updateArcadeLevel();
    void getPreviousState();
    void saveCurrentState();
    void render(GlRender &renderer);
    void generateBackground(float val, GlRender &renderer);
    void renderMenu(GlRender &renderer);
    void renderArcadeMenu(GlRender& renderer);
    bool handleMenuInput(GLFWwindow *window, GlRender &renderer);
    void handleArcadeButton();
    bool handleArcadeMenuInput(GLFWwindow* window);
    void handleBackButton();
    void handleSettingsButton();
    void handleHelpButton();
    void renderHelpScreen(GlRender &renderer);
    bool handleHelpInput(GLFWwindow *window);

    bool handleCharacterInput(GLFWwindow *window);
    void renderCharacterSelect(GlRender &renderer, float offset1, float offset2, bool p1, bool p2);
    void renderArcadePrefight(GlRender& renderer, float offset1, bool p1);
    void handleArcadePrefightInputs(GLWindow& glWindow, bool& p1KeyPressed, bool& p1Ready, bool& goDown1, bool& goUp1, float& offsetY1);
    void handleCharacterInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &p2KeyPressed,
                               bool &p2Ready, bool &goDown1, bool &goDown2, bool &goUp1, bool &goUp2, float &offsetY1, float &offsetY2);
    void renderReadyText(GlRender &renderer, bool p1Ready, bool p2Ready, Game &game);
    void resetGame(GlRender &renderer, WorldSystem &worldSystem);
    void updateScores(const Health &h1, const Health &h2);
    int getPlayer1Score() const { return player1Score; }
    int getPlayer2Score() const { return player2Score; }
    void resetScores()
    {
        player1Score = 0;
        player2Score = 0;
    }
    void renderSettingsScreen(GlRender &renderer);
    bool handleSettingsInput(GLFWwindow *window);
    WorldSystem *getWorldSystem() { return worldSystem; }
    void renderPauseButton(GlRender &renderer);

private:
    GameState currentState;
    bool running;
    float loadingProgress;
    bool showHelpDialog;
    bool isPaused;
    struct Button
    {
        float x, y, width, height;
        const char *text;
    };
    Button startButton;
    Button arcadeButton;
    Button helpButton;
    Button closeButton;
    Button backButton;
    Button settingsButton;
    Button pauseButton;
    Button resumeButton;
    Button menuButton;
    Button generalButton;
    Button controlsButton;
    Button windowButton;
    Button audioButton;
    Button player1Button;
    Button player2Button;
    Button applyButton;
    Button resetButton;
    Button pauseSettingsButton;

    Button arcadeLevelOneButton;
    Button arcadeLevelTwoButton;
    Button arcadeLevelThreeButton;
    Button arcadeLevelFourButton;
    Button arcadeLevelFiveButton;

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

    int player1Score = 0;
    int player2Score = 0;
    WorldSystem *worldSystem = nullptr;
    bool isGeneralSelected = true;
    bool isControlsSelected = false;
    bool isWindowSelected = true;
    bool isAudioSelected = false;
    bool isPlayer1Selected = true;
    bool isPlayer2Selected = false;

    int levelCompleted = 0;
    int currentLevel = 0;

    void cleanupButtons();

    bool isUpPressed = false;
    bool isDownPressed = false;
    bool isWPressed = false;
    bool isSPressed = false;
    bool isXPressed = false;
    bool isRPressed = false;
    bool isEnterPressed = false;
    bool isReturnPressed = false;
    bool buttonAlreadyHovered = false; // prevent sound spam 
    bool isMenuSelectedSoundPlaying = false; 
};
