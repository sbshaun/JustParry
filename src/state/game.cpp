#include "game.hpp"
#include <iostream>
#include "../linearinterp.hpp"

Game::Game() : currentState(GameState::MENU), running(true), loadingProgress(0.0f)
{
    // Position button in the upper middle of the screen
    startButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f, // x position
        500.0f,                            // y position
        260.0f,                            // width
        80.0f,                             // height
        "START"                            // button text
    };

    // Position help button below start button
    helpButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 210.0f, // x position
        600.0f,                            // y position
        260.0f,                            // width
        80.0f,                             // height
        "HELP"                             // button text
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background
    renderer.renderTexturedQuad(renderer.m_backgroundTexture);

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

    bool startPressed = startHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool helpPressed = helpHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render main menu buttons
    renderer.renderButton(startButton.x, startButton.y,
                          startButton.width, startButton.height,
                          startButton.text,
                          startHovered, startPressed);

    renderer.renderButton(helpButton.x, helpButton.y,
                          helpButton.width, helpButton.height,
                          helpButton.text,
                          helpHovered, helpPressed);

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
        renderer.m_backgroundTexture,
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
        renderer.m_backgroundTexture,
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

void Game::resetGame(GlRender &renderer)
{
    Fighters current_char1 = registry.players.get(renderer.m_player1).current_char;
    Fighters current_char2 = registry.players.get(renderer.m_player2).current_char;
    FighterConfig config1 = FighterManager::getFighterConfig(current_char1);
	FighterConfig config2 = FighterManager::getFighterConfig(current_char2);

    // Reset timer
    extern int timer;
    extern const int timer_length;
    timer = timer_length;

    // Reset interpolation variables
    resetInterpVariables();

    // Reset player healths
    Health &h1 = registry.healths.get(renderer.m_player1);
    Health &h2 = registry.healths.get(renderer.m_player2);
    h1.currentHealth = h1.maxHealth;
    h2.currentHealth = h2.maxHealth;

    // Reset player positions (off screen)
    Motion &m1 = registry.motions.get(renderer.m_player1);
    Motion &m2 = registry.motions.get(renderer.m_player2);

    // Set initial positions off-screen and reset scales
    m1.position = glm::vec2(-1.0f, FLOOR_Y + config1.NDC_HEIGHT); // Start from top-left
    m2.position = glm::vec2(1.0f, FLOOR_Y + config2.NDC_HEIGHT);  // Start from top-right

    // Reset scales to initial small size
    m1.scale = glm::vec2(0.1f, 0.1f);
    m2.scale = glm::vec2(0.1f, 0.1f);

    // Reset player states
    registry.playerCurrentStates.get(renderer.m_player1).currentState = PlayerState::IDLE;
    registry.playerCurrentStates.get(renderer.m_player2).currentState = PlayerState::IDLE;

    // Reset player inputs
    registry.playerInputs.get(renderer.m_player1) = PlayerInput();
    registry.playerInputs.get(renderer.m_player2) = PlayerInput();

    // Set state to ROUND_START to trigger entry animation
    setState(GameState::ROUND_START);
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
