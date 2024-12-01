#pragma once
#include "../constants.hpp"
#include "../graphics/renderer.hpp"
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
#include "../settings/settings.hpp"

enum class GameState
{
    MENU,
    ARCADE_MENU,
    LOADING,
    PLAYING,
    HELP,
    CHARACTER_SELECT,
    ARCADE_PREFIGHT,
    ARCADE_STORY,
    ARCADE_WIN,
    ARCADE_LOSE,
    SETTINGS,
    SETTINGS_EXIT,
    ROUND_START,
    ROUND_OVER,
    MATCH_OVER,
    LEVEL_OVER,
    PAUSED,
    INIT
};

struct Button
{
    float x, y, width, height;
    const char *text;
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
    void loadArcadeState();
    void saveCurrentState();
    void render(GlRender &renderer);
    void generateBackground(float val, GlRender &renderer);
    void renderMenu(GlRender &renderer);
    void renderArcadeMenu(GlRender &renderer);
    void renderArcadeStory(GlRender& renderer);
    bool handleMenuInput(GLFWwindow *window, GlRender &renderer);
    void handleArcadeButton();
    bool handleArcadeMenuInput(GLFWwindow *window);
    bool handleArcadeStoryInput(GLFWwindow* window);
    void handleBackButton();
    void handleSettingsButton();
    void handleHelpButton();
    void renderHelpScreen(GlRender &renderer, bool &botEnabled);
    bool handleHelpInput(GLFWwindow *window);
    void attemptPause();

    bool handleCharacterInput(GLFWwindow *window);
    void renderCharacterSelect(GlRender &renderer, float offset1, float offset2, bool p1, bool p2);
    void renderArcadePrefight(GlRender &renderer, float offset1, bool p1);
    void handleArcadePrefightInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &goDown1, bool &goUp1, float &offsetY1);
    void handleCharacterInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &p2KeyPressed,
                               bool &p2Ready, bool &goDown1, bool &goDown2, bool &goUp1, bool &goUp2, float &offsetY1, float &offsetY2);
    void renderReadyText(GlRender &renderer, bool p1Ready, bool p2Ready, Game &game);
    void resetGame(GlRender &renderer, WorldSystem &worldSystem);
    void updateScores(const Health &h1, const Health &h2);
    int getPlayer1Score() const { return player1Score; }
    int getPlayer2Score() const { return player2Score; }
    void resetScores();

    int getCurrentLevel() { return currentLevel; }
    int getCurrentRound() { return currentRound; }
    void incrementRound() { currentRound++; }
    // SETTINGS MENU
    void renderSettingsMenu(GlRender &renderer);
    bool handleSettingsInput(GlRender &renderer, GLFWwindow *window);


    WorldSystem *getWorldSystem() { return worldSystem; }
    void renderPauseButton(GlRender &renderer);

    bool getShowFPS() const { return showFPS; }
    void setShowFPS(bool show) { showFPS = show; }

    GameState getPreviousState() const { return previousState; }
    bool isArcadeMode() const
    {
        return currentState == GameState::ARCADE_PREFIGHT ||
               currentState == GameState::ARCADE_MENU ||
               (currentState == GameState::PLAYING && previousState == GameState::ARCADE_PREFIGHT);
    }

    bool isVersus = false;
    bool isVersusMode() const
    {
        return isVersus;
    }

    void setVersusMode(bool mode)
    {
        isVersus = mode;
    }

    void renderMatchOver(GlRender &renderer);
    void renderLevelOver(GlRender &renderer);

private:
    GameState currentState;
    GameState previousState = GameState::MENU;
    bool running;
    float loadingProgress;
    bool showHelpDialog;
    bool isPaused;

    struct Button startButton;
    struct Button arcadeButton;
    struct Button helpButton;
    struct Button closeButton;
    struct Button backButton;
    struct Button settingsButton;
    struct Button pauseButton;
    struct Button resumeButton;
    struct Button menuButton;
    struct Button pauseSettingsButton;

    struct Button arcadeLevelOneButton;
    struct Button arcadeLevelTwoButton;
    struct Button arcadeLevelThreeButton;
    struct Button arcadeLevelFourButton;
    struct Button arcadeLevelFiveButton;

    int player1Score = 0;
    int player2Score = 0;
    WorldSystem *worldSystem = nullptr;

    int levelCompleted = 0;
    int currentLevel = 0;
    int currentFrame = 0;
    int currentFinalFrame = 0;

    int currentRound = 1;

    bool rightRelease = true;
    bool leftRelease = true;
    bool spaceRelease = true;
    bool backButtonReleased = true;

    bool showFPS;

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

    bool isBackButtonHovered = false;
    bool isBackButtonPressed = false;

    int currentTutorialPage = 0;

    float koTextTimer = 0.0f;
    int koFlashCount = 0;
    bool showKoText = true;
    const float KO_FLASH_DURATION = 0.5f;
    const int TOTAL_KO_FLASHES = 3;

    float fadeOutTimer = 0.0f;
    float fadeInTimer = 0.0f;
    const float FADE_DURATION = 0.75f;
    bool startedFading = false;
};
