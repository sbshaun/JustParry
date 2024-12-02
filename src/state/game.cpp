#include "game.hpp"
#include "../input_system/input_mapping.hpp"
#include "../interp/linearinterp.hpp"
#include "settings_menu.hpp"

Fighters Game::p1SelectedCharacter = Fighters::BIRDMAN;
Fighters Game::p2SelectedCharacter = Fighters::BIRDMAN;

Game::Game() : currentState(GameState::INIT), running(true), loadingProgress(0.0f),
               // Initialize these from settings
               showFPS(Settings::windowSettings.show_fps)
{
    float leftShift = M_WINDOW_WIDTH_PX * (0.05f + 0.015f + 0.02f + 0.03f); // 5% + 1.5% + 2% + 3% of window width
    float upShift = M_WINDOW_HEIGHT_PX * 0.09f;                             // 5% of window height

    // Position start button (moved left and up)
    startButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        375.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "VERSUS"                                       // button text
    };

    arcadeButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        475.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "ARCADE"                                       // button text
    };

    arcadeLevelOneButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift + 75.f, // x position
        235.0f - upShift + 50.f,                              // y position
        400.0f,                                               // width
        70.0f,                                                // height
        "Level One"                                           // button text
    };

    arcadeLevelTwoButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift + 75.f, // x position
        325.0f - upShift + 50.f,                              // y position
        400.0f,                                               // width
        70.0f,                                                // height
        "Level Two"                                           // button text
    };
    arcadeLevelThreeButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift + 75.f, // x position
        415.0f - upShift + 50.f,                              // y position
        400.0f,                                               // width
        70.0f,                                                // height
        "Level Three"                                         // button text
    };
    arcadeLevelFourButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift + 75.f, // x position
        505.0f - upShift + 50.f,                              // y position
        400.0f,                                               // width
        70.0f,                                                // height
        "Level Four"                                          // button text
    };
    arcadeLevelFiveButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift + 75.f, // x position
        595.0f - upShift + 50.f,                              // y position
        400.0f,                                               // width
        70.0f,                                                // height
        "- ENDING -"                                          // button text
    };
    // Position help button below start button
    helpButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        575.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "TUTORIAL"                                     // button text
    };

    // Add settings button below help button
    settingsButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        675.0f - upShift,                              // y position
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

    // Position close button for help dialog
    backButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 470.0f,  // Dialog will be 400px wide
        M_WINDOW_HEIGHT_PX / 2.0f - 355.0f, // Dialog will be 300px tall
        85.0f,                              // width
        85.0f,                              // height
        "<-"                                // button text
    };

    // Position pause button in top left corner
    pauseButton = {
        20.0f, // x position (10px from left edge)
        20.0f, // y position (10px from top)
        65.0f, // width
        60.0f, // height
        "="    // pause symbol
    };

    // // Initialize resume button (centered)
    resumeButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 130.0f, // x position
        M_WINDOW_HEIGHT_PX / 2.0f - 60.0f, // y position
        260.0f,                            // width
        80.0f,                             // height
        "RESUME"                           // button text
    };

    // Initialize settings button (below resume)
    pauseSettingsButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 130.0f, // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 40.0f, // y position (below resume)
        260.0f,                            // width
        80.0f,                             // height
        "SETTINGS"                         // button text
    };

    // Initialize menu button (below settings)
    menuButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 130.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 140.0f, // y position (below settings)
        260.0f,                             // width
        80.0f,                              // height
        "MAIN MENU"                         // button text
    };

    std::cout << "Buttons initialized" << std::endl;
}

void Game::setState(GameState newState)
{
    // Don't store previous state for certain transitions
    if (currentState != GameState::SETTINGS &&
        currentState != GameState::SETTINGS_EXIT)
    {
        previousState = currentState;
    }

    // Special handling for settings transitions
    if (newState == GameState::SETTINGS)
    {
        // If we're not already in settings-related states, store the previous state
        if (currentState != GameState::SETTINGS &&
            currentState != GameState::SETTINGS_EXIT)
        {
            previousState = currentState;
        }
    }

    // If transitioning from settings to menu or gameplay
    if (currentState == GameState::SETTINGS &&
        (newState == GameState::MENU || newState == GameState::PLAYING))
    {
        // Reload settings and reinitialize input handlers
        Settings::loadSettings();
        if (worldSystem != nullptr)
        {
            worldSystem->initInputHandlers();
        }
    }
    // When transitioning from character select to gameplay
    else if (currentState == GameState::CHARACTER_SELECT &&
             newState == GameState::ROUND_START)
    {
        // Reload settings and reinitialize input handlers
        Settings::loadSettings();
        if (worldSystem != nullptr)
        {
            worldSystem->initInputHandlers();
            std::cout << "Reinitialized controls for gameplay" << std::endl;
        }
    }

    // Debug output to track state transitions
    std::cout << "State transition: " << static_cast<int>(currentState)
              << " -> " << static_cast<int>(newState)
              << " (Previous: " << static_cast<int>(previousState) << ")" << std::endl;

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

void Game::updateArcadeLevel()
{
    if (currentLevel > levelCompleted)
    {
        levelCompleted = currentLevel;
    }
}

void Game::loadArcadeState()
{
    FILE *file = nullptr;
    file = fopen(saves_path("arcadeLevelState.txt").c_str(), "r");
    if (file != nullptr)
    {
        fscanf(file, "%d", &this->levelCompleted);
        fclose(file);
        printf("Integer read from file: %d\n", this->levelCompleted);
    }
    else
    {
        printf("Unable to open file for reading.\n");
    }
}
void Game::saveCurrentState()
{
    FILE *file = nullptr;
    file = fopen(saves_path("arcadeLevelState.txt").c_str(), "w");
    if (file != nullptr)
    {
        fprintf(file, "%d", this->levelCompleted);
        fclose(file);
        printf("Saved arcade level state.\n");
    }
    else
    {
        printf("Unable to open file for writing.\n");
    }
}
void Game::render(GlRender &renderer)
{
    // Existing game render logic

    // Add this line to render the pause button during gameplay
    if (currentState == GameState::PLAYING)
    {
        renderPauseButton(renderer);
    }
}
void Game::generateBackground(float val, GlRender &renderer)
{
    renderer.renderTexturedQuad(renderer.m_menuTexture);
}

#include <GLFW/glfw3.h>

// bool isPressed(int key) //REDUNDANT with isKeyPressed in input_utils.hpp
// {
//     GLFWwindow *window = glfwGetCurrentContext();
//     return glfwGetKey(window, key) == GLFW_PRESS;
// }

void Game::renderCharacterSelect(GlRender &renderer, float offset1, float offset2, bool p1, bool p2)
{
    // First clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    backButton.x = 25.0f;
    backButton.y = 15.0f;
    backButton.width = 80.0f;
    backButton.height = 70.0f;

    // Render background
    renderer.renderTexturedQuadScaled(
        renderer.m_characterSelectTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness
    );

    // Add back button
    renderer.renderButton(
        backButton.x, backButton.y,          // x, y position
        backButton.width, backButton.height, // width, height
        "<-",                                // text
        isBackButtonHovered,                 // Add this member variable to Game class
        isBackButtonPressed                  // Add this member variable to Game class
    );

    // std::cout << registry.players.get(renderer.m_player1).color << std::endl;

    int p1Color = registry.players.get(renderer.m_player1).color;
    int p2Color = registry.players.get(renderer.m_player2).color;

    // Rest of the character select rendering...
    if (p1Color == 0) {
        renderer.renderTexturedQuadScaled(
            p1 ? renderer.m_character1_ready : renderer.m_character1,
            200.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p1Color == 1) {
        renderer.renderTexturedQuadScaled(
            p1 ? renderer.m_character1_ready_red : renderer.m_character1_red,
            200.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p1Color == 2) {
        renderer.renderTexturedQuadScaled(
            p1 ? renderer.m_character1_ready_green : renderer.m_character1_green,
            200.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p1Color == 3) {
        renderer.renderTexturedQuadScaled(
            p1 ? renderer.m_character1_ready_blue : renderer.m_character1_blue,
            200.f, 360.f,
            225, 275,
            1.0f
        );
    }

    if (p2Color == 0) {
        renderer.renderTexturedQuadScaled(
            p2 ? renderer.m_character1_flip_ready : renderer.m_character1_flip,
            600.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p2Color == 1) {
        renderer.renderTexturedQuadScaled(
            p2 ? renderer.m_character1_flip_ready_red : renderer.m_character1_flip_red,
            600.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p2Color == 2) {
        renderer.renderTexturedQuadScaled(
            p2 ? renderer.m_character1_flip_ready_green : renderer.m_character1_flip_green,
            600.f, 360.f,
            225, 275,
            1.0f
        );
    }
    else if (p2Color == 3) {
        renderer.renderTexturedQuadScaled(
            p2 ? renderer.m_character1_flip_ready_blue : renderer.m_character1_flip_blue,
            600.f, 360.f,
            225, 275,
            1.0f
        );
    }

    renderer.renderText(Settings::getKeyName(Settings::p1Controls.punch), 360.f, 270.0f + offset1, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText(Settings::getKeyName(Settings::p2Controls.punch), 645.f, 270.0f + offset2, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));

    renderer.renderSelectorTriangleP1(400, 245 + offset1, 30, 30, p1);
    renderer.renderSelectorTriangleP2(605, 245 + offset2, 30, 30, p2);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glDepthFunc(GL_LESS);
}

void Game::startArcade(GlRender& renderer)
{
    Player& p1 = registry.players.get(renderer.m_player1);

    p1.current_char = BIRDMAN;
}

void Game::renderArcadePrefight(GlRender &renderer, float offset1, bool p1)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background
    renderer.renderTexturedQuadScaled(
        renderer.m_characterSelectTextureArcade,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_character1,
        290, 360.0f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    // renderer.renderTexturedQuadScaled(
    //     renderer.m_character1_flip,
    //     625.f, 360.0f,
    //     225, 275,
    //     1.0f // Full brightness for main menu
    // );

    // Get mouse position for hover effects
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Add back button
    backButton.x = 25.0f;
    backButton.y = 15.0f;
    backButton.width = 80.0f;
    backButton.height = 70.0f;

    // Check button states
    bool backHovered = mouseX >= backButton.x && mouseX <= backButton.x + backButton.width &&
                       mouseY >= backButton.y && mouseY <= backButton.y + backButton.height;

    // static bool backButtonPressed = false;

    if (backHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (backButtonReleased)
        {
            handleBackButton();
        }
        backButtonReleased = false;
    }
    else
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            backButtonReleased = true;
        }
    }

    // render back button
    renderer.renderButton(
        backButton.x, backButton.y,          // x, y position
        backButton.width, backButton.height, // width, height
        "<-",                                // text
        backHovered,                         // Add this member variable to Game class
        !backButtonReleased                  // Add this member variable to Game class
    );

    renderer.renderSelectorTriangleP1(560, 255 + offset1, 30, 30, p1);
    // renderer.renderSelectorTriangleP2(600, 245, 30, 30, true);

    renderer.renderText(Settings::getKeyName(Settings::p1Controls.punch), 525.f, 285.0f + offset1, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));
    // renderer.renderText(Settings::getKeyName(Settings::p2Controls.punch), 645.f, 270.0f, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glDepthFunc(GL_LESS);
}

void Game::handleArcadePrefightInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready,
                                      bool &goDown1, bool &goUp1, float &offsetY1)
{
    if (glfwGetKey(glWindow.window, Settings::p1Controls.punch) == GLFW_PRESS)
    {
        if (!p1KeyPressed) // Check if the key was not pressed before
        {
            WorldSystem::playMenuConfirmSound();
            p1Ready = !p1Ready;  // Toggle p1Ready
            p1KeyPressed = true; // Mark the key as pressed
            if (std::to_string(p1Ready) == "0")
            {
                std::cout << "Player 1 Not Ready" << std::endl;
            }
            else
            {
                std::cout << "Player 1 Ready" << std::endl;
            }
        }
    }
    else
    {
        p1KeyPressed = false; // Reset when the key is released
    }

    if (!p1Ready)
    {
        if (glfwGetKey(glWindow.window, Settings::p1Controls.down) == GLFW_PRESS)
        {
            if (!goDown1)
            {
                WorldSystem::playMenuSelectSound();
                goDown1 = true;
                if (offsetY1 < 300.f)
                {
                    offsetY1 += 150.f;
                }
                else
                {
                    offsetY1 = 0;
                }
            }
        }
        else
        {
            goDown1 = false;
        }

        if (glfwGetKey(glWindow.window, Settings::p1Controls.up) == GLFW_PRESS)
        {
            if (!goUp1)
            {
                WorldSystem::playMenuSelectSound();
                goUp1 = true;
                if (offsetY1 > 0.0f)
                {
                    offsetY1 -= 150.f;
                }
                else
                {
                    offsetY1 = 0;
                }
            }
        }
        else
        {
            goUp1 = false;
        }
    }
}

void Game::handleCharacterInputs(GlRender& renderer, GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &p2KeyPressed,
                                 bool &p2Ready, bool &goDown1, bool &goDown2, bool &goUp1, bool &goUp2,
                                 float &offsetY1, float &offsetY2)
{
    double mouseX, mouseY;
    glfwGetCursorPos(glWindow.window, &mouseX, &mouseY);

    // Check back button hover
    isBackButtonHovered = SettingsMenu::isHovered(mouseX, mouseY, backButton);

    // Check for back button click
    if (isBackButtonHovered && glfwGetMouseButton(glWindow.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        isBackButtonPressed = true;
    }
    else if (isBackButtonPressed && glfwGetMouseButton(glWindow.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Reset character select state
        p1Ready = false;
        p2Ready = false;
        offsetY1 = 0.0f;
        offsetY2 = 0.0f;

        // Return to menu
        setState(GameState::MENU);
        isBackButtonPressed = false;
    }
    else if (!isBackButtonHovered)
    {
        isBackButtonPressed = false;
    }

    if (glfwGetKey(glWindow.window, Settings::p1Controls.punch) == GLFW_PRESS)
    {
        if (!p1KeyPressed) // Check if the key was not pressed before
        {
            WorldSystem::playMenuConfirmSound();
            p1Ready = !p1Ready;  // Toggle p1Ready
            p1KeyPressed = true; // Mark the key as pressed
            if (std::to_string(p1Ready) == "0")
            {
                std::cout << "Player 1 Not Ready" << std::endl;
            }
            else
            {
                std::cout << "Player 1 Ready" << std::endl;
            }
        }
    }
    else
    {
        p1KeyPressed = false; // Reset when the key is released
    }

    if (glfwGetKey(glWindow.window, Settings::p2Controls.punch) == GLFW_PRESS)
    {
        if (!p2KeyPressed) // Check if the key was not pressed before
        {
            WorldSystem::playMenuConfirmSound();
            p2Ready = !p2Ready;  // Toggle p2Ready
            p2KeyPressed = true; // Mark the key as pressed
            if (std::to_string(p2Ready) == "0")
            {
                std::cout << "Player 2 Not Ready" << std::endl;
            }
            else
            {
                std::cout << "Player 2 Ready" << std::endl;
            }
        }
    }
    else
    {
        p2KeyPressed = false; // Reset when the key is released
    }

    if (glfwGetKey(glWindow.window, Settings::p1Controls.left) == GLFW_PRESS)
    {
        if (!p1LeftPressed) // Check if the key was not pressed before
        {
            p1LeftPressed = true;
            Player& p1 = registry.players.get(renderer.m_player1);

            if (p1.color == 0) {
                p1.color = 3;
            }
            else {
                p1.color -= 1;
            }
        }
    }
    else
    {
        p1LeftPressed = false; // Reset when the key is released
    }
    if (glfwGetKey(glWindow.window, Settings::p1Controls.right) == GLFW_PRESS)
    {
        if (!p1RightPressed) // Check if the key was not pressed before
        {
            p1RightPressed = true;
            Player& p1 = registry.players.get(renderer.m_player1);

            if (p1.color == 3) {
                p1.color = 0;
            }
            else {
                p1.color += 1;
            }
        }
    }
    else
    {
        p1RightPressed = false; // Reset when the key is released
    }

    if (glfwGetKey(glWindow.window, Settings::p2Controls.left) == GLFW_PRESS)
    {
        if (!p2LeftPressed) // Check if the key was not pressed before
        {
            p2LeftPressed = true;
            Player& p2 = registry.players.get(renderer.m_player2);

            if (p2.color == 0) {
                p2.color = 3;
            }
            else {
                p2.color -= 1;
            }
        }
    }
    else
    {
        p2LeftPressed = false; // Reset when the key is released
    }

    if (glfwGetKey(glWindow.window, Settings::p2Controls.right) == GLFW_PRESS)
    {
        if (!p2RightPressed) // Check if the key was not pressed before
        {
            p2RightPressed = true;
            Player& p2 = registry.players.get(renderer.m_player2);

            if (p2.color == 3) {
                p2.color = 0;
            }
            else {
                p2.color += 1;
            }
        }
    }
    else
    {
        p2RightPressed = false; // Reset when the key is released
    }

    if (!p1Ready)
    {
        if (glfwGetKey(glWindow.window, Settings::p1Controls.down) == GLFW_PRESS)
        {
            if (!goDown1)
            {
                WorldSystem::playMenuSelectSound();
                
                if (p1SelectedCharacterIndex == 1) {
                    p1SelectedCharacterIndex = 0;
                }
                else {
                    p1SelectedCharacterIndex = 1;
                }

                goDown1 = true;
                if (offsetY1 < 300.f)
                {
                    offsetY1 += 150.f;
                }
                else
                {
                    offsetY1 = 0;
                }
            }
        }
        else
        {
            goDown1 = false;
        }

        if (glfwGetKey(glWindow.window, Settings::p1Controls.up) == GLFW_PRESS)
        {
            if (!goUp1)
            {
                WorldSystem::playMenuSelectSound();

                if (p1SelectedCharacterIndex == 1) {
                    p1SelectedCharacterIndex = 0;
                }
                else {
                    p1SelectedCharacterIndex = 1;
                }

                goUp1 = true;
                if (offsetY1 > 0.0f)
                {
                    offsetY1 -= 150.f;
                }
                else
                {
                    offsetY1 = 0;
                }
            }
        }
        else
        {
            goUp1 = false;
        }
    }

    if (!p2Ready)
    {
        if (glfwGetKey(glWindow.window, Settings::p2Controls.down) == GLFW_PRESS)
        {
            if (!goDown2)
            {
                WorldSystem::playMenuSelectSound();

                if (p2SelectedCharacterIndex == 1) {
                    p2SelectedCharacterIndex = 0;
                }
                else {
                    p2SelectedCharacterIndex = 1;
                }

                goDown2 = true;
                if (offsetY2 < 300.f)
                {
                    offsetY2 += 150.f;
                }
                else
                {
                    offsetY2 = 0;
                }
            }
        }
        else
        {
            goDown2 = false;
        }

        if (glfwGetKey(glWindow.window, Settings::p2Controls.up) == GLFW_PRESS)
        {
            if (!goUp2)
            {
                WorldSystem::playMenuSelectSound();

                if (p2SelectedCharacterIndex == 1) {
                    p2SelectedCharacterIndex = 0;
                }
                else {
                    p2SelectedCharacterIndex = 1;
                }

                goUp2 = true;
                if (offsetY2 > 0.0f)
                {
                    offsetY2 -= 150.f;
                }
                else
                {
                    offsetY2 = 0;
                }
            }
        }
        else
        {
            goUp2 = false;
        }
    }
}

void Game::renderReadyText(GlRender &renderer, bool p1Ready, bool p2Ready, Game &game)
{
    if (!isArcadeMode())
    {
        if (p1Ready)
        {
            renderer.renderText("READY", 210, 350, 0.25f, glm::vec3(.0f, 1.0f, 0.0f));
        }
        else
        {
            renderer.renderText("NOT READY", 180, 350, 0.25f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (p2Ready)
        {
            renderer.renderText("READY", 740, 350, 0.25f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else
        {
            renderer.renderText("NOT READY", 725, 350, 0.25f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (p1Ready && p2Ready)
        {
            setP1SelectedCharacter();
            setP2SelectedCharacter();
            renderer.renderText("PRESS SPACE", 410, 650, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("TO START!", 435, 700, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                WorldSystem::playGameCountDownSound();
                game.setState(GameState::ROUND_START);
            }
        }
    }
    else
    {
        if (p1Ready)
        {
            renderer.renderText("READY", 315, 390, 0.25f, glm::vec3(.0f, 1.0f, 0.0f));
        }
        else
        {
            renderer.renderText("NOT READY", 295, 390, 0.25f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (p1Ready)
        {
            renderer.renderText("PRESS SPACE", 260, 260, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("TO START!", 285, 310, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                game.setState(GameState::ARCADE_MENU);
            }
        }
    }
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

    bool arcadeHovered = mouseX >= arcadeButton.x && mouseX <= arcadeButton.x + arcadeButton.width &&
                         mouseY >= arcadeButton.y && mouseY <= arcadeButton.y + arcadeButton.height;

    bool helpHovered = mouseX >= helpButton.x && mouseX <= helpButton.x + helpButton.width &&
                       mouseY >= helpButton.y && mouseY <= helpButton.y + helpButton.height;

    bool settingsHovered = mouseX >= settingsButton.x && mouseX <= settingsButton.x + settingsButton.width &&
                           mouseY >= settingsButton.y && mouseY <= settingsButton.y + settingsButton.height;

    bool startPressed = startHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool helpPressed = helpHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool arcadePressed = arcadeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool settingsPressed = settingsHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render all buttons
    renderer.renderButton(startButton.x, startButton.y,
                          startButton.width, startButton.height,
                          startButton.text,
                          startHovered, startPressed);

    renderer.renderButton(arcadeButton.x, arcadeButton.y,
                          arcadeButton.width, arcadeButton.height,
                          arcadeButton.text,
                          arcadeHovered, arcadePressed);

    renderer.renderButton(helpButton.x, helpButton.y,
                          helpButton.width, helpButton.height,
                          helpButton.text,
                          helpHovered, helpPressed);

    renderer.renderButton(settingsButton.x, settingsButton.y,
                          settingsButton.width, settingsButton.height,
                          settingsButton.text,
                          settingsHovered, settingsPressed);

    // If help dialog is shown, render it
	// SIDDH: Not sure why this is here. Commenting out for now.
    /*if (currentState == GameState::HELP)
    {
        renderHelpScreen(renderer);
    }*/

    glDepthFunc(GL_LESS);
}

void Game::renderArcadeMenu(GlRender &renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background with full brightness
    renderer.renderTexturedQuadScaled(
        renderer.m_arcadeMenuTexture,
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

    // Add back button
    backButton.x = 25.0f;
    backButton.y = 15.0f;
    backButton.width = 80.0f;
    backButton.height = 70.0f;

    // Check button states
    bool backHovered = mouseX >= backButton.x && mouseX <= backButton.x + backButton.width &&
                       mouseY >= backButton.y && mouseY <= backButton.y + backButton.height;

    bool levelOneHovered = mouseX >= arcadeLevelOneButton.x && mouseX <= arcadeLevelOneButton.x + arcadeLevelOneButton.width &&
                           mouseY >= arcadeLevelOneButton.y && mouseY <= arcadeLevelOneButton.y + arcadeLevelOneButton.height;

    bool levelTwoHovered = mouseX >= arcadeLevelTwoButton.x && mouseX <= arcadeLevelTwoButton.x + arcadeLevelTwoButton.width &&
                           mouseY >= arcadeLevelTwoButton.y && mouseY <= arcadeLevelTwoButton.y + arcadeLevelTwoButton.height;

    bool levelThreeHovered = mouseX >= arcadeLevelThreeButton.x && mouseX <= arcadeLevelThreeButton.x + arcadeLevelThreeButton.width &&
                             mouseY >= arcadeLevelThreeButton.y && mouseY <= arcadeLevelThreeButton.y + arcadeLevelThreeButton.height;

    bool levelFourHovered = mouseX >= arcadeLevelFourButton.x && mouseX <= arcadeLevelFourButton.x + arcadeLevelFourButton.width &&
                            mouseY >= arcadeLevelFourButton.y && mouseY <= arcadeLevelFourButton.y + arcadeLevelFourButton.height;

    bool levelFiveHovered = mouseX >= arcadeLevelFiveButton.x && mouseX <= arcadeLevelFiveButton.x + arcadeLevelFiveButton.width &&
                            mouseY >= arcadeLevelFiveButton.y && mouseY <= arcadeLevelFiveButton.y + arcadeLevelFiveButton.height;

    bool backPressed = backHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    renderer.renderButton(
        backButton.x, backButton.y,          // x, y position
        backButton.width, backButton.height, // width, height
        "<-",                                // text
        backHovered,                         // Add this member variable to Game class
        backPressed                          // Add this member variable to Game class
    );

    renderer.renderText("SELECT LEVEL", 200.f, 135.0f, 1.f, glm::vec3(0x9e / 255.0f, 0x9b / 255.0f, 0x9e / 255.0f));

    bool isLevelOneCompleted = (levelCompleted >= 1);
    bool isLevelTwoCompleted = (levelCompleted >= 2);
    bool isLevelThreeCompleted = (levelCompleted >= 3);
    bool isLevelFourCompleted = (levelCompleted >= 4);
    bool isLevelFiveCompleted = (levelCompleted == 5);

    bool LevelOnePressed = levelOneHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelTwoPressed = isLevelOneCompleted && levelThreeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelThreePressed = isLevelTwoCompleted && levelTwoHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelFourPressed = isLevelThreeCompleted && levelFourHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelFivePressed = isLevelFourCompleted && levelFiveHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    renderer.renderButton(arcadeLevelOneButton.x, arcadeLevelOneButton.y,
                          arcadeLevelOneButton.width, arcadeLevelOneButton.height,
                          arcadeLevelOneButton.text,
                          levelOneHovered, LevelOnePressed,
                          glm::vec3(0.8F, 0.8F, 0.8F));
    renderer.renderButton(arcadeLevelTwoButton.x, arcadeLevelTwoButton.y,
                          arcadeLevelTwoButton.width, arcadeLevelTwoButton.height,
                          arcadeLevelTwoButton.text,
                          isLevelOneCompleted ? levelTwoHovered : false,
                          isLevelOneCompleted ? LevelTwoPressed : false,
                          isLevelOneCompleted ? glm::vec3(0.8F, 0.8F, 0.8F) : glm::vec3(-1.0f, -1.0f, -1.0f));
    renderer.renderButton(arcadeLevelThreeButton.x, arcadeLevelThreeButton.y,
                          arcadeLevelThreeButton.width, arcadeLevelThreeButton.height,
                          arcadeLevelThreeButton.text,
                          isLevelTwoCompleted ? levelThreeHovered : false,
                          isLevelTwoCompleted ? LevelThreePressed : false,
                          isLevelTwoCompleted ? glm::vec3(0.8F, 0.8F, 0.8F) : glm::vec3(-1.0f, -1.0f, -1.0f));
    renderer.renderButton(arcadeLevelFourButton.x, arcadeLevelFourButton.y,
                          arcadeLevelFourButton.width, arcadeLevelFourButton.height,
                          arcadeLevelFourButton.text,
                          isLevelThreeCompleted ? levelFourHovered : false,
                          isLevelThreeCompleted ? LevelFourPressed : false,
                          isLevelThreeCompleted ? glm::vec3(0.8F, 0.8F, 0.8F) : glm::vec3(-1.0f, -1.0f, -1.0f));
    renderer.renderButton(arcadeLevelFiveButton.x, arcadeLevelFiveButton.y,
                          arcadeLevelFiveButton.width, arcadeLevelFiveButton.height,
                          arcadeLevelFiveButton.text,
                          isLevelFourCompleted ? levelFiveHovered : false,
                          isLevelFourCompleted ? LevelFivePressed : false,
                          isLevelFourCompleted ? glm::vec3(0.8F, 0.8F, 0.8F) : glm::vec3(-1.0f, -1.0f, -1.0f));
    static bool oneDone = false;
    static bool twoDone = false;
    static bool threeDone = false;
    static bool fourDone = false;
    static bool fiveDone = false;

    if (levelCompleted >= 0)
    {
        renderer.renderSelectorTriangleP1(110.f, 235.f, 40.f, 40.f, isLevelOneCompleted);
    }
    if (levelCompleted >= 1)
    {
        renderer.renderSelectorTriangleP1(110.f, 325.f, 40.f, 40.f, isLevelTwoCompleted);
    }
    if (levelCompleted >= 2)
    {
        renderer.renderSelectorTriangleP1(110.f, 415.f, 40.f, 40.f, isLevelThreeCompleted);
    }
    if (levelCompleted >= 3)
    {
        renderer.renderSelectorTriangleP1(110.f, 505.f, 40.f, 40.f, isLevelFourCompleted);
    }

    if (levelCompleted >= 4)
    {
        renderer.renderSelectorTriangleP1(110.f, 595.f, 40.f, 40.f, isLevelFiveCompleted);
    }

    glDepthFunc(GL_LESS);
}

void Game::renderArcadeStory(GlRender &renderer)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render background with full brightness
    renderer.renderTexturedQuadScaled(
        renderer.m_arcadeMenuTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness for main menu
    );

    // Calculate dimensions for the story images
    float storyBoxWidth = 800.0f;
    float storyBoxHeight = 600.0f;
    float storyBoxX = (M_WINDOW_WIDTH_PX - storyBoxWidth) / 2.0f;
    float storyBoxY = (M_WINDOW_HEIGHT_PX - storyBoxHeight) / 2.0f;

    // LEVEL ONE
    // TODO: MAKE INTO A TUTORIAL LEVEL
    if (currentLevel == 1)
    {
        switch (currentFrame)
        {
        case 1:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_1,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 2:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_2,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 3:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_3,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 4:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_4,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 5:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_5,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 6:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_6,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 7: 
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_1_7,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        }
    }
    // LEVEL TWO
    if (currentLevel == 2) {
        switch (currentFrame) {
        case 1:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_1,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 2:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_2,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 3:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_3,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 4:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_4,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 5:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_5,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 6:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_2_6,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        }
    }
    // LEVEL THREE
    if (currentLevel == 3) {
        switch (currentFrame) {
        case 1:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_1,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 2:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_2,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 3:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_3,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 4:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_4,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 5:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_5,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 6:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_6,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 7:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_7,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 8:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_8,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 9:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_9,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 10:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_3_10,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        }
    }
    // LEVEL FOUR
    if (currentLevel == 4) {
        switch (currentFrame) {
        case 1:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_1,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 2:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_2,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 3:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_3,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 4:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_4,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 5:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_5,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 6:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_6,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 7:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_7,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 8:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_8,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 9:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_4_9,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;

        }
    }
    // LEVEL FIVE
    if (currentLevel == 5) {
        switch (currentFrame) {
        case 1:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_1,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 2:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_2,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 3:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_3,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 4:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_4,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 5:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_5,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 6:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_6,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 7:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_7,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 8:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_8,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;
        case 9:
            renderer.renderTexturedQuadScaled(
                renderer.bird_Story_E_9,
                storyBoxX, storyBoxY,
                storyBoxWidth, storyBoxHeight,
                1.0f);
            break;

        }
    }

    renderer.renderText("SPACE ->", 850.f, 750.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
}

void Game::renderHelpScreen(GlRender &renderer, bool &botEnabled)
{
    // Calculate dimensions for the help screen image
    glEnable(GL_BLEND);
    float helpBoxWidth = 300.0f;
    float helpBoxHeight = 300.0f;
    float helpBoxX = (M_WINDOW_WIDTH_PX + 350.f) / 2.0f;
    float helpBoxY = (M_WINDOW_HEIGHT_PX) / 2.0f - 325.f;

    // Position and size the close button (make it square)
    closeButton = {
        30.f,   // 10px padding from right
        22.f,   // 10px padding from top
        120.0f, // Square width
        55.0f,  // Square height
        ""};

    // Get mouse position and check hover/press state
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    bool closeHovered = mouseX >= closeButton.x &&
                        mouseX <= closeButton.x + closeButton.width &&
                        mouseY >= closeButton.y &&
                        mouseY <= closeButton.y + closeButton.height;
    bool closePressed = closeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    Button previousButton = {
        helpBoxX,         // x position
        helpBoxY + 315.f, // y position
        145.0f,           // width
        50.0f,            // height
        "BACK"};

    Button nextButton = {
        helpBoxX + 150.f, // x position
        helpBoxY + 315.f, // y position
        145.0f,           // width
        50.0f,            // height
        "NEXT"};

    bool previousHovered = mouseX >= previousButton.x &&
                           mouseX <= previousButton.x + previousButton.width &&
                           mouseY >= previousButton.y &&
                           mouseY <= previousButton.y + previousButton.height;
    bool previousPressed = previousHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    bool nextHovered = mouseX >= nextButton.x &&
                       mouseX <= nextButton.x + nextButton.width &&
                       mouseY >= nextButton.y &&
                       mouseY <= nextButton.y + nextButton.height;
    bool nextPressed = nextHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    static bool prevButtonWasPressed = false;
    static bool nextButtonWasPressed = false;

    // Handle button clicks with debouncing
    if (previousPressed && !prevButtonWasPressed)
    {
        if (currentTutorialPage > 0)
        {
            currentTutorialPage--;
            WorldSystem::playMenuSelectSound();
        }
    }
    if (nextPressed && !nextButtonWasPressed)
    {
        if (currentTutorialPage < 2)
        { // 0,1,2 = 3 pages total
            currentTutorialPage++;
            WorldSystem::playMenuSelectSound();
        }
    }
    prevButtonWasPressed = previousPressed;
    nextButtonWasPressed = nextPressed;

    // Render close button
    renderer.renderButton(
        closeButton.x, closeButton.y,
        closeButton.width, closeButton.height,
        closeButton.text,
        closeHovered, closePressed,
        glm::vec3(0.7f, 0.1f, 0.1f));
    renderer.renderText("EXIT", 55.f, 60.f, 0.35f, glm::vec3(0.0f, 0.0f, 0.0f));

    // Render navigation buttons with disabled states
    glm::vec3 backButtonColor = (currentTutorialPage == 0) ? glm::vec3(0.2f, 0.2f, 0.2f) : // Much darker when disabled
                                    glm::vec3(0.9f, 0.9f, 0.9f);                           // Normal color

    glm::vec3 nextButtonColor = (currentTutorialPage == 2) ? glm::vec3(0.2f, 0.2f, 0.2f) : // Much darker when disabled
                                    glm::vec3(0.9f, 0.9f, 0.9f);                           // Normal color

    // Only allow button clicks if they're not at the limits
    if (previousPressed && !prevButtonWasPressed)
    {
        if (currentTutorialPage > 0)
        {
            currentTutorialPage--;
            WorldSystem::playMenuSelectSound();
        }
    }
    if (nextPressed && !nextButtonWasPressed)
    {
        if (currentTutorialPage < 2)
        {
            currentTutorialPage++;
            WorldSystem::playMenuSelectSound();
        }
    }
    prevButtonWasPressed = previousPressed;
    nextButtonWasPressed = nextPressed;

    // Render navigation buttons with disabled states
    renderer.renderButton(
        previousButton.x, previousButton.y,
        previousButton.width, previousButton.height,
        previousButton.text,
        currentTutorialPage > 0 && previousHovered,
        currentTutorialPage > 0 && previousPressed,
        backButtonColor);

    renderer.renderButton(
        nextButton.x, nextButton.y,
        nextButton.width, nextButton.height,
        nextButton.text,
        currentTutorialPage < 2 && nextHovered,
        currentTutorialPage < 2 && nextPressed,
        nextButtonColor);

    // Render the appropriate help screen image based on currentTutorialPage
    GLuint currentTexture;
  
    switch (currentTutorialPage) {
        case 0:
            currentTexture = renderer.m_helpTexture1;
            renderer.renderTexturedQuadScaled(
                currentTexture,
                helpBoxX, helpBoxY,
                helpBoxWidth, helpBoxHeight,
                1.0f
            );
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.left), 865.f, 280.f, 0.35f, glm::vec3(1.f, 1.f, 1.f));
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.right), 865.f, 318.f, 0.35f, glm::vec3(1.f, 1.f, 1.f));
            break;
        case 1:
            currentTexture = renderer.m_helpTexture2;
            renderer.renderTexturedQuadScaled(
                currentTexture,
                helpBoxX, helpBoxY,
                helpBoxWidth, helpBoxHeight,
                1.0f
            );
            renderer.renderText("ATTACK USING", 735.f, 90.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("PUNCH AND KICK", 720.f, 115.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("PUNCH", 750.f, 295.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.punch), 865.f, 295.f, 0.3f, glm::vec3(1.f, 1.f, 1.f));
            renderer.renderText("KICK", 782.f, 330.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.down), 865.f, 330.f, 0.3f, glm::vec3(1.f, 1.f, 1.f));
            renderer.renderText(" + ", 880.f, 330.f, 0.3f, glm::vec3(1.f, 1.f, 1.f));
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.punch), 905.f, 330.f, 0.3f, glm::vec3(1.f, 1.f, 1.f));
            break;
        case 2:
            botEnabled = true;
            currentTexture = renderer.m_helpTexture3;
            renderer.renderTexturedQuadScaled(
                currentTexture,
                helpBoxX, helpBoxY,
                helpBoxWidth, helpBoxHeight,
                1.0f
            );
            renderer.renderText("PARRY TO STUN", 760.f, 85.f, 0.25f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("YOUR OPPONENT AND", 720.f, 108.f, 0.25f, glm::vec3(0.0f, 0.0f, 0.0f));
             renderer.renderText("COUNTER ATTACK", 750.f, 130.f, 0.25f, glm::vec3(0.0f, 0.0f, 0.0f));

            renderer.renderText("PARRY", 770.f, 330.f, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText(Settings::getKeyName(Settings::p1Controls.parry), 880.f, 327.f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
            break;
    }

    // Add page indicator text
    renderer.renderText(std::to_string(currentTutorialPage + 1) + "/3",
                        helpBoxX, helpBoxY - 10.f, 0.25f, glm::vec3(0.0f, 0.0f, 0.0f));
}

bool Game::handleMenuInput(GLFWwindow *window, GlRender &renderer)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    currentLevel = 0;

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
            this->setState(GameState::MENU);
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

    bool mouseOverArcade =
        mouseX >= arcadeButton.x &&
        mouseX <= arcadeButton.x + arcadeButton.width &&
        mouseY >= arcadeButton.y &&
        mouseY <= arcadeButton.y + arcadeButton.height;

    bool mouseOverHelp =
        mouseX >= helpButton.x &&
        mouseX <= helpButton.x + helpButton.width &&
        mouseY >= helpButton.y &&
        mouseY <= helpButton.y + helpButton.height;

    bool mouseOverSettings =
        mouseX >= settingsButton.x &&
        mouseX <= settingsButton.x + settingsButton.width &&
        mouseY >= settingsButton.y &&
        mouseY <= settingsButton.y + settingsButton.height;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (mouseOverStart)
        {
            this->resetScores();

            // Reset interpolation state
            extern bool isLoading;
            isLoading = true;
            resetInterpVariables();

            std::cout << "Start button clicked!" << std::endl;
            this->setState(GameState::ROUND_START);
            return true;
        }
        else if (mouseOverArcade)
        {
            this->handleArcadeButton();
        }
        else if (mouseOverHelp)
        {
            placePlayersOnScreen(renderer);
            this->handleHelpButton();
        }
        else if (mouseOverSettings)
        {
            this->handleSettingsButton();
        }
    }

    return false;
}

void Game::handleArcadeButton()
{
    this->setState(GameState::ARCADE_PREFIGHT);
    std::cout << "Arcade menu screen opened!" << std::endl;
}

bool Game::handleArcadeMenuInput(GLFWwindow *window)
{
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check button states
    bool mouseOverBack =
        mouseX >= backButton.x &&
        mouseX <= backButton.x + backButton.width &&
        mouseY >= backButton.y &&
        mouseY <= backButton.y + backButton.height;

    bool mouseOverLevelOne =
        mouseX >= arcadeLevelOneButton.x &&
        mouseX <= arcadeLevelOneButton.x + arcadeLevelOneButton.width &&
        mouseY >= arcadeLevelOneButton.y &&
        mouseY <= arcadeLevelOneButton.y + arcadeLevelOneButton.height;

    bool mouseOverLevelTwo =
        mouseX >= arcadeLevelTwoButton.x &&
        mouseX <= arcadeLevelTwoButton.x + arcadeLevelTwoButton.width &&
        mouseY >= arcadeLevelTwoButton.y &&
        mouseY <= arcadeLevelTwoButton.y + arcadeLevelTwoButton.height;

    bool mouseOverLevelThree =
        mouseX >= arcadeLevelThreeButton.x &&
        mouseX <= arcadeLevelThreeButton.x + arcadeLevelThreeButton.width &&
        mouseY >= arcadeLevelThreeButton.y &&
        mouseY <= arcadeLevelThreeButton.y + arcadeLevelThreeButton.height;

    bool mouseOverLevelFour =
        mouseX >= arcadeLevelFourButton.x &&
        mouseX <= arcadeLevelFourButton.x + arcadeLevelFourButton.width &&
        mouseY >= arcadeLevelFourButton.y &&
        mouseY <= arcadeLevelFourButton.y + arcadeLevelFourButton.height;

    bool mouseOverLevelFive =
        mouseX >= arcadeLevelFiveButton.x &&
        mouseX <= arcadeLevelFiveButton.x + arcadeLevelFiveButton.width &&
        mouseY >= arcadeLevelFiveButton.y &&
        mouseY <= arcadeLevelFiveButton.y + arcadeLevelFiveButton.height;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (mouseOverBack)
        {
            if (!backButtonReleased)
            {
                handleBackButton();
            }
            backButtonReleased = false;
        }
        if (mouseOverLevelOne)
        {
            currentLevel = 1;
            currentFrame = 1;
            currentFinalFrame = 7;
            return true;
        }
        else if (mouseOverLevelTwo && levelCompleted >= 1)
        {
            currentLevel = 2;
            currentFrame = 1;
            currentFinalFrame = 6;
            return true;
        }
        else if (mouseOverLevelThree && levelCompleted >= 2)
        {
            currentLevel = 3;
            currentFrame = 1;
            currentFinalFrame = 10;
            return true;
        }
        else if (mouseOverLevelFour && levelCompleted >= 3)
        {
            currentLevel = 4;
            currentFrame = 1;
            currentFinalFrame = 9;
            return true;
        }
        else if (mouseOverLevelFive && levelCompleted >= 4)
        {
            currentLevel = 5;
            currentFrame = 1;
            currentFinalFrame = 9;
            return true;
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        backButtonReleased = true;
    }

    return false;
}

bool Game::handleArcadeStoryInput(GLFWwindow *window)
{
    if (glfwGetKey(glfwGetCurrentContext(), Settings::p2Controls.right) == GLFW_PRESS && rightRelease == true)
    {
        rightRelease = false;
        if (currentFrame < currentFinalFrame)
        {
            currentFrame++;
        }
    }

    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS && spaceRelease == true)
    {
        spaceRelease = false;
        if (currentFrame == currentFinalFrame) {
            if (currentLevel == 5) {
                this->setState(GameState::ARCADE_MENU);
                this->levelCompleted = 5;
            }
            else {
                return true;
            }
        }
        if (currentFrame < currentFinalFrame) {
            currentFrame++;
        }
    }

    if (glfwGetKey(glfwGetCurrentContext(), Settings::p2Controls.right) == GLFW_RELEASE)
    {
        rightRelease = true;
    }

    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spaceRelease = true;
    }
    return false;
}

void Game::handleBackButton()
{
    this->setState(GameState::MENU);
    std::cout << "Going to Menu Screen" << std::endl;
}

void Game::handleHelpButton()
{
    currentTutorialPage = 0; // Reset to first page when opening help
    this->setState(GameState::HELP);
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
        currentTutorialPage = 0; // Reset page counter when closing help
        return true;             // Return to menu
    }

    return false; // Stay in help state
}

void Game::resetGame(GlRender &renderer, WorldSystem &worldSystem)
{
    this->worldSystem = &worldSystem;

    int p1Color = registry.players.get(renderer.m_player1).color;
    int p2Color = registry.players.get(renderer.m_player2).color;

    // Clear all components first
    registry.clear_all_components();

    // Reinitialize world system
    worldSystem.init(&renderer);

    registry.players.get(renderer.m_player1).color = p1Color;
    registry.players.get(renderer.m_player2).color = p2Color;

    // Reset timer
    extern int timer;
    timer = timer_length;

    // Reset interpolation variables
    resetInterpVariables();

    // Reset round over animation
    renderer.resetRoundOverAnimation();

    // Reset animation flags using the setter methods
    renderer.setAnimationComplete(false);
    renderer.setExitAnimationStarted(false);

    registry.players.get(renderer.m_player1).parries = 0;
    registry.players.get(renderer.m_player1).perfectParries = 0;

    isLoading = true;
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
        // Timer ran out, give point based on health
        if (h1.currentHealth > h2.currentHealth)
        {
            player1Score++;
        }
        else if (h2.currentHealth > h1.currentHealth)
        {
            player2Score++;
        }
        // If health is equal (tie), don't give points to anyone
    }
}

void Game::handleSettingsButton()
{
    this->setState(GameState::SETTINGS);
    std::cout << "Settings screen opened!" << std::endl;
}

void Game::renderSettingsMenu(GlRender &renderer)
{
    SettingsMenu::initializeButtons();
    SettingsMenu::renderSettingsScreen(renderer, *this);
}

bool Game::handleSettingsInput(GlRender &renderer, GLFWwindow *window)
{
    // handle player 1 and 2 control changes
    SettingsMenu::handleControlsSettingsInput(window);

    if (SettingsMenu::shouldClose())
    {
        SettingsMenu::cleanup(); // cleanup
        return true;             // Close settings
    }
    return false; // Stay in settings state
}

Game::~Game()
{
    // Clean up world system pointer
    if (worldSystem)
    {
        worldSystem = nullptr; // We don't delete it since it's managed elsewhere
    }

    // Clean up button resources
    cleanupButtons();

    // Note: The renderer and its textures are cleaned up in the renderer's destructor
    std::cout << "Game resources cleaned up." << std::endl;
}

void Game::cleanupButtons()
{
    // Reset each button structure to its default state
    startButton = {};
    helpButton = {};
    settingsButton = {};
    closeButton = {};
    pauseButton = {};
    resumeButton = {};
    menuButton = {};
    pauseSettingsButton = {};
}

void Game::attemptPause()
{
    if (currentState == GameState::PLAYING || currentState == GameState::ROUND_START)
    {
        WorldSystem::stopAllSounds();
        setState(GameState::PAUSED); // Use setState instead of directly modifying currentState
    }
}

void Game::renderPauseButton(GlRender &renderer)
{
    // Show pause button only during gameplay, and when not loading
    if ((currentState != GameState::PLAYING &&
         currentState != GameState::PAUSED &&
         currentState != GameState::ROUND_START) ||
        isLoading) // Add isLoading check here
        return;

    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    bool pauseHovered = mouseX >= pauseButton.x &&
                        mouseX <= pauseButton.x + pauseButton.width &&
                        mouseY >= pauseButton.y &&
                        mouseY <= pauseButton.y + pauseButton.height;

    bool pausePressed = pauseHovered &&
                        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Check for pause button click with debouncing
    static bool wasPressed = false;
    if (pausePressed && !wasPressed)
    {
        attemptPause();
    }
    wasPressed = pausePressed;

    // Render pause button
    renderer.renderButton(
        pauseButton.x - 5.f, pauseButton.y - 5.f,
        pauseButton.width + 5.f, pauseButton.height + 5.f,
        "",
        pauseHovered, false,
        glm::vec3(0.4f, 0.4f, 0.4f));
    renderer.renderText("||", pauseButton.x + 20.f, pauseButton.y + 42.5f, 0.48f, glm::vec3(0.0f, 0.0f, 0.0f));

    // If game is paused, render pause menu
    if (currentState == GameState::PAUSED)
    {
        // First render a black overlay with reduced opacity
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            0.0f, // Set brightness to 0 to make it black
            0.5f  // 50% opacity for the black overlay
        );

        // Calculate dimensions for the pause menu background
        float bgWidth = 500.0f;
        float bgHeight = 500.0f;
        float bgX = (M_WINDOW_WIDTH_PX - bgWidth) / 2.0f;
        float bgY = (M_WINDOW_HEIGHT_PX - bgHeight) / 2.0f;

        // Render the pause menu background
        renderer.renderTexturedQuadScaled(
            renderer.m_pauseMenuTexture,
            bgX, bgY,
            bgWidth, bgHeight,
            1.0f, // Full brightness
            1.0f  // Full opacity
        );

        // Get mouse position for button hover states
        bool resumeHovered = mouseX >= resumeButton.x &&
                             mouseX <= resumeButton.x + resumeButton.width &&
                             mouseY >= resumeButton.y &&
                             mouseY <= resumeButton.y + resumeButton.height;

        bool menuHovered = mouseX >= menuButton.x &&
                           mouseX <= menuButton.x + menuButton.width &&
                           mouseY >= menuButton.y &&
                           mouseY <= menuButton.y + menuButton.height;

        bool settingsHovered = mouseX >= pauseSettingsButton.x &&
                               mouseX <= pauseSettingsButton.x + pauseSettingsButton.width &&
                               mouseY >= pauseSettingsButton.y &&
                               mouseY <= pauseSettingsButton.y + pauseSettingsButton.height;

        bool resumePressed = resumeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool menuPressed = menuHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool settingsPressed = settingsHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Handle button clicks
        static bool wasResumePressed = false;
        static bool wasMenuPressed = false;
        static bool wasSettingsPressed = false;

        if (resumePressed && !wasResumePressed)
        {
            setState(GameState::PLAYING);
        }
        else if (menuPressed && !wasMenuPressed)
        {
            // Proper cleanup before going to menu
            WorldSystem::stopBackgroundMusic();
            WorldSystem::stopAllSounds();

            // Clear all components first
            registry.clear_all_components();

            // Reset game state
            resetScores();
            resetInterpVariables();

            // Set state to INIT to ensure proper reinitialization
            setState(GameState::INIT);

            // Reset timer
            extern int timer;
            timer = timer_length;
        }
        else if (settingsPressed && !wasSettingsPressed)
        {
            setState(GameState::SETTINGS);
        }

        // Render pause menu buttons
        renderer.renderButton(resumeButton.x, resumeButton.y,
                              resumeButton.width, resumeButton.height,
                              "RESUME", resumeHovered, resumePressed);

        renderer.renderButton(menuButton.x, menuButton.y,
                              menuButton.width, menuButton.height,
                              "MAIN MENU", menuHovered, menuPressed);

        renderer.renderButton(pauseSettingsButton.x, pauseSettingsButton.y,
                              pauseSettingsButton.width, pauseSettingsButton.height,
                              "SETTINGS", settingsHovered, settingsPressed);

        wasResumePressed = resumePressed;
        wasMenuPressed = menuPressed;
        wasSettingsPressed = settingsPressed;
    }
}

void Game::renderMatchOver(GlRender &renderer)
{
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    // First show the flashing K.O. text
    if (koFlashCount < TOTAL_KO_FLASHES)
    {
        // Update the timer using deltaTime
        koTextTimer += deltaTime;

        // Toggle text visibility every KO_FLASH_DURATION seconds
        if (koTextTimer >= KO_FLASH_DURATION)
        {
            koTextTimer = 0.0f;
            showKoText = !showKoText;
            if (!showKoText)
            {
                koFlashCount++;
            }
        }

        // Render the previous frame in the background
        renderer.render();
        renderer.renderUI(timer);

        // Render K.O. text if it should be visible
        if (showKoText)
        {
            // Center the text on screen
            float centerX = M_WINDOW_WIDTH_PX / 2.0f - 100.0f;
            float centerY = M_WINDOW_HEIGHT_PX / 2.0f - 50.0f;
            renderer.renderText("K.O!", centerX - 40.f, centerY, 2.0f, glm::vec3(0.3f, 0.0f, 0.0f));
        }
        return;
    }

    // Start fade transitions after K.O. animation
    if (!startedFading)
    {
        startedFading = true;
        fadeOutTimer = 0.0f;
        fadeInTimer = 0.0f;
    }

    // Handle fade out of the game screen
    if (fadeOutTimer < FADE_DURATION)
    {
        fadeOutTimer += deltaTime;
        float alpha = std::min(fadeOutTimer / FADE_DURATION, 1.0f);

        // Render the game screen
        renderer.render();
        renderer.renderUI(timer);

        // Render black overlay with increasing opacity
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            0.0f, // Black
            alpha // Increasing opacity
        );
        return;
    }

    // Handle fade in of the match over screen
    if (fadeInTimer < FADE_DURATION)
    {
        fadeInTimer += deltaTime;
        float alpha = std::min(fadeInTimer / FADE_DURATION, 1.0f);

        // Render the appropriate match over screen with increasing opacity
        if (player1Score == 2)
        {
            renderer.renderTexturedQuadScaled(
                renderer.m_matchOverP1Texture,
                0, 0,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f,
                alpha);
        }
        else if (player2Score == 2)
        {
            renderer.renderTexturedQuadScaled(
                renderer.m_matchOverP2Texture,
                0, 0,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f,
                alpha);
        }
        return;
    }

    // After all animations, show the full match over screen with buttons
    if (player1Score == 2)
    {
        renderer.renderTexturedQuadScaled(
            renderer.m_matchOverP1Texture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }
    else if (player2Score == 2)
    {
        renderer.renderTexturedQuadScaled(
            renderer.m_matchOverP2Texture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }

    // Position buttons in the center bottom of the screen
    Button restartButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 280.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 275.0f, // y position
        260.0f,                             // width
        80.0f,                              // height
        "REMATCH"                           // button text
    };

    Button menuButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 20.0f,   // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 275.0f, // y position
        260.0f,                             // width
        80.0f,                              // height
        "MAIN MENU"                         // button text
    };

    // Get mouse position for hover effects
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check button states
    bool restartHovered = mouseX >= restartButton.x && mouseX <= restartButton.x + restartButton.width &&
                          mouseY >= restartButton.y && mouseY <= restartButton.y + restartButton.height;

    bool menuHovered = mouseX >= menuButton.x && mouseX <= menuButton.x + menuButton.width &&
                       mouseY >= menuButton.y && mouseY <= menuButton.y + menuButton.height;

    bool restartPressed = restartHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool menuPressed = menuHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    static bool wasRestartPressed = false;
    static bool wasMenuPressed = false;

    // Handle button clicks with debouncing
    if (restartPressed && !wasRestartPressed)
    {
        // Reset scores and start new match
        resetScores();
        resetGame(renderer, *worldSystem);
        setState(GameState::ROUND_START);
        WorldSystem::stopAllSounds();
        WorldSystem::stopBackgroundMusic();
        WorldSystem::playGameCountDownSound();
    }
    else if (menuPressed && !wasMenuPressed)
    {
        // Reset scores and return to menu
        resetScores();
        resetGame(renderer, *worldSystem);
        setState(GameState::MENU);
        WorldSystem::stopAllSounds();
        WorldSystem::stopBackgroundMusic();
    }

    wasRestartPressed = restartPressed;
    wasMenuPressed = menuPressed;

    // Render the buttons
    renderer.renderButton(
        restartButton.x, restartButton.y,
        restartButton.width, restartButton.height,
        restartButton.text,
        restartHovered, restartPressed);

    renderer.renderButton(
        menuButton.x, menuButton.y,
        menuButton.width, menuButton.height,
        menuButton.text,
        menuHovered, menuPressed);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (player1Score == 2)
    {
        renderer.renderText("P1 WINS!", 240, 130, 1.5f, glm::vec3(0.05f, 0.05f, 0.05f));
    }
    else
    {
        renderer.renderText("P2 WINS!", 225, 130, 1.5f, glm::vec3(0.05f, 0.05f, 0.05f));
    }


    renderer.renderText("PARRIES :" , 325, 250, 0.5f, glm::vec3(0.05f, 0.05f, 0.05f));
    renderer.renderText("JUST PARRIES : ", 200, 320, 0.5f, glm::vec3(0.05f, 0.05f, 0.05f));
}

void Game::renderLevelOver(GlRender &renderer)
{
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    // First show the flashing K.O. text
    if (koFlashCount < TOTAL_KO_FLASHES)
    {
        // Update the timer using deltaTime
        koTextTimer += deltaTime;

        // Toggle text visibility every KO_FLASH_DURATION seconds
        if (koTextTimer >= KO_FLASH_DURATION)
        {
            koTextTimer = 0.0f;
            showKoText = !showKoText;
            if (!showKoText)
            {
                koFlashCount++;
            }
        }

        // Render the previous frame in the background
        renderer.render();
        renderer.renderUI(timer);

        // Render K.O. text if it should be visible
        if (showKoText)
        {
            // Center the text on screen
            float centerX = M_WINDOW_WIDTH_PX / 2.0f - 100.0f;
            float centerY = M_WINDOW_HEIGHT_PX / 2.0f - 50.0f;
            renderer.renderText("K.O!", centerX - 40.f, centerY, 2.0f, glm::vec3(0.3f, 0.0f, 0.0f));
        }
        return;
    }

    // Start fade transitions after K.O. animation
    if (!startedFading)
    {
        startedFading = true;
        fadeOutTimer = 0.0f;
        fadeInTimer = 0.0f;
    }

    // Handle fade out of the game screen
    if (fadeOutTimer < FADE_DURATION)
    {
        fadeOutTimer += deltaTime;
        float alpha = std::min(fadeOutTimer / FADE_DURATION, 1.0f);

        // Render the game screen
        renderer.render();
        renderer.renderUI(timer);

        // Render black overlay with increasing opacity
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            0.0f, // Black
            alpha // Increasing opacity
        );
        return;
    }

    // Handle fade in of the match over screen
    if (fadeInTimer < FADE_DURATION)
    {
        fadeInTimer += deltaTime;
        float alpha = std::min(fadeInTimer / FADE_DURATION, 1.0f);

        // Render the appropriate match over screen with increasing opacity
        if (player1Score == 2)
        {
            renderer.renderTexturedQuadScaled(
                renderer.m_levelWonTexture,
                0, 0,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f,
                alpha);
        }
        else if (player2Score == 2)
        {
            renderer.renderTexturedQuadScaled(
                renderer.m_levelLostTexture,
                0, 0,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f,
                alpha);
        }
        return;
    }

    // After all animations, show the full match over screen with buttons
    if (player1Score == 2)
    {
        renderer.renderTexturedQuadScaled(
            renderer.m_levelWonTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }
    else if (player2Score == 2)
    {
        renderer.renderTexturedQuadScaled(
            renderer.m_levelLostTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }

    // Position buttons in the center bottom of the screen
    Button restartButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 400.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 275.0f, // y position
        260.0f,                             // width
        80.0f,                              // height
        "REMATCH"                           // button text
    };

    Button selectLevelButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 125.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 275.0f, // y position
        275.0f,                             // width
        80.0f,                              // height
        "LEVEL MENU"                        // button text
    };

    Button menuButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 160.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f + 275.0f, // y position
        260.0f,                             // width
        80.0f,                              // height
        "MAIN MENU"                         // button text
    };

    // Get mouse position for hover effects
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check button states
    bool restartHovered = mouseX >= restartButton.x && mouseX <= restartButton.x + restartButton.width &&
                          mouseY >= restartButton.y && mouseY <= restartButton.y + restartButton.height;

    bool selectLevelHovered = mouseX >= selectLevelButton.x && mouseX <= selectLevelButton.x + selectLevelButton.width &&
                              mouseY >= selectLevelButton.y && mouseY <= selectLevelButton.y + selectLevelButton.height;

    bool menuHovered = mouseX >= menuButton.x && mouseX <= menuButton.x + menuButton.width &&
                       mouseY >= menuButton.y && mouseY <= menuButton.y + menuButton.height;

    bool restartPressed = restartHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool selectLevelPressed = selectLevelHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool menuPressed = menuHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    static bool wasRestartPressed = false;
    static bool wasSelectLevelPressed = false;
    static bool wasMenuPressed = false;

    // Handle button clicks with debouncing
    if (restartPressed && !wasRestartPressed)
    {
        // Reset scores and start new match
        resetScores();
        resetGame(renderer, *worldSystem);
        setState(GameState::ROUND_START);
        WorldSystem::stopAllSounds();
        WorldSystem::stopBackgroundMusic();
        WorldSystem::playGameCountDownSound();
    }
    else if (selectLevelPressed && !wasSelectLevelPressed)
    {
        // Return to arcade menu
        resetScores();
        resetGame(renderer, *worldSystem);
        setState(GameState::ARCADE_MENU);
        WorldSystem::stopAllSounds();
        WorldSystem::stopBackgroundMusic();
    }
    else if (menuPressed && !wasMenuPressed)
    {
        // Reset scores and return to main menu
        resetScores();
        resetGame(renderer, *worldSystem);
        setState(GameState::MENU);
        WorldSystem::stopAllSounds();
        WorldSystem::stopBackgroundMusic();
    }

    wasRestartPressed = restartPressed;
    wasSelectLevelPressed = selectLevelPressed;
    wasMenuPressed = menuPressed;

    // Render the buttons
    renderer.renderButton(
        restartButton.x, restartButton.y,
        restartButton.width, restartButton.height,
        restartButton.text,
        restartHovered, restartPressed);

    renderer.renderButton(
        selectLevelButton.x, selectLevelButton.y,
        selectLevelButton.width, selectLevelButton.height,
        selectLevelButton.text,
        selectLevelHovered, selectLevelPressed);

    renderer.renderButton(
        menuButton.x, menuButton.y,
        menuButton.width, menuButton.height,
        menuButton.text,
        menuHovered, menuPressed);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (player1Score == 2)
    {
        renderer.renderText("CHALLENGE COMPLETE!", 40, 150, 0.89f, glm::vec3(0.05f, 0.05f, 0.05f));
    }
    else
    {
        renderer.renderText("CHALLENGE FAILED!", 115, 150, 0.9f, glm::vec3(0.05f, 0.05f, 0.05f));
    }
    renderer.renderText("PARRIES :", 325, 250, 0.5f, glm::vec3(0.05f, 0.05f, 0.05f));
    renderer.renderText("JUST PARRIES : ", 200, 320, 0.5f, glm::vec3(0.05f, 0.05f, 0.05f));
}

void Game::resetScores()
{
    player1Score = 0;
    player2Score = 0;
    koTextTimer = 0.0f;
    koFlashCount = 0;
    currentRound = 1;
    showKoText = true;
    fadeOutTimer = 0.0f;
    fadeInTimer = 0.0f;
    startedFading = false;
}

void Game::setP1SelectedCharacter() { 
    if (p1SelectedCharacterIndex == 0) {
        p1SelectedCharacter = Fighters::BIRDMAN;
        std::cout << "P1 Selected Birdman" << std::endl;
    } else {
        p1SelectedCharacter = Fighters::BEARMAN;
        std::cout << "P1 Selected Bearman" << std::endl;
    }
}

void Game::setP2SelectedCharacter() { 
    if (p2SelectedCharacterIndex == 0) {
        p2SelectedCharacter = Fighters::BIRDMAN;
        std::cout << "P2 Selected Birdman" << std::endl;
    } else {
        p2SelectedCharacter = Fighters::BEARMAN;
        std::cout << "P2 Selected Bearman" << std::endl;
    }
}