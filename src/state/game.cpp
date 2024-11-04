#include "game.hpp"
#include <iostream>
#include "../linearinterp.hpp"

Game::Game() : currentState(GameState::MENU), running(true), loadingProgress(0.0f)
{
    // Position start button (moved down by additional 100px)
    startButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f, // x position
        425.0f,                            // y position (was 325)
        260.0f,                            // width
        80.0f,                             // height
        "START"                            // button text
    };

    // Position help button below start button
    helpButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f, // x position
        525.0f,                            // y position (was 425)
        260.0f,                            // width
        80.0f,                             // height
        "HELP"                             // button text
    };

    // Add settings button below help button
    newButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f, // x position
        625.0f,                            // y position (was 525)
        260.0f,                            // width
        80.0f,                             // height
        "SETTINGS"                         // renamed from "NEW BUTTON"
    };

    // Position close button for help dialog
    closeButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 200.0f,  // Dialog will be 400px wide
        M_WINDOW_HEIGHT_PX / 2.0f - 150.0f, // Dialog will be 300px tall
        40.0f,                              // width
        40.0f,                              // height
        "X"                                 // button text
    };

    std::cout << "Buttons initialized" << std::endl;
}

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
    renderer.renderTexturedQuad(renderer.m_menuTexture);
}

#include <GLFW/glfw3.h>

bool isPressed(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void Game::renderMenu(GlRender &renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background with full brightness
    renderer.renderTexturedQuadScaled(
        renderer.m_menuTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness for main menu
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    // Get mouse position for hover effects
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check button states
    bool startHovered = mouseX >= startButton.x && mouseX <= startButton.x + startButton.width &&
                        mouseY >= startButton.y && mouseY <= startButton.y + startButton.height;

    bool helpHovered = mouseX >= helpButton.x && mouseX <= helpButton.x + helpButton.width &&
                       mouseY >= helpButton.y && mouseY <= helpButton.y + helpButton.height;

    bool newButtonHovered = mouseX >= newButton.x && mouseX <= newButton.x + newButton.width &&
                            mouseY >= newButton.y && mouseY <= newButton.y + newButton.height;

    bool startPressed = startHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool helpPressed = helpHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool newButtonPressed = newButtonHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render all buttons
    renderer.renderButton(startButton.x, startButton.y,
                          startButton.width, startButton.height,
                          startButton.text,
                          startHovered, startPressed);

    renderer.renderButton(helpButton.x, helpButton.y,
                          helpButton.width, helpButton.height,
                          helpButton.text,
                          helpHovered, helpPressed);

    renderer.renderButton(newButton.x, newButton.y,
                          newButton.width, newButton.height,
                          newButton.text,
                          newButtonHovered, newButtonPressed);

    // If help dialog is shown, render it
    if (currentState == GameState::HELP)
    {
        renderHelpScreen(renderer);
    }

    glDepthFunc(GL_LESS);
}

void Game::renderHelpScreen(GlRender &renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // First render the full background (darkened)
    renderer.renderTexturedQuadScaled(
        renderer.m_menuTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        0.3f // Darkness factor
    );

    // Calculate dimensions for the smaller centered background
    float helpBoxWidth = 600.0f;  // Fixed width
    float helpBoxHeight = 400.0f; // Fixed height
    float helpBoxX = (M_WINDOW_WIDTH_PX - helpBoxWidth) / 2.0f;
    float helpBoxY = (M_WINDOW_HEIGHT_PX - helpBoxHeight) / 2.0f;

    // Render the smaller background in the center
    renderer.renderTexturedQuadScaled(
        renderer.m_menuTexture,
        helpBoxX, helpBoxY,
        helpBoxWidth, helpBoxHeight,
        1.0f // No darkening for the center image
    );

    // Get mouse position for close button hover effect
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Position the close button in the top-right corner of the help box
    closeButton.x = helpBoxX + helpBoxWidth - closeButton.width - 10; // 10px padding
    closeButton.y = helpBoxY + 10;                                    // 10px padding from top

    // Check close button state
    bool closeHovered =
        mouseX >= closeButton.x &&
        mouseX <= closeButton.x + closeButton.width &&
        mouseY >= closeButton.y &&
        mouseY <= closeButton.y + closeButton.height;
    bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render close/back button
    renderer.renderButton(
        closeButton.x, closeButton.y,
        closeButton.width, closeButton.height,
        closeButton.text,
        closeHovered, closePressed,
        glm::vec3(0.7f, 0.1f, 0.1f) // Dark red color
    );
}

bool Game::handleMenuInput(GLFWwindow *window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (currentState == GameState::HELP)
    {
        // When help dialog is shown, only handle close button
        bool mouseOverClose =
            mouseX >= closeButton.x &&
            mouseX <= closeButton.x + closeButton.width &&
            mouseY >= closeButton.y &&
            mouseY <= closeButton.y + closeButton.height;

        if (mouseOverClose && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            setState(GameState::MENU);
            return false;
        }
        return false; // Ignore other buttons when help dialog is shown
    }

    // Normal menu button handling
    bool mouseOverStart =
        mouseX >= startButton.x &&
        mouseX <= startButton.x + startButton.width &&
        mouseY >= startButton.y &&
        mouseY <= startButton.y + startButton.height;

    bool mouseOverHelp =
        mouseX >= helpButton.x &&
        mouseX <= helpButton.x + helpButton.width &&
        mouseY >= helpButton.y &&
        mouseY <= helpButton.y + helpButton.height;

    bool mouseOverNewButton = mouseX >= newButton.x && mouseX <= newButton.x + newButton.width &&
                              mouseY >= newButton.y && mouseY <= newButton.y + newButton.height;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (mouseOverStart)
        {
            resetScores(); // Reset scores when starting new game
            std::cout << "Start button clicked!" << std::endl;
            return true;
        }
        else if (mouseOverHelp)
        {
            handleHelpButton();
        }
        else if (mouseOverNewButton)
        {
            handleSettingsButton();
        }
    }

    return false;
}

void Game::handleHelpButton()
{
    setState(GameState::HELP);
    std::cout << "Help screen opened!" << std::endl;
}

bool Game::handleHelpInput(GLFWwindow *window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check if back/close button is clicked
    bool mouseOverClose =
        mouseX >= closeButton.x &&
        mouseX <= closeButton.x + closeButton.width &&
        mouseY >= closeButton.y &&
        mouseY <= closeButton.y + closeButton.height;

    if (mouseOverClose && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        return true; // Return to menu
    }

    return false; // Stay in help state
}

void Game::resetGame(GlRender &renderer, WorldSystem &worldSystem)
{   
    registry.clear_all_components();

    worldSystem.init(&renderer);
    
    // Reset timer
    extern int timer;
    extern const int timer_length;
    timer = timer_length;

    // Reset interpolation variables
    resetInterpVariables();

    isLoading = true;
    setState(GameState::PLAYING);
}

void Game::updateScores(const Health &h1, const Health &h2)
{
    if (h1.currentHealth <= 0)
    {
        player2Score++;
    }
    else if (h2.currentHealth <= 0)
    {
        player1Score++;
    }
    else
    {
        // Timer ran out, give point to player with higher health
        if (h1.currentHealth > h2.currentHealth)
        {
            player1Score++;
        }
        else if (h2.currentHealth > h1.currentHealth)
        {
            player2Score++;
        }
        else
        {
            // If health is equal, give point to player 1
            player1Score++;
        }
    }
}

static Shader *settingsShader = nullptr; // Static shader for settings screen

void Game::renderSettingsScreen(GlRender &renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // First render the full background (darkened)
    renderer.renderTexturedQuadScaled(
        renderer.m_menuTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        0.3f // Darkness factor
    );

    // Calculate dimensions for settings box (centered)
    float settingsBoxWidth = M_WINDOW_WIDTH_PX * 0.7f;
    float settingsBoxHeight = M_WINDOW_HEIGHT_PX * 0.8f;
    float settingsBoxX = (M_WINDOW_WIDTH_PX - settingsBoxWidth) / 2.0f;
    float settingsBoxY = (M_WINDOW_HEIGHT_PX - settingsBoxHeight) / 2.0f;

    GLuint VAO = 0, VBO = 0;

    try
    {
        // Initialize shader if needed
        if (!settingsShader)
        {
            settingsShader = new Shader("menu");
        }

        // Create vertices for white background (two triangles)
        float vertices[] = {
            // Position (x, y, z)            // Texture coords (u, v)
            -0.7f, 0.8f, 0.0f, 0.0f, 1.0f,  // Top-left
            -0.7f, -0.8f, 0.0f, 0.0f, 0.0f, // Bottom-left
            0.7f, -0.8f, 0.0f, 1.0f, 0.0f,  // Bottom-right

            0.7f, -0.8f, 0.0f, 1.0f, 0.0f, // Bottom-right
            0.7f, 0.8f, 0.0f, 1.0f, 1.0f,  // Top-right
            -0.7f, 0.8f, 0.0f, 0.0f, 1.0f  // Top-left
        };

        // Create and bind VAO/VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use shader and set color
        settingsShader->use();
        settingsShader->setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // White color
        settingsShader->setFloat("brightness", 1.0f);                  // Full brightness

        // Draw the white box
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Position close button in top-right corner with padding
        closeButton.width = 50.0f;
        closeButton.height = 50.0f;
        closeButton.x = settingsBoxX + settingsBoxWidth - closeButton.width - 20.0f;
        closeButton.y = settingsBoxY + 20.0f;

        // Get mouse position and handle close button
        GLFWwindow *window = glfwGetCurrentContext();
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        bool closeHovered = mouseX >= closeButton.x &&
                            mouseX <= closeButton.x + closeButton.width &&
                            mouseY >= closeButton.y &&
                            mouseY <= closeButton.y + closeButton.height;
        bool closePressed = closeHovered &&
                            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Render close button
        renderer.renderButton(
            closeButton.x, closeButton.y,
            closeButton.width, closeButton.height,
            closeButton.text,
            closeHovered, closePressed,
            glm::vec3(0.7f, 0.1f, 0.1f) // Dark red color
        );
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in renderSettingsScreen: " << e.what() << std::endl;
    }

    // Cleanup OpenGL resources
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
    }

    // Restore OpenGL state
    glDisable(GL_BLEND);
}

void Game::handleSettingsButton()
{
    setState(GameState::SETTINGS);
    std::cout << "Settings screen opened!" << std::endl;
}

bool Game::handleSettingsInput(GLFWwindow *window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check if close button is clicked
    bool mouseOverClose = mouseX >= closeButton.x &&
                          mouseX <= closeButton.x + closeButton.width &&
                          mouseY >= closeButton.y &&
                          mouseY <= closeButton.y + closeButton.height;

    if (mouseOverClose && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        setState(GameState::MENU);
        return true; // Return to menu
    }

    return false; // Stay in settings state
}

Game::~Game()
{
    // Clean up settings shader
    if (settingsShader)
    {
        delete settingsShader;
        settingsShader = nullptr;
    }
}
