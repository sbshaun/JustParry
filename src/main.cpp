#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include "bot.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "physics/physics_system.hpp"
#include "ecs/ecs_registry.hpp"
#include "world/world_system.hpp"
#include "linearinterp.hpp"
#include "state/game.hpp"
#include "fps/fps.hpp"
#include "input_system/input_handler.hpp"
#include "input_system/state_machine.hpp"
#include "input_system/utility_inputs.hpp"
#include "SDL.h"
#include <SDL.h>
#define SDL_MAIN_HANDLED

int timer = timer_length;
static bool roundEnded = false;
auto last_time = std::chrono::high_resolution_clock::now();

int generateUI(GlRender &renderer)
{
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_time;

    if (elapsed.count() >= 1.0 && timer > 0)
    {
        last_time = current_time;
        timer--;
    }

    if (timer <= 0)
    {
        return 1;
    }

    renderer.renderUI(timer);
    return 0;
}

void checkIsRoundOver(GlRender &renderer, Bot &botInstance, WorldSystem &worldSystem, Game &game, bool &botEnabled)
{
    Health &h1 = registry.healths.get(renderer.m_player1);
    Health &h2 = registry.healths.get(renderer.m_player2);
    PlayerInput &p1 = registry.playerInputs.get(renderer.m_player1);
    PlayerInput &p2 = registry.playerInputs.get(renderer.m_player2);

    // Check if round is over due to health or timer
    if (h1.currentHealth <= 0 || h2.currentHealth <= 0 || generateUI(renderer) == 1)
    {
        // Update scores and set round over state only once
        if (!roundEnded)
        {
            game.updateScores(h1, h2);
            roundEnded = true;
            game.setState(GameState::ROUND_OVER);
        }

        // Disable player inputs during round over
        p1 = PlayerInput();
        p2 = PlayerInput();

        // Always render round over animation with winner text
        renderer.renderRoundOver(1);
    }
    else
    {
        roundEnded = false;
        if (botEnabled)
        {
            botInstance.pollBotRng(renderer);
        }
        renderer.renderUI(timer);
    }
}

int main()
{
    //// START INITS ////
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    assert(gl3w_init() == 0);
    // assert(is_fine == 0);

    // glfwSwapInterval(1); // Enable vsync
    if (SDL_Init(SDL_INIT_AUDIO) < 0) // Init SDL joystick
    {
        std::cerr << "Failed to init SDL joy handler" << std::endl;
        return EXIT_FAILURE;
    }

    // init fighters config
    FighterManager::init();

    GlRender renderer;
    renderer.initialize();

    // Load settings before initializing world system
    Settings::loadSettings();

    WorldSystem worldSystem; // This will now use the loaded settings

    PhysicsSystem physicsSystem;
    Bot botInstance;

    // Initialize game state
    Game game;
    renderer.setGameInstance(&game);
    FPSCounter fpsCounter(60);

    // Toggle states
    bool showFPS = false;
    bool fKeyPressed = false;
    bool bKeyPressed = false;
    bool hKeyPressed = false;
    bool botEnabled = false;
    bool shouldExit = false;

    const float targetLogicDuration = 1000 / TARGET_LOGIC_RATE;      // denominator is logic+input checks per second
    const float FramesPerLogicLoop = TARGET_LOGIC_RATE / TARGET_FPS; // The number of logic loops that would result in 60fps

    int loopsSinceLastFrame = 0;
    //// END INITS ////

    // font setup and initialization.
    std::string font_filename = "SpaceGrotesk-Bold.ttf";
    unsigned int font_default_size = 100;
    if (!renderer.fontInit(font_filename, font_default_size))
    {
        std::cerr << "Failed to initialize fonts. Exiting." << std::endl;
        return EXIT_FAILURE;
    }

    // Main loop
    bool p1Ready = false;
    bool p2Ready = false;
    static bool p1KeyPressed = false; // Track the state of the key for player 1
    static bool p2KeyPressed = false; // Track the state of the key for player 2
    bool goDown1 = false;
    bool goUp1 = false;
    bool goDown2 = false;
    bool goUp2 = false;

    float offsetY1 = 0.0f;
    float offsetY2 = 0.0f;

    while (!glWindow.shouldClose())
    {
        // start a timer for each loop
        auto start = std::chrono::steady_clock::now();

        switch (game.getState())
        {
        case GameState::INIT:
            worldSystem.init(&renderer);
            game.setState(GameState::MENU);
            break;
        case GameState::MENU:
            p1Ready = false;
            p2Ready = false;
            game.renderMenu(renderer);
            if (game.handleMenuInput(glWindow.window, renderer))
            {
                std::cout << "Entered Character Select Stage" << std::endl;
                game.setState(GameState::CHARACTER_SELECT);
            }
            glWindow.windowSwapBuffers();

            break;
        case GameState::ARCADE_MENU:
            game.renderArcadeMenu(renderer);
            if (game.handleArcadeMenuInput(glWindow.window))
            {
                std::cout << "Entered Character Select Stage" << std::endl;
                game.setState(GameState::ARCADE_PREFIGHT);
            }
            glWindow.windowSwapBuffers();
            break;
        case GameState::ARCADE_PREFIGHT:
            botEnabled = true;
            worldSystem.botEnabled = true;
            game.handleArcadePrefightInputs(glWindow, p1KeyPressed, p1Ready, goDown1, goUp1, offsetY1);
            game.renderArcadePrefight(renderer, offsetY1, p1Ready);
            game.renderReadyText(renderer, p1Ready, true, game);

            glWindow.windowSwapBuffers();
            break;
        case GameState::HELP:

            game.renderHelpScreen(renderer);
            if (game.handleHelpInput(glWindow.window))
            {
                game.setState(GameState::MENU);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::SETTINGS:
            game.renderSettingsScreen(renderer);
            if (game.handleSettingsInput(glWindow.window))
            {
                game.setState(GameState::SETTINGS_EXIT);
            }
            glWindow.windowSwapBuffers();
            break;
        case GameState::SETTINGS_EXIT:
            // Reinitialize input handlers after updating settings
            worldSystem.initInputHandlers();
            game.setState(GameState::MENU);
            break;
        case GameState::CHARACTER_SELECT:
            game.handleCharacterInputs(glWindow, p1KeyPressed, p1Ready, p2KeyPressed, p2Ready, goDown1, goDown2, goUp1, goUp2, offsetY1, offsetY2);
            game.renderCharacterSelect(renderer, offsetY1, offsetY2, p1Ready, p2Ready);
            game.renderReadyText(renderer, p1Ready, p2Ready, game);

            glWindow.windowSwapBuffers();
            break;
        case GameState::ROUND_START:
            interp_moveEntitesToScreen(renderer); // Move players into position

            renderer.render();
            renderer.renderUI(timer);
            game.renderPauseButton(renderer);

            if (!isLoading)
            {
                game.setState(GameState::PLAYING);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::PAUSED:
            // Continue rendering the game state in the background
            renderer.render();
            renderer.renderUI(timer);

            // Render FPS counter if enabled
            if (showFPS)
            {
                renderer.renderFPS(fpsCounter.getFPS(), showFPS);
            }

            // Render the pause menu on top
            game.renderPauseButton(renderer);

            glWindow.windowSwapBuffers();
            break;

        case GameState::PLAYING:
        {
            if (loopsSinceLastFrame == FramesPerLogicLoop)
            { // Only do certain checks each frame rather than every loop
                // std::cout << "RENDER CALL" << std::endl;
                loopsSinceLastFrame = 0;
                interp_moveEntitesToScreen(renderer);
                handleUtilityInputs(renderer, showFPS, botEnabled,
                                    fKeyPressed, bKeyPressed, hKeyPressed,
                                    glWindow, fpsCounter, shouldExit,
                                    worldSystem);
                // toggleFPS(renderer, showFPS, fKeyPressed, glWindow, fpsCounter);
                glWindow.windowSwapBuffers();
            }

            loopsSinceLastFrame++;

            // Update center for playable area
            worldSystem.movementProcessing(); // PROCESS MOVEMENTS BASED ON THE DECISIONS MADE BY FRAME BUFFER
            worldSystem.playerCollisions(&renderer);

            worldSystem.hitBoxCollisions();
            worldSystem.updatePlayableArea();
            physicsSystem.step();

            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);

            renderer.render();
            renderer.renderUI(timer);
            game.renderPauseButton(renderer);

            checkIsRoundOver(renderer, botInstance, worldSystem, game, botEnabled);
            // worldSystem.inputProcessing(); // this sets player inputs #1
            // worldSystem.handleInput(); // this sets player inputs #2

            // time the next logic check
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> FastLoopIterTime = end - start;

            // Calculate the remaining time to sleep
            int sleepDuration = static_cast<int>(targetLogicDuration) - static_cast<int>(FastLoopIterTime.count());
            // std::cout << "i wanna sleep for " << sleepDuration << std::endl;
            if (sleepDuration > 0)
            {
                auto sleepEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepDuration);
                while (std::chrono::steady_clock::now() < sleepEnd)
                {
                    worldSystem.handleInput(); // this sets player inputs #3
                } // Do input polling during wait time maybe and input conflict resoltion each logic step rather than each frame
            }
        }
        break;

        case GameState::LOADING:
            // supress compilation warning
            std::cout << "warning: enumeration value 'LOADING' not handled in switch [-Wswitch]" << std::endl;
            glWindow.windowSwapBuffers();
            break;

        case GameState::ROUND_OVER:
            renderer.render();
            renderer.renderUI(timer);
            renderer.renderRoundOver(1);

            if (!renderer.isExitAnimationStarted())
                if (registry.healths.get(renderer.m_player1).currentHealth > registry.healths.get(renderer.m_player2).currentHealth)
                {
                    game.updateArcadeLevel();
                }

            // Only handle enter press if exit animation hasn't started
            if (!renderer.isExitAnimationStarted() &&
                isKeyPressed(GLFW_KEY_ENTER))
            {
                renderer.startExitAnimation();
            }

            if (!renderer.isExitAnimationStarted() &&
                isKeyPressed(GLFW_KEY_BACKSPACE))
            {
                roundEnded = false;
                game.resetGame(renderer, worldSystem);
                game.setState(GameState::MENU);
            }

            // Only reset the game once the exit animation is complete
            if (renderer.isExitAnimationComplete())
            {
                roundEnded = false;
                game.resetGame(renderer, worldSystem);
                game.setState(GameState::PLAYING);
            }

            glWindow.windowSwapBuffers();
            break;
        default:
            std::cerr << "unhandled game state" << std::endl;
            /*handleUtilityInputs(renderer, showFPS, botEnabled,
                fKeyPressed, bKeyPressed, hKeyPressed,
                glWindow, fpsCounter, shouldExit,
                worldSystem);*/
            glWindow.windowSwapBuffers();
            if (shouldExit)
                break;
            break;
        }
        if (shouldExit)
            break;
    }

    // Cleanup in correct order
    SDL_Quit();
    return 0;
}
