#include "game.hpp"
#include "../input_system/input_mapping.hpp"
#include "../interp/linearinterp.hpp"
#include "settings_menu.hpp"

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
        "Level Five"                                          // button text
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
        fscanf(file, "%d", &this->levelCompleted);
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

    // Rest of the character select rendering...
    renderer.renderTexturedQuadScaled(
        p1 ? renderer.m_character1_ready : renderer.m_character1,
        200.f, 360.f,
        225, 275,
        1.0f);

    renderer.renderTexturedQuadScaled(
        p2 ? renderer.m_character1_flip_ready : renderer.m_character1_flip,
        600.f, 360.f,
        225, 275,
        1.0f // Full brightness for main menu
    );

    // renderer.renderTexturedQuadScaled(
    //     renderer.m_p1SelectKey,
    //     360.f, 245.0f + offset1,
    //     30, 30,
    //     1.0f // Full brightness for main menu
    // );

    // renderer.renderTexturedQuadScaled(
    //     renderer.m_p2SelectKey,
    //     645.f, 245.0f + offset2,
    //     30, 30,
    //     1.0f // Full brightness for main menu
    // );

    renderer.renderText(Settings::getKeyName(Settings::p1Controls.punch), 360.f, 270.0f + offset1, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.renderText(Settings::getKeyName(Settings::p2Controls.punch), 645.f, 270.0f + offset2, 0.39f, glm::vec3(0.0f, 0.0f, 0.0f));

    renderer.renderSelectorTriangleP1(400, 245 + offset1, 30, 30, p1);
    renderer.renderSelectorTriangleP2(605, 245 + offset2, 30, 30, p2);

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

    static bool backButtonPressed = false;

    if (backHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!backButtonPressed)
        {
            backButtonPressed = true; // Remember that button was pressed
        }
    }
    else
    {
        if (backButtonPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            handleBackButton(); // Trigger action on release
            std::cout << "Back button pressed!" << std::endl;
        }
        backButtonPressed = false; // Reset the state when released
    }

    // render back button
    renderer.renderButton(
        backButton.x, backButton.y,          // x, y position
        backButton.width, backButton.height, // width, height
        "<-",                                // text
        backHovered,                         // Add this member variable to Game class
        backButtonPressed                    // Add this member variable to Game class
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
        if (glfwGetKey(glWindow.window, GLFW_KEY_S) == GLFW_PRESS)
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_W) == GLFW_PRESS)
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

void Game::handleCharacterInputs(GLWindow &glWindow, bool &p1KeyPressed, bool &p1Ready, bool &p2KeyPressed,
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

    if (!p1Ready)
    {
        if (glfwGetKey(glWindow.window, GLFW_KEY_S) == GLFW_PRESS)
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_W) == GLFW_PRESS)
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

    if (!p2Ready)
    {
        if (glfwGetKey(glWindow.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (!goDown2)
            {
                WorldSystem::playMenuSelectSound();
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

        if (glfwGetKey(glWindow.window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if (!goUp2)
            {
                WorldSystem::playMenuSelectSound();
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
    if (!isArcadeMode()) {
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
    } else {
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
                WorldSystem::playGameCountDownSound();
                game.setState(GameState::ARCADE_STORY);
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

    renderer.renderText("PROGRESS : " + std::to_string(levelCompleted) + "/5 COMPLETED", 180.f, 190.0f, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));

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

void Game::renderArcadeStory(GlRender& renderer)
{

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

bool Game::handleArcadeStoryInput(GLFWwindow* window) 
{
    // TODO: handle the story inputs
    return true;
}

void Game::handleBackButton()
{
    if (this->getState() == GameState::ARCADE_PREFIGHT)
    {
        this->setState(GameState::ARCADE_MENU);
        std::cout << "Going to Arcade Menu Screen" << std::endl;
    }
    else
    {
        this->setState(GameState::MENU);
        std::cout << "Going to Menu Screen" << std::endl;
    }
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

void Game::resetGame(GlRender &renderer, WorldSystem &worldSystem)
{
    this->worldSystem = &worldSystem;

    // Clear all components first
    registry.clear_all_components();

    // Reinitialize world system
    worldSystem.init(&renderer);

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
