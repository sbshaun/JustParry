#include "game.hpp"
#include "../linearinterp.hpp"

Game::Game() : currentState(GameState::INIT), running(true), loadingProgress(0.0f),
               isGeneralSelected(true),
               isControlsSelected(false),
               isWindowSelected(true),
               isAudioSelected(false),
               isPlayer1Selected(true),
               isPlayer2Selected(false),
               // Initialize these from settings
               showFPS(Settings::windowSettings.show_fps),
               botEnabled(Settings::windowSettings.enable_bot)
{
    float leftShift = M_WINDOW_WIDTH_PX * (0.05f + 0.015f + 0.02f + 0.03f); // 5% + 1.5% + 2% + 3% of window width
    float upShift = M_WINDOW_HEIGHT_PX * 0.05f;                             // 5% of window height

    // Position start button (moved left and up)
    startButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        375.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "START"                                        // button text
    };

    arcadeButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        475.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "ARCADE"                                       // button text
    };

    arcadeLevelOneButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift, // x position
        250.0f - upShift,                              // y position
        500.0f,                                        // width
        80.0f,                                         // height
        "Level One"                                    // button text
    };

    arcadeLevelTwoButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift, // x position
        350.0f - upShift,                              // y position
        500.0f,                                        // width
        80.0f,                                         // height
        "Level Two"                                    // button text
    };
    arcadeLevelThreeButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift, // x position
        450.0f - upShift,                              // y position
        500.0f,                                        // width
        80.0f,                                         // height
        "Level Three"                                  // button text
    };
    arcadeLevelFourButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift, // x position
        550.0f - upShift,                              // y position
        500.0f,                                        // width
        80.0f,                                         // height
        "Level Four"                                   // button text
    };
    arcadeLevelFiveButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 300.0f - leftShift, // x position
        650.0f - upShift,                              // y position
        500.0f,                                        // width
        80.0f,                                         // height
        "Level Five"                                   // button text
    };
    // Position help button below start button
    helpButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 300.0f - leftShift, // x position
        575.0f - upShift,                              // y position
        260.0f,                                        // width
        80.0f,                                         // height
        "HELP"                                         // button text
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

    // Initialize resume button (centered)
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

    // Add two new buttons for settings navigation
    generalButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 295.0f,  // x position
        M_WINDOW_HEIGHT_PX / 2.0f - 217.5f, // y position
        275.0f,                             // width
        45.0f,                              // height
        "GENERAL"};

    controlsButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 30.f,    // x position
        M_WINDOW_HEIGHT_PX / 2.0f - 217.5f, // y position
        275.0f,                             // width
        45.0f,                              // height
        "CONTROLS"};

    // Add smaller toggle buttons
    windowButton = {
        M_WINDOW_WIDTH_PX / 2.0f - 210.0f, // Same x as generalButton
        M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // 50px below generalButton
        190.0f,                            // Smaller width
        35.0f,                             // Smaller height
        "WINDOW"};

    audioButton = {
        M_WINDOW_WIDTH_PX / 2.0f + 30.0f,  // Next to windowButton
        M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as windowButton
        190.0f,                            // Same width
        35.0f,                             // Same height
        "AUDIO"};

    player1Button = {
        M_WINDOW_WIDTH_PX / 2.0f - 210.0f, // Same x as controlsButton
        M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as other toggle buttons
        190.0f,                            // Same width
        35.0f,                             // Same height
        "PLAYER 1"};

    player2Button = {
        M_WINDOW_WIDTH_PX / 2.0f + 30.0f,  // Next to player1Button
        M_WINDOW_HEIGHT_PX / 2.0f - 145.f, // Same y as other toggle buttons
        190.0f,                            // Same width
        35.0f,                             // Same height
        "PLAYER 2"};

    // Initialize reset button (bottom left of settings)
    resetButton = {
        125.0f,                      // x - align with labels
        M_WINDOW_HEIGHT_PX - 160.0f, // y - 100px from bottom
        175.0f,                      // width
        60.0f,                       // height
        "RESET"};

    // Add new button definitions

    // In the Game constructor, initialize the new buttons
    windowButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 65.0f, 210.0f, 40.0f, "Placeholder 2"};
    windowButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 15.0f, 210.0f, 40.0f, "Placeholder 3"};
    windowButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 40.0f, "Placeholder 4"};
    windowButton5 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 85.0f, 210.0f, 40.0f, "Placeholder 5"};
    windowButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 135.0f, 210.0f, 40.0f, "Placeholder 1"};

    audioButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 65.0f, 210.0f, 40.0f, "Placeholder 2"};
    audioButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 15.0f, 210.0f, 40.0f, "Placeholder 3"};
    audioButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 40.0f, "Placeholder 4"};
    audioButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 85.0f, 210.0f, 40.0f, "Placeholder 5"};

    playerButton1 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 70.f, 210.0f, 30.0f, "Placeholder 1"};
    playerButton2 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f - 35.0f, 210.0f, 30.0f, "Placeholder 2"};
    playerButton3 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 0.0f, 210.0f, 30.0f, "Placeholder 3"};
    playerButton4 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 35.0f, 210.0f, 30.0f, "Placeholder 4"};
    playerButton5 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 90.0f, 210.0f, 30.0f, "Placeholder 5"};
    playerButton6 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 125.0f, 210.0f, 30.0f, "Placeholder 6"};
    playerButton7 = {M_WINDOW_WIDTH_PX / 2.0f + 60.0f, M_WINDOW_HEIGHT_PX / 2.0f + 160.0f, 210.0f, 30.0f, "Placeholder 7"};

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
    FILE *file = fopen(saves_path("arcadeLevelState.txt").c_str(), "r");
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
    FILE *file = fopen(saves_path("arcadeLevelState.txt").c_str(), "w");
    if (file != nullptr)
    {
        fprintf(file, "%d", this->levelCompleted);
        fclose(file);
        printf("Integer written to file successfully.\n");
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background
    renderer.renderTexturedQuadScaled(
        renderer.m_characterSelectTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness
    );

    renderer.renderTexturedQuadScaled(
        p1 ? renderer.m_character1_ready : renderer.m_character1,
        200.f, 360.f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        p2 ? renderer.m_character1_flip_ready : renderer.m_character1_flip,
        600.f, 360.f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_p1SelectKey,
        360.f, 245.0f + offset1,
        30, 30,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_p2SelectKey,
        620.f, 245.0f + offset2,
        30, 30,
        1.0f // Full brightness for main menu
    );

    renderer.renderSelectorTriangleP1(400, 245 + offset1, 30, 30, p1);
    renderer.renderSelectorTriangleP2(580, 245 + offset2, 30, 30, p2);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glDepthFunc(GL_LESS);
}

void Game::renderArcadePrefight(GlRender &renderer, float offset1, bool p1)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render background
    renderer.renderTexturedQuadScaled(
        renderer.m_characterSelectTexture,
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        1.0f // Full brightness
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_character1,
        175, 360.0f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_character1_flip,
        625.f, 360.0f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_p1SelectKey,
        360.f, 245.0f + offset1,
        30, 30,
        1.0f // Full brightness for main menu
    );

    renderer.renderTexturedQuadScaled(
        renderer.m_p2SelectKey,
        620.f, 245.0f,
        30, 30,
        1.0f // Full brightness for main menu
    );

    renderer.renderSelectorTriangleP1(400, 245 + offset1, 30, 30, p1);
    renderer.renderSelectorTriangleP2(580, 245, 30, 30, true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glDepthFunc(GL_LESS);
}

void Game::handleArcadePrefightInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready,
                                      bool &goDown1, bool &goUp1, float &offsetY1)
{
    if (glfwGetKey(glWindow.window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (!p1KeyPressed) // Check if the key was not pressed before
        {
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
        if (glfwGetKey(glWindow.window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (!goDown1)
            {
                goDown1 = true;
                if (offsetY1 < 280.f)
                {
                    offsetY1 += 140.f;
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (!goUp1)
            {
                goUp1 = true;
                if (offsetY1 > 0.0f)
                {
                    offsetY1 -= 140.f;
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

void Game::handleCharacterInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &p2KeyPressed,
                                 bool &p2Ready, bool &goDown1, bool &goDown2, bool &goUp1, bool &goUp2, float &offsetY1, float &offsetY2)
{
    if (glfwGetKey(glWindow.window, GLFW_KEY_R) == GLFW_PRESS)
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

    if (glfwGetKey(glWindow.window, GLFW_KEY_X) == GLFW_PRESS)
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

    if (!p1Ready)
    {
        if (glfwGetKey(glWindow.window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (!goDown1)
            {
                WorldSystem::playMenuSelectSound();
                goDown1 = true;
                if (offsetY1 < 280.f)
                {
                    offsetY1 += 140.f;
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (!goUp1)
            {
                WorldSystem::playMenuSelectSound();
                goUp1 = true;
                if (offsetY1 > 0.0f)
                {
                    offsetY1 -= 140.f;
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
        if (glfwGetKey(glWindow.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (!goDown2)
            {
                WorldSystem::playMenuSelectSound();
                goDown2 = true;
                if (offsetY2 < 280.f)
                {
                    offsetY2 += 140.f;
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if (!goUp2)
            {
                WorldSystem::playMenuSelectSound();
                goUp2 = true;
                if (offsetY2 > 0.0f)
                {
                    offsetY2 -= 140.f;
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
        renderer.renderText("PRESS SPACE", 410, 650, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("TO START!", 435, 700, 0.3f, glm::vec3(0.0f, 0.0f, 0.0f));
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            WorldSystem::playGameCountDownSound(); 
            game.setState(GameState::ROUND_START);
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
    if (currentState == GameState::HELP)
    {
        renderHelpScreen(renderer);
    }

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
    bool LevelOnePressed = levelOneHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelTwoPressed = levelThreeHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelThreePressed = levelTwoHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelFourPressed = levelFourHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool LevelFivePressed = levelFiveHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Render all buttons
    renderer.renderButton(backButton.x, backButton.y,
                          backButton.width, backButton.height,
                          backButton.text,
                          backHovered, backPressed);

    renderer.renderButton(arcadeLevelOneButton.x, arcadeLevelOneButton.y,
                          arcadeLevelOneButton.width, arcadeLevelOneButton.height,
                          arcadeLevelOneButton.text,
                          levelOneHovered, LevelOnePressed);

    if (levelCompleted >= 1)
    {
        renderer.renderButton(arcadeLevelTwoButton.x, arcadeLevelTwoButton.y,
                              arcadeLevelTwoButton.width, arcadeLevelTwoButton.height,
                              arcadeLevelTwoButton.text,
                              levelTwoHovered, LevelThreePressed);
    }

    if (levelCompleted >= 2)
    {
        renderer.renderButton(arcadeLevelThreeButton.x, arcadeLevelThreeButton.y,
                              arcadeLevelThreeButton.width, arcadeLevelThreeButton.height,
                              arcadeLevelThreeButton.text,
                              levelThreeHovered, LevelTwoPressed);
    }

    if (levelCompleted >= 3)
    {
        renderer.renderButton(arcadeLevelFourButton.x, arcadeLevelFourButton.y,
                              arcadeLevelFourButton.width, arcadeLevelFourButton.height,
                              arcadeLevelFourButton.text,
                              levelFourHovered, LevelFourPressed);
    }

    if (levelCompleted >= 4)
    {
        renderer.renderButton(arcadeLevelFiveButton.x, arcadeLevelFiveButton.y,
                              arcadeLevelFiveButton.width, arcadeLevelFiveButton.height,
                              arcadeLevelFiveButton.text,
                              levelFiveHovered, LevelFivePressed);
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
        helpBoxX + helpBoxWidth - 75.0f, // 10px padding from right
        helpBoxY + 15.0f,                // 10px padding from top
        60.0f,                           // Square width
        50.0f,                           // Square height
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
    // First render the appropriate background based on where we came from
    if (getPreviousState() == GameState::PAUSED)
    {
        // If from pause menu, render the game state
        renderer.render();
        extern int timer;
        renderer.renderUI(timer);
        this->renderPauseButton(renderer);
    }
    else if (getPreviousState() == GameState::MENU)
    {
        // If from main menu, render the menu background first
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture,
            0, 0,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f // Full brightness
        );
    }

    // Then render the semi-transparent black overlay
    renderer.renderTexturedQuadScaled(
        renderer.m_menuTexture, // Use any texture
        0, 0,
        M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
        0.0f, // Set brightness to 0 to make it black
        0.5f  // 50% opacity for the black overlay
    );

    // Calculate dimensions for the settings screen image
    float settingsBoxWidth = 850.0f;
    float settingsBoxHeight = 600.0f;
    float settingsBoxX = (M_WINDOW_WIDTH_PX - settingsBoxWidth) / 2.0f;
    float settingsBoxY = (M_WINDOW_HEIGHT_PX - settingsBoxHeight) / 2.0f;

    // Render the settings screen image in the center
    if (renderer.m_settingsTexture != 0)
    {
        renderer.renderTexturedQuadScaled(
            renderer.m_settingsTexture,
            settingsBoxX, settingsBoxY,
            settingsBoxWidth, settingsBoxHeight,
            1.0f, // Full brightness
            1.0f  // Full opacity
        );
    }

    // Always render the SETTINGS text on top
    renderer.renderText("SETTINGS", 410, 140, 0.55f, glm::vec3(0.0f, 0.0f, 0.0f));

    // Position and size the close button (make it square)
    closeButton = {
        settingsBoxX + settingsBoxWidth - 75.f, // 10px padding from right
        settingsBoxY + 10.0f,                   // 10px padding from top
        65.0f,                                  // Square width
        50.0f,                                  // Square height
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

    // Get mouse position and check hover/press states
    bool generalHovered = mouseX >= generalButton.x &&
                          mouseX <= generalButton.x + generalButton.width &&
                          mouseY >= generalButton.y &&
                          mouseY <= generalButton.y + generalButton.height;

    bool controlsHovered = mouseX >= controlsButton.x &&
                           mouseX <= controlsButton.x + controlsButton.width &&
                           mouseY >= controlsButton.y &&
                           mouseY <= controlsButton.y + controlsButton.height;

    // Add hover checks for the toggle buttons
    bool windowHovered = mouseX >= windowButton.x &&
                         mouseX <= windowButton.x + windowButton.width &&
                         mouseY >= windowButton.y &&
                         mouseY <= windowButton.y + windowButton.height;

    bool audioHovered = mouseX >= audioButton.x &&
                        mouseX <= audioButton.x + audioButton.width &&
                        mouseY >= audioButton.y &&
                        mouseY <= audioButton.y + audioButton.height;

    bool player1Hovered = mouseX >= player1Button.x &&
                          mouseX <= player1Button.x + player1Button.width &&
                          mouseY >= player1Button.y &&
                          mouseY <= player1Button.y + player1Button.height;

    bool player2Hovered = mouseX >= player2Button.x &&
                          mouseX <= player2Button.x + player2Button.width &&
                          mouseY >= player2Button.y &&
                          mouseY <= player2Button.y + player2Button.height;

    // Handle button clicks
    static bool wasPressed = false;
    bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool generalPressed = generalHovered && isPressed;
    bool controlsPressed = controlsHovered && isPressed;
    bool windowPressed = windowHovered && isPressed;
    bool audioPressed = audioHovered && isPressed;
    bool player1Pressed = player1Hovered && isPressed;
    bool player2Pressed = player2Hovered && isPressed;

    if (isPressed && !wasPressed)
    {
        if (generalHovered)
        {
            isGeneralSelected = true;
            isControlsSelected = false;
        }
        else if (controlsHovered)
        {
            isGeneralSelected = false;
            isControlsSelected = true;
        }
        else if (windowHovered && isGeneralSelected)
        {
            isWindowSelected = true;
            isAudioSelected = false;
        }
        else if (audioHovered && isGeneralSelected)
        {
            isWindowSelected = false;
            isAudioSelected = true;
        }
        else if (player1Hovered && isControlsSelected)
        {
            isPlayer1Selected = true;
            isPlayer2Selected = false;
        }
        else if (player2Hovered && isControlsSelected)
        {
            isPlayer1Selected = false;
            isPlayer2Selected = true;
        }
    }
    wasPressed = isPressed;

    // Save current OpenGL state
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

    // Disable depth testing and enable blending for all 2D elements
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Original color (normalized)
    glm::vec3 color(139.0f / 255.0f, 169.0f / 255.0f, 174.0f / 255.0f);
    glm::vec3 darkerColor = color * 1.075f;

    // Navigation buttons (GENERAL, CONTROLS)
    renderer.renderSimpleButton(
        generalButton.x, generalButton.y,
        generalButton.width, generalButton.height,
        false, // No selection animation
        false, // No hover animation for button
        false, // No pressed animation
        darkerColor);

    renderer.renderSimpleButton(
        controlsButton.x, controlsButton.y,
        controlsButton.width, controlsButton.height,
        false, // No selection animation
        false, // No hover animation for button
        false, // No pressed animation
        darkerColor);

    // Then render the button text with hover effect
    renderer.renderText("GENERAL",
                        generalButton.x + 50.0f, generalButton.y + 35.0f, 0.4f,
                        isGeneralSelected || generalHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

    renderer.renderText("CONTROLS",
                        controlsButton.x + 40.0f, controlsButton.y + 35.0f, 0.4f,
                        isControlsSelected || controlsHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

    // Render content based on which button is selected
    if (isGeneralSelected)
    {
        renderer.renderSimpleButton(
            windowButton.x, windowButton.y,
            windowButton.width, windowButton.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);

        renderer.renderSimpleButton(
            audioButton.x, audioButton.y,
            audioButton.width, audioButton.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);

        renderer.renderText("WINDOW",
                            windowButton.x + 33.5f, windowButton.y + 27.5f, 0.3f,
                            isWindowSelected || windowHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("AUDIO",
                            audioButton.x + 47.5f, audioButton.y + 27.5f, 0.3f,
                            isAudioSelected || audioHovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

        if (isWindowSelected)
        {
            // Window settings labels and buttons
            float labelX = 215;
            float buttonX = M_WINDOW_WIDTH_PX / 2.0f + 60.0f;
            float startY = 350;
            float spacing = 50;
            float buttonWidth = 210.0f;
            float buttonHeight = 40.0f;

            // Labels - gray out the disabled options
            renderer.renderText("WINDOW MODE", labelX, startY, 0.24f, glm::vec3(0.5f, 0.5f, 0.5f));          // Grayed out
            renderer.renderText("RESOLUTION", labelX, startY + spacing, 0.24f, glm::vec3(0.5f, 0.5f, 0.5f)); // Grayed out
            renderer.renderText("SHOW FPS COUNTER [F]", labelX, startY + spacing * 2, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("ENABLE BOT [B]", labelX, startY + spacing * 3, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("ENABLE DEBUG MODE [D]", labelX, startY + spacing * 4, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));

            // Update button positions
            float buttonY = startY - 22;
            windowButton1 = {buttonX, buttonY, buttonWidth, buttonHeight, "WINDOWED"};             // Fixed text
            windowButton2 = {buttonX, buttonY + spacing, buttonWidth, buttonHeight, "1024 x 768"}; // Fixed text
            windowButton3 = {buttonX, buttonY + spacing * 2, buttonWidth, buttonHeight, Settings::windowSettings.show_fps ? "ON" : "OFF"};
            windowButton4 = {buttonX, buttonY + spacing * 3, buttonWidth, buttonHeight, Settings::windowSettings.enable_bot ? "ON" : "OFF"};
            windowButton5 = {buttonX, buttonY + spacing * 4, buttonWidth, buttonHeight, Settings::windowSettings.enable_debug ? "ON" : "OFF"};

            // Check hover states (only for enabled buttons)
            bool wButton1Hovered = false; // Disabled
            bool wButton2Hovered = false; // Disabled
            bool wButton3Hovered = mouseX >= windowButton3.x && mouseX <= windowButton3.x + windowButton3.width &&
                                   mouseY >= windowButton3.y && mouseY <= windowButton3.y + windowButton3.height;
            bool wButton4Hovered = mouseX >= windowButton4.x && mouseX <= windowButton4.x + windowButton4.width &&
                                   mouseY >= windowButton4.y && mouseY <= windowButton4.y + windowButton4.height;
            bool wButton5Hovered = mouseX >= windowButton5.x && mouseX <= windowButton5.x + windowButton5.width &&
                                   mouseY >= windowButton5.y && mouseY <= windowButton5.y + windowButton5.height;

            // Handle clicks (only for enabled buttons)
            static bool wasPressed = false;
            bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

            if (isPressed && !wasPressed)
            {
                // Remove window mode and resolution handlers
                if (wButton3Hovered) // FPS Counter
                {
                    Settings::windowSettings.show_fps = !Settings::windowSettings.show_fps;
                    showFPS = Settings::windowSettings.show_fps;
                    Settings::saveSettings();
                    std::cout << "FPS display " << (showFPS ? "enabled" : "disabled") << std::endl;
                }
                else if (wButton4Hovered) // Bot Mode
                {
                    Settings::windowSettings.enable_bot = !Settings::windowSettings.enable_bot;
                    botEnabled = Settings::windowSettings.enable_bot;
                    if (worldSystem != nullptr)
                    {
                        worldSystem->botEnabled = botEnabled;
                    }
                    Settings::saveSettings();
                    std::cout << "Bot mode " << (botEnabled ? "enabled" : "disabled") << std::endl;
                }
                else if (wButton5Hovered) // Debug Mode
                {
                    Settings::windowSettings.enable_debug = !Settings::windowSettings.enable_debug;
                    renderer.debugMode = Settings::windowSettings.enable_debug;
                    Settings::saveSettings();
                    std::cout << "Debug mode " << (renderer.debugMode ? "enabled" : "disabled") << std::endl;
                }
            }
            wasPressed = isPressed;

            // Render buttons with hover effect
            const glm::vec3 disabledColor(0.4f, 0.4f, 0.4f); // Darker gray for disabled buttons
            const glm::vec3 placeholderColor(0.6f, 0.6f, 0.6f);
            const glm::vec3 white(1.0f, 1.0f, 1.0f);
            const glm::vec3 grayText(0.7f, 0.7f, 0.7f); // Gray text for disabled buttons

            // Render disabled buttons with gray color
            renderer.renderSimpleButton(windowButton1.x, windowButton1.y, windowButton1.width, windowButton1.height,
                                        true, false, false, disabledColor);
            renderer.renderSimpleButton(windowButton2.x, windowButton2.y, windowButton2.width, windowButton2.height,
                                        true, false, false, disabledColor);

            // Render enabled buttons normally
            renderer.renderSimpleButton(windowButton3.x, windowButton3.y, windowButton3.width, windowButton3.height,
                                        true, wButton3Hovered, false, placeholderColor);
            renderer.renderSimpleButton(windowButton4.x, windowButton4.y, windowButton4.width, windowButton4.height,
                                        true, wButton4Hovered, false, placeholderColor);
            renderer.renderSimpleButton(windowButton5.x, windowButton5.y, windowButton5.width, windowButton5.height,
                                        true, wButton5Hovered, false, placeholderColor);

            // Render button text (grayed out for disabled buttons)
            renderer.renderText(windowButton1.text, windowButton1.x + 40.f, windowButton1.y + 30.f, 0.25f, grayText);
            renderer.renderText(windowButton2.text, windowButton2.x + 40.f, windowButton2.y + 30.f, 0.25f, grayText);
            if (strcmp(windowButton3.text, "ON") == 0)
            {
                renderer.renderText(windowButton3.text, windowButton3.x + 85.f, windowButton3.y + 30.f, 0.25f, white);
            }
            else
            {
                renderer.renderText(windowButton3.text, windowButton3.x + 80.f, windowButton3.y + 30.f, 0.25f, white);
            }

            if (strcmp(windowButton4.text, "ON") == 0)
            {
                renderer.renderText(windowButton4.text, windowButton4.x + 85.f, windowButton4.y + 30.f, 0.25f, white);
            }
            else
            {
                renderer.renderText(windowButton4.text, windowButton4.x + 80.f, windowButton4.y + 30.f, 0.25f, white);
            }

            if (strcmp(windowButton5.text, "ON") == 0)
            {
                renderer.renderText(windowButton5.text, windowButton5.x + 85.f, windowButton5.y + 30.f, 0.25f, white);
            }
            else
            {
                renderer.renderText(windowButton5.text, windowButton5.x + 80.f, windowButton5.y + 30.f, 0.25f, white);
            }
        }
        else if (isAudioSelected)
        {
            // Audio settings labels and buttons
            float labelX = 215;
            float buttonX = M_WINDOW_WIDTH_PX / 2.0f + 60.0f;
            float startY = 350;
            float spacing = 50;
            float buttonWidth = 210.0f;
            float buttonHeight = 40.0f;

            // Labels
            renderer.renderText("ENABLE SOUND EFFECTS", labelX, startY, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("ENABLE MUSIC", labelX, startY + spacing, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("OVERALL VOLUME", labelX, startY + spacing * 2, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
            renderer.renderText("MUSIC VOLUME", labelX, startY + spacing * 3, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));

            // Update button positions and texts
            float buttonY = startY - 22;
            audioButton1 = {buttonX, buttonY, buttonWidth, buttonHeight,
                            Settings::audioSettings.enable_sound_effects ? "ON" : "OFF"};
            audioButton2 = {buttonX, buttonY + spacing, buttonWidth, buttonHeight,
                            Settings::audioSettings.enable_music ? "ON" : "OFF"};
            audioButton3 = {buttonX, buttonY + spacing * 2, buttonWidth, buttonHeight,
                            (std::to_string(int(Settings::audioSettings.overall_volume * 100)) + "%").c_str()};
            audioButton4 = {buttonX, buttonY + spacing * 3, buttonWidth, buttonHeight,
                            (std::to_string(int(Settings::audioSettings.music_volume * 100)) + "%").c_str()};

            // Check hover states
            bool aButton1Hovered = mouseX >= audioButton1.x && mouseX <= audioButton1.x + audioButton1.width &&
                                   mouseY >= audioButton1.y && mouseY <= audioButton1.y + audioButton1.height;
            bool aButton2Hovered = mouseX >= audioButton2.x && mouseX <= audioButton2.x + audioButton2.width &&
                                   mouseY >= audioButton2.y && mouseY <= audioButton2.y + audioButton2.height;
            bool aButton3Hovered = mouseX >= audioButton3.x && mouseX <= audioButton3.x + audioButton3.width &&
                                   mouseY >= audioButton3.y && mouseY <= audioButton3.y + audioButton3.height;
            bool aButton4Hovered = mouseX >= audioButton4.x && mouseX <= audioButton4.x + audioButton4.width &&
                                   mouseY >= audioButton4.y && mouseY <= audioButton4.y + audioButton4.height;

            static bool isDraggingOverall = false;
            static bool isDraggingMusic = false;

            // Handle clicks and drags
            bool isPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            static bool wasPressed = false;

            if (isPressed)
            {
                if (!wasPressed)
                {
                    // Initial click
                    if (aButton1Hovered)
                    {
                        Settings::audioSettings.enable_sound_effects = !Settings::audioSettings.enable_sound_effects;
                        Settings::saveSettings();
                        WorldSystem::updateVolume(); // Add this line
                    }
                    else if (aButton2Hovered)
                    {
                        Settings::audioSettings.enable_music = !Settings::audioSettings.enable_music;
                        Settings::saveSettings();
                        WorldSystem::updateAudioState(); // Add this line
                    }
                    else if (aButton3Hovered)
                    {
                        isDraggingOverall = true;
                    }
                    else if (aButton4Hovered)
                    {
                        isDraggingMusic = true;
                    }
                }

                // Handle dragging for volume sliders
                if (isDraggingOverall)
                {
                    float relativeX = std::max(0.0f, std::min(1.0f, (float)(mouseX - audioButton3.x) / audioButton3.width));
                    Settings::audioSettings.overall_volume = relativeX;
                    Settings::saveSettings();
                    WorldSystem::updateVolume(); // Add this line
                }
                else if (isDraggingMusic)
                {
                    float relativeX = std::max(0.0f, std::min(1.0f, (float)(mouseX - audioButton4.x) / audioButton4.width));
                    Settings::audioSettings.music_volume = relativeX;
                    Settings::saveSettings();
                    WorldSystem::updateVolume(); // Add this line
                }
            }
            else
            {
                isDraggingOverall = false;
                isDraggingMusic = false;
            }
            wasPressed = isPressed;

            // Render buttons
            const glm::vec3 placeholderColor(0.6f, 0.6f, 0.6f);
            // Use the theme color for sliders
            const glm::vec3 sliderColor(139.0f / 255.0f, 169.0f / 255.0f, 174.0f / 255.0f);
            const glm::vec3 white(1.0f, 1.0f, 1.0f);

            // Render ON/OFF buttons
            renderer.renderSimpleButton(audioButton1.x, audioButton1.y, audioButton1.width, audioButton1.height,
                                        true, aButton1Hovered, false, placeholderColor);
            renderer.renderSimpleButton(audioButton2.x, audioButton2.y, audioButton2.width, audioButton2.height,
                                        true, aButton2Hovered, false, placeholderColor);

            // Render volume sliders
            // Background
            renderer.renderSimpleButton(audioButton3.x, audioButton3.y, audioButton3.width, audioButton3.height,
                                        true, aButton3Hovered, false, placeholderColor);
            renderer.renderSimpleButton(audioButton4.x, audioButton4.y, audioButton4.width, audioButton4.height,
                                        true, aButton4Hovered, false, placeholderColor);

            // Filled portion for overall volume
            float fillWidth3 = audioButton3.width * Settings::audioSettings.overall_volume;
            renderer.renderSimpleButton(audioButton3.x, audioButton3.y, fillWidth3, audioButton3.height,
                                        false, false, false, sliderColor);

            // Filled portion for music volume
            float fillWidth4 = audioButton4.width * Settings::audioSettings.music_volume;
            renderer.renderSimpleButton(audioButton4.x, audioButton4.y, fillWidth4, audioButton4.height,
                                        false, false, false, sliderColor);

            // Render button text
            if (strcmp(audioButton1.text, "ON") == 0)
            {
                renderer.renderText(audioButton1.text, audioButton1.x + 85.f, audioButton1.y + 30.f, 0.25f, white);
            }
            else
            {
                renderer.renderText(audioButton1.text, audioButton1.x + 80.f, audioButton1.y + 30.f, 0.25f, white);
            }

            if (strcmp(audioButton2.text, "ON") == 0)
            {
                renderer.renderText(audioButton2.text, audioButton2.x + 85.f, audioButton2.y + 30.f, 0.25f, white);
            }
            else
            {
                renderer.renderText(audioButton2.text, audioButton2.x + 80.f, audioButton2.y + 30.f, 0.25f, white);
            }

            // Render volume percentages centered on the sliders
            std::string overallVolText = std::to_string(int(Settings::audioSettings.overall_volume * 100)) + "%";
            std::string musicVolText = std::to_string(int(Settings::audioSettings.music_volume * 100)) + "%";

            renderer.renderText(overallVolText.c_str(),
                                audioButton3.x + (audioButton3.width - overallVolText.length() * 15) / 2,
                                audioButton3.y + 30.f, 0.25f, white);

            renderer.renderText(musicVolText.c_str(),
                                audioButton4.x + (audioButton4.width - musicVolText.length() * 15) / 2,
                                audioButton4.y + 30.f, 0.25f, white);
        }
    }
    else if (isControlsSelected)
    {
        renderer.renderSimpleButton(
            player1Button.x, player1Button.y,
            player1Button.width, player1Button.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);

        renderer.renderSimpleButton(
            player2Button.x, player2Button.y,
            player2Button.width, player2Button.height,
            false, // No selection animation
            false, // No hover animation for button
            false, // No pressed animation
            darkerColor);

        renderer.renderText("PLAYER 1",
                            player1Button.x + 30.0f, player1Button.y + 27.5f, 0.3f,
                            isPlayer1Selected || player1Hovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));
        renderer.renderText("PLAYER 2",
                            player2Button.x + 27.5f, player2Button.y + 27.5f, 0.3f,
                            isPlayer2Selected || player2Hovered ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f));

        // Remove all the player button rendering code here and just call renderControlsSettings
        renderControlsSettings(renderer, isPlayer1Selected, isPlayer2Selected);

        // Restore previous OpenGL state
        if (depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        glBlendFunc(blendSrc, blendDst);

        bool resetHovered = mouseX >= resetButton.x &&
                            mouseX <= resetButton.x + resetButton.width &&
                            mouseY >= resetButton.y &&
                            mouseY <= resetButton.y + resetButton.height;

        bool resetPressed = resetHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Check if settings are at default values
        bool isDefaultSettings = Settings::areSettingsDefault();

        // If settings are default, render disabled button
        if (isDefaultSettings)
        {
            // Render disabled reset button (grayed out)
            renderer.renderButton(
                resetButton.x, resetButton.y,
                resetButton.width, resetButton.height,
                resetButton.text,
                false,                      // Not hoverable
                false,                      // Not pressable
                glm::vec3(0.4f, 0.4f, 0.4f) // Gray color for disabled state
            );
        }
        else
        {
            // Render normal reset button
            renderer.renderButton(
                resetButton.x, resetButton.y,
                resetButton.width, resetButton.height,
                resetButton.text,
                resetHovered,
                resetPressed,
                glm::vec3(0.6f, 0.2f, 0.2f) // Normal red color
            );
        }
    }

    // Restore previous OpenGL state
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    glBlendFunc(blendSrc, blendDst);

    // Check if settings are at default values
    bool isDefaultSettings = Settings::areSettingsDefault();

    // Get mouse position for reset button
    bool resetHovered = mouseX >= resetButton.x &&
                        mouseX <= resetButton.x + resetButton.width &&
                        mouseY >= resetButton.y &&
                        mouseY <= resetButton.y + resetButton.height;

    bool resetPressed = resetHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Handle reset button click
    static bool wasResetPressed = false;
    if (resetPressed && !wasResetPressed && !isDefaultSettings)
    {
        Settings::resetToDefaults();
        // Update local state
        showFPS = Settings::windowSettings.show_fps;
        botEnabled = Settings::windowSettings.enable_bot;
        if (worldSystem != nullptr)
        {
            worldSystem->botEnabled = botEnabled;
            worldSystem->initInputHandlers();
        }
        renderer.debugMode = Settings::windowSettings.enable_debug;
        std::cout << "Settings reset to defaults" << std::endl;
    }
    wasResetPressed = resetPressed;

    // Render the reset button based on current settings state
    if (isDefaultSettings)
    {
        // Render disabled reset button (grayed out)
        renderer.renderButton(
            resetButton.x, resetButton.y,
            resetButton.width, resetButton.height,
            resetButton.text,
            false,                      // Not hoverable
            false,                      // Not pressable
            glm::vec3(0.4f, 0.4f, 0.4f) // Gray color for disabled state
        );
    }
    else
    {
        // Render normal reset button
        renderer.renderButton(
            resetButton.x, resetButton.y,
            resetButton.width, resetButton.height,
            resetButton.text,
            resetHovered,
            resetPressed,
            glm::vec3(0.6f, 0.2f, 0.2f) // Normal red color
        );
    }
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
    this->setState(GameState::ARCADE_MENU);
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

    // reset the arcade level when going back to the menu
    currentLevel = 0;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (mouseOverBack)
        {
            handleBackButton();
        }
        if (mouseOverLevelOne)
        {

            currentLevel = 1;
            return true;
        }
        else if (mouseOverLevelTwo && levelCompleted >= 1)
        {
            currentLevel = 2;
            return true;
        }
        else if (mouseOverLevelThree && levelCompleted >= 2)
        {
            currentLevel = 3;
            return true;
        }
        else if (mouseOverLevelFour && levelCompleted >= 3)
        {
            currentLevel = 4;
            return true;
        }
        else if (mouseOverLevelFive && levelCompleted >= 4)
        {
            currentLevel = 5;
            return true;
        }
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

bool Game::handleSettingsInput(GLFWwindow *window)
{
    // Add this line to handle control settings input
    handleControlsSettingsInput(window);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check if close button is clicked
    bool mouseOverClose = mouseX >= closeButton.x &&
                          mouseX <= closeButton.x + closeButton.width &&
                          mouseY >= closeButton.y &&
                          mouseY <= closeButton.y + closeButton.height;

    if (mouseOverClose && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        return true; // Return to menu
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
    generalButton = {};
    controlsButton = {};
    windowButton = {};
    audioButton = {};
    player1Button = {};
    player2Button = {};
    resetButton = {}; // Keep this
    pauseSettingsButton = {};

    // Remove applyButton = {};  // Remove this line

    // Clean up window and audio buttons
    windowButton1 = {};
    windowButton2 = {};
    windowButton3 = {};
    windowButton4 = {};
    windowButton5 = {};

    audioButton1 = {};
    audioButton2 = {};
    audioButton3 = {};
    audioButton4 = {};

    playerButton1 = {};
    playerButton2 = {};
    playerButton3 = {};
    playerButton4 = {};
    playerButton5 = {};
    playerButton6 = {};
    playerButton7 = {};
}

void Game::renderPauseButton(GlRender &renderer)
{
    // Show pause button during gameplay, round start, and when paused
    if (currentState != GameState::PLAYING &&
        currentState != GameState::PAUSED &&
        currentState != GameState::ROUND_START)
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
        if (currentState == GameState::PLAYING || currentState == GameState::ROUND_START)
        {
            WorldSystem::stopAllSounds();
            currentState = GameState::PAUSED;
        }
    }
    wasPressed = pausePressed;

    // Always render the pause button (removed scaling on press)
    renderer.renderButton(
        pauseButton.x, pauseButton.y,
        pauseButton.width, pauseButton.height,
        "=",
        pauseHovered, false, // Set pressed to false to prevent scaling
        glm::vec3(0.4f, 0.4f, 0.4f));

    // Render pause menu if game is paused
    if (currentState == GameState::PAUSED)
    {
        // First render a black overlay with reduced opacity
        renderer.renderTexturedQuadScaled(
            renderer.m_menuTexture, // Use any texture
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

        // Render the pause menu background image at full opacity
        renderer.renderTexturedQuadScaled(
            renderer.m_pauseMenuTexture,
            bgX, bgY,
            bgWidth, bgHeight,
            1.0f, // Full brightness
            1.0f  // Full opacity for the pause menu
        );

        // Update button positions to be centered on the background

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

        // Handle button clicks with debouncing
        static bool wasResumePressed = false;
        static bool wasMenuPressed = false;
        static bool wasSettingsPressed = false;

        if (resumePressed && !wasResumePressed)
        {
            currentState = GameState::PLAYING;
        }
        else if (menuPressed && !wasMenuPressed)
        {
            // First change state to INIT to ensure proper reinitialization
            currentState = GameState::INIT;
            resetScores();

            // Clear all components
            registry.clear_all_components();

            // Reset interpolation state
            extern bool isLoading;
            isLoading = true;
            resetInterpVariables();

            // Reset timer
            extern int timer;
            timer = timer_length;

            // Reset camera/playable area position and world model
            renderer.m_worldModel = mat4(1.0f); // Reset world model to identity matrix

            // Reset player positions for next game
            if (worldSystem)
            {
                worldSystem->init(&renderer);

                // Get the players and reset their positions
                Motion &p1Motion = registry.motions.get(renderer.m_player1);
                Motion &p2Motion = registry.motions.get(renderer.m_player2);

                // Reset positions to starting points
                Fighters current_char1 = registry.players.get(renderer.m_player1).current_char;
                Fighters current_char2 = registry.players.get(renderer.m_player2).current_char;
                FighterConfig config1 = FighterManager::getFighterConfig(current_char1);
                FighterConfig config2 = FighterManager::getFighterConfig(current_char2);

                p1Motion.position = {-1.25f, FLOOR_Y + config1.NDC_HEIGHT};
                p2Motion.position = {1.25f, FLOOR_Y + config2.NDC_HEIGHT};

                // Reset scales
                p1Motion.scale = {0.1f, 0.1f};
                p2Motion.scale = {0.1f, 0.1f};

                // Reset playable area
                if (registry.playableArea.has(renderer.m_playableArea))
                {
                    PlayableArea &playableArea = registry.playableArea.get(renderer.m_playableArea);
                    playableArea.position = vec2(0, 0); // Reset to center
                    playableArea.updateWorldModel(renderer.m_worldModel);
                }
            }

            // Don't try to reinitialize here - let the INIT state handle it
            worldSystem = nullptr; // Clear the world system pointer
        }
        else if (settingsPressed && !wasSettingsPressed)
        {
            currentState = GameState::SETTINGS;
            previousState = GameState::PAUSED;
        }

        // Render all pause menu buttons
        renderer.renderButton(
            resumeButton.x, resumeButton.y,
            resumeButton.width, resumeButton.height,
            "RESUME",
            resumeHovered, resumePressed);

        renderer.renderButton(
            menuButton.x, menuButton.y,
            menuButton.width, menuButton.height,
            "MAIN MENU",
            menuHovered, menuPressed);

        renderer.renderButton(
            pauseSettingsButton.x, pauseSettingsButton.y,
            pauseSettingsButton.width, pauseSettingsButton.height,
            "SETTINGS",
            settingsHovered, settingsPressed);

        wasResumePressed = resumePressed;
        wasMenuPressed = menuPressed;
        wasSettingsPressed = settingsPressed;
    }
}

void Game::renderControlsSettings(GlRender &renderer, bool isPlayer1Selected, bool isPlayer2Selected)
{
    // Get mouse position
    GLFWwindow *window = glfwGetCurrentContext();
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Define key button dimensions - make them wider
    float buttonWidth = 215.0f;
    float buttonHeight = 35.0f; // Increased from 30.0f
    float buttonX = 570.0f;     // Adjusted from 665.0f to align with previous buttons
    float labelX = 300.0f;      // Position for labels
    float startY = 330.0f;      // Starting Y position
    float spacing = 40.0f;      // Increased spacing between buttons

    // Render control labels
    renderer.renderText("JUMP", labelX, startY, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("DUCK", labelX, startY + spacing, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("MOVE LEFT", labelX - 25.0f, startY + spacing * 2, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("MOVE RIGHT", labelX - 32.0f, startY + spacing * 3, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("PUNCH", labelX, startY + spacing * 4, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("KICK", labelX, startY + spacing * 5, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText("PARRY", labelX, startY + spacing * 6, 0.24f, glm::vec3(0.0f, 0.0f, 0.0f));

    PlayerControls &controls = isPlayer1Selected ? Settings::p1Controls : Settings::p2Controls;
    float yPositions[] = {
        startY - 22,
        startY - 22 + spacing,
        startY - 22 + spacing * 2,
        startY - 22 + spacing * 3,
        startY - 22 + spacing * 4,
        startY - 22 + spacing * 5,
        startY - 22 + spacing * 6};

    int *keys[] = {
        &controls.up, &controls.down, &controls.left, &controls.right,
        &controls.punch, &controls.kick, &controls.parry};

    // Render each key binding button
    for (int i = 0; i < 7; i++)
    {
        float y = yPositions[i];
        bool isHovered = mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
                         mouseY >= y && mouseY <= y + buttonHeight;
        bool isSelected = currentlyRebindingKey == keys[i];
        bool isError = (i == errorButtonIndex && showErrorMessage);

        // Calculate error alpha for fade effect
        float errorAlpha = 0.0f;
        if (isError)
        {
            if (errorMessageTimer < 1.25f)
            {
                // Stay solid for 1.25 seconds
                errorAlpha = 1.0f;
            }
            else
            {
                // Fade out over 0.5 seconds (from 1.25s to 1.75s)
                errorAlpha = 1.0f - ((errorMessageTimer - 1.25f) / 0.5f);
            }
        }

        // Render button background with error tint if needed
        if (isError)
        {
            // Render red tinted button with fade
            glm::vec3 buttonColor = glm::vec3(1.0f, 0.0f, 0.0f);
            renderer.renderSimpleButton(buttonX, y, buttonWidth, buttonHeight,
                                        true, isHovered, isSelected,
                                        glm::vec3(
                                            buttonColor.r * errorAlpha + 0.6f * (1.0f - errorAlpha),
                                            buttonColor.g * errorAlpha + 0.6f * (1.0f - errorAlpha),
                                            buttonColor.b * errorAlpha + 0.6f * (1.0f - errorAlpha)));

            // Render error icon with fade
            if (errorAlpha > 0.0f)
            {
                renderer.renderText("!", buttonX + buttonWidth + 10, y + buttonHeight / 2 + 10,
                                    0.35f, glm::vec3(1.0f * errorAlpha, 0.0f, 0.0f));
            }
        }
        else
        {
            // Normal button render
            renderer.renderSimpleButton(buttonX, y, buttonWidth, buttonHeight,
                                        true, isHovered, isSelected);
        }

        // Render key text
        std::string keyText = isSelected ? "___" : Settings::getKeyName(*keys[i]);
        float textWidth = keyText.length() * 8.0f;
        float textX = buttonX + (buttonWidth - textWidth) / 2.0f;
        float textY = y + (buttonHeight - 8.0f) / 2.0f;

        renderer.renderText(keyText, textX - 10.f, textY + 10.f,
                            0.24f, glm::vec3(1.0f, 1.0f, 1.0f));

        // Handle click
        if (isHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
            !isRebinding && !currentlyRebindingKey)
        {
            currentlyRebindingKey = keys[i];
            isRebinding = true;
        }
    }

    // Update error message timer
    if (showErrorMessage)
    {
        errorMessageTimer += 0.016f; // Assuming 60 FPS
        if (errorMessageTimer >= 1.75f)
        { // Total duration: 1.25s solid + 0.5s fade
            showErrorMessage = false;
            errorMessageTimer = 0.0f;
            errorButtonIndex = -1;
        }
    }
}

void Game::handleControlsSettingsInput(GLFWwindow *window)
{
    // Get mouse position for reset button
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Check if reset button is clicked
    bool resetHovered = mouseX >= resetButton.x &&
                        mouseX <= resetButton.x + resetButton.width &&
                        mouseY >= resetButton.y &&
                        mouseY <= resetButton.y + resetButton.height;

    static bool wasResetPressed = false;
    bool isResetPressed = resetHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Only trigger reset if settings are not default and button is pressed
    if (isResetPressed && !wasResetPressed && !Settings::areSettingsDefault())
    {
        Settings::resetToDefaults();
    }
    wasResetPressed = isResetPressed;

    if (isRebinding)
    {
        // Check for any key press
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++)
        {
            if (glfwGetKey(window, key) == GLFW_PRESS)
            {
                if (currentlyRebindingKey != nullptr)
                {
                    // Check if this is the same key that's currently bound
                    if (*currentlyRebindingKey == key)
                    {
                        isRebinding = false;
                        currentlyRebindingKey = nullptr;
                        return;
                    }

                    // Check if this key is already bound
                    bool keyAlreadyBound = false;
                    PlayerControls &p1 = Settings::p1Controls;
                    PlayerControls &p2 = Settings::p2Controls;

                    // Check P1 controls
                    if (key == p1.up || key == p1.down || key == p1.left || key == p1.right ||
                        key == p1.punch || key == p1.kick || key == p1.parry)
                    {
                        keyAlreadyBound = true;
                    }

                    // Check P2 controls
                    if (key == p2.up || key == p2.down || key == p2.left || key == p2.right ||
                        key == p2.punch || key == p2.kick || key == p2.parry)
                    {
                        keyAlreadyBound = true;
                    }

                    if (keyAlreadyBound)
                    {
                        // Show error feedback
                        showErrorMessage = true;
                        errorMessageTimer = 0.0f;

                        // Find which button index we're on
                        PlayerControls &controls = isPlayer1Selected ? Settings::p1Controls : Settings::p2Controls;
                        int *keys[] = {&controls.up, &controls.down, &controls.left, &controls.right,
                                       &controls.punch, &controls.kick, &controls.parry};

                        for (int i = 0; i < 7; i++)
                        {
                            if (currentlyRebindingKey == keys[i])
                            {
                                errorButtonIndex = i;
                                break;
                            }
                        }
                    }
                    else
                    {
                        // Update the key binding
                        *currentlyRebindingKey = key;
                        Settings::saveSettings();
                        Settings::loadSettings();
                        if (worldSystem != nullptr)
                        {
                            worldSystem->initInputHandlers();
                        }
                    }
                }

                isRebinding = false;
                currentlyRebindingKey = nullptr;
                break;
            }
        }

        // Allow canceling with backspace
        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        {
            isRebinding = false;
            currentlyRebindingKey = nullptr;
        }
    }
}
