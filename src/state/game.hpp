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

enum class GameState
{
    MENU,
    LOADING,
    PLAYING
};

class Game
{
private:
    GameState currentState;
    bool running;
    float loadingProgress;

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
};
