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

    int player1Score = 0;
    int player2Score = 0;
    WorldSystem *worldSystem = nullptr;
};
