#pragma once
#include "../constants.hpp"
#include "../renderer.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../common.hpp"
#include "stb_image/stb_image.h"
#include <fstream>
#include <sstream>
#include "../window.hpp"
#include <string>
#include <GLFW/glfw3.h>

enum class GameState
{
    MENU,
    LOADING,
    PLAYING,
    HELP,
    ROUND_START
};

class Game
{
private:
    GameState currentState;
    bool running;
    float loadingProgress;
    bool showHelpDialog;
    struct Button
    {
        float x, y, width, height;
        const char *text;
    };
    Button startButton;
    Button helpButton;
    Button closeButton;

    int player1Score = 0;
    int player2Score = 0;

public:
    Game();

    void setState(GameState newState);
    GameState getState() const;
    bool isRunning() const;
    void update();
    void render();
    void generateBackground(float val, GlRender &renderer);
    void renderMenu(GlRender &renderer);
    GLuint createShaderProgram(const char *vertexPath, const char *fragmentPath);
    bool handleMenuInput(GLFWwindow *window);
    void handleSettingsButton();
    void renderHelpDialog(GlRender &renderer);
    void handleHelpButton();
    void renderHelpScreen(GlRender &renderer);
    bool handleHelpInput(GLFWwindow *window);
    void resetGame(GlRender &renderer);
    void updateScores(const Health &h1, const Health &h2);
    int getPlayer1Score() const { return player1Score; }
    int getPlayer2Score() const { return player2Score; }
    void resetScores()
    {
        player1Score = 0;
        player2Score = 0;
    }
};
