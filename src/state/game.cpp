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
    // Step 1: Load the image using stb_image
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../../assets/textures/menu 1.png", &width, &height, &nrChannels, 0);
    if (data == nullptr)
    {
        std::cerr << "Failed to load background image!" << std::endl;
        return;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Step 3: Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step 4: Upload image data to the texture
    if (nrChannels == 3)
    {
        // For RGB images
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (nrChannels == 4)
    {
        // For RGBA images
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cerr << "Unsupported image format!" << std::endl;
    }

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Step 5: Free the image data now that it's loaded into OpenGL
    stbi_image_free(data);

    renderer.renderTexturedQuad(texture);
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
