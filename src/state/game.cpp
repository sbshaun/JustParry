#include "game.hpp"
#include <iostream>
#include "../linearinterp.hpp"

Game::Game() : currentState(GameState::MENU), running(true), loadingProgress(0.0f)
{
    float leftShift = M_WINDOW_WIDTH_PX * (0.05f + 0.015f + 0.02f + 0.03f); // 5% + 1.5% + 2% + 3% of window width
    float upShift = M_WINDOW_HEIGHT_PX * 0.05f;                             // 5% of window height

    // Position start button (moved left and up)
    startButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f - leftShift, // x position
        425.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "START"                                        // button text
    };

    // Position help button below start button
    helpButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f - leftShift, // x position
        525.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "HELP"                                         // button text
    };

    // Add settings button below help button
    newButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f - leftShift, // x position
        625.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "SETTINGS"                                     // renamed from "NEW BUTTON"
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

    // Calculate dimensions for the help screen image
    float helpBoxWidth = 660.0f;
    float helpBoxHeight = 500.0f;
    float helpBoxX = (M_WINDOW_WIDTH_PX - helpBoxWidth) / 2.0f;
    float helpBoxY = (M_WINDOW_HEIGHT_PX - helpBoxHeight) / 2.0f;

    // Render the help screen image in the center
    renderer.renderTexturedQuadScaled(
        renderer.m_helpTexture,
        helpBoxX, helpBoxY,
        helpBoxWidth, helpBoxHeight,
        1.0f);

    // Position and size the close button (make it square)
    closeButton = {
        helpBoxX + helpBoxWidth - 50.0f - 10.0f, // 10px padding from right
        helpBoxY + 10.0f,                        // 10px padding from top
        50.0f,                                   // Square width
        50.0f,                                   // Square height
        "X"};

    // Get mouse position and check hover/press state
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    bool closeHovered = mouseX >= closeButton.x &&
                        mouseX <= closeButton.x + closeButton.width &&
                        mouseY >= closeButton.y &&
                        mouseY <= closeButton.y + closeButton.height;
    bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render close button
    renderer.renderButton(
        closeButton.x, closeButton.y,
        closeButton.width, closeButton.height,
        closeButton.text,
        closeHovered, closePressed,
        glm::vec3(0.7f, 0.1f, 0.1f) // Dark red color
    );
}

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

    // Calculate dimensions for the settings screen image
    float settingsBoxWidth = 660.0f;
    float settingsBoxHeight = 500.0f;
    float settingsBoxX = (M_WINDOW_WIDTH_PX - settingsBoxWidth) / 2.0f;
    float settingsBoxY = (M_WINDOW_HEIGHT_PX - settingsBoxHeight) / 2.0f;

    // Render the settings screen image in the center
    renderer.renderTexturedQuadScaled(
        renderer.m_settingsTexture,
        settingsBoxX, settingsBoxY,
        settingsBoxWidth, settingsBoxHeight,
        1.0f);

    // Position and size the close button (make it square)
    closeButton = {
        settingsBoxX + settingsBoxWidth - 50.0f - 10.0f, // 10px padding from right
        settingsBoxY + 10.0f,                            // 10px padding from top
        50.0f,                                           // Square width
        50.0f,                                           // Square height
        "X"};

    // Get mouse position and check hover/press state
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    bool closeHovered = mouseX >= closeButton.x &&
                        mouseX <= closeButton.x + closeButton.width &&
                        mouseY >= closeButton.y &&
                        mouseY <= closeButton.y + closeButton.height;
    bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render close button
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

void Game::resetGame(GlRender &renderer, WorldSystem &worldSystemRef)
{
    worldSystem = &worldSystemRef;
    registry.clear_all_components();
    worldSystem->init(&renderer);

    // Reset timer
    extern int timer;
    extern const int timer_length;
    timer = timer_length;

    // Reset interpolation variables
    resetInterpVariables();

    // Reset round over animation
    renderer.resetRoundOverAnimation();

    // Reset animation flags using the setter methods
    renderer.setAnimationComplete(false);
    renderer.setExitAnimationStarted(false);

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
    // No cleanup needed here since shaders are managed by the renderer
}
