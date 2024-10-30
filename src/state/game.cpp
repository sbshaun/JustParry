#include "game.hpp"
#include <iostream>

Game::Game() : currentState(GameState::MENU), running(true), loadingProgress(0.0f) {}

void Game::setState(GameState newState)
{
    currentState = newState;
}

GameState Game::getState() const
{
    return currentState;
}

bool Game::isRunning() const
{
    return running;
}

void Game::update()
{
    // Existing game update logic
}

void Game::render()
{
    // Existing game render logic
}
void Game::generateBackground(float val, GlRender &renderer)
{
    renderer.renderTexturedQuad(renderer.m_backgroundTexture);
}

#include <GLFW/glfw3.h>

bool isPressed(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void Game::renderMenu(GlRender &renderer)
{
    // Check if Enter key is pressed
    if (isPressed(GLFW_KEY_ENTER))
    {
        loadingProgress += 0.2f;
        if (loadingProgress >= 1.0f)
        {
            std::cout << "Loading: " << static_cast<int>(loadingProgress * 100) << "%\n";
            setState(GameState::PLAYING);
        }
    }
}
