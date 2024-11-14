#pragma once
#include "../constants.hpp"
#include "../renderer.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../common.hpp"
#include "stb_image/stb_image.h"
#include <fstream>
#include <sstream>
#include "../window.hpp"
#include "../world/world_system.hpp"
#include "../world/world_init.hpp"
#include <string>
#include <GLFW/glfw3.h>

enum class GameState
{
    MENU,
    LOADING,
    PLAYING,
    HELP,
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
    void render(GlRender &renderer);
    void generateBackground(float val, GlRender &renderer);
    void renderMenu(GlRender &renderer);
    bool handleMenuInput(GLFWwindow *window, GlRender &renderer);
    void handleSettingsButton();
    void handleHelpButton();
    void renderHelpScreen(GlRender &renderer);
    bool handleHelpInput(GLFWwindow *window);
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
    Button helpButton;
    Button closeButton;
    Button newButton;
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

    void cleanupButtons();
};
