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

    if (h1.currentHealth <= 0 || h2.currentHealth <= 0)
    {
        if (!roundEnded)
        {
            game.updateScores(h1, h2);
            roundEnded = true;
        }

        p1 = PlayerInput();
        p2 = PlayerInput();

        renderer.renderRoundOver(1);
        game.setState(GameState::ROUND_OVER);
    }
    else
    {

        int exit = generateUI(renderer);

        if (exit == 1)
        {
            if (!roundEnded)
            {
                game.updateScores(h1, h2);
                roundEnded = true;
                game.setState(GameState::ROUND_OVER);
            }

            p1 = PlayerInput();
            p2 = PlayerInput();
            renderer.renderRoundOver(0);
        }
        else
        {
            roundEnded = false;
            if (botEnabled)
            {
                botInstance.pollBotRng(renderer);
            }
        }
    }
}

int main()
{
    //// START INITS ////
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    assert(gl3w_init() == 0);
    // assert(is_fine == 0);

    // glfwSwapInterval(1); // Enable vsync

    // init fighters config
    FighterManager::init();

    GlRender renderer;
    renderer.initialize();

    WorldSystem worldSystem;
    worldSystem.init(&renderer);
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

    // Main loop

    while (!glWindow.shouldClose())
    {
        // start a timer for each loop
        auto start = std::chrono::steady_clock ::now();

        switch (game.getState())
        {
        case GameState::MENU:
            game.renderMenu(renderer);
            if (game.handleMenuInput(glWindow.window))
            {
                game.setState(GameState::ROUND_START);
            }
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
                game.setState(GameState::MENU);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::ROUND_START:

            renderer.drawUI();
            interp_moveEntitesToScreen(renderer); // Move players into position

            renderer.render();
            renderer.renderUI(timer);

            if (!isLoading)
            {
                game.setState(GameState::PLAYING);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::PLAYING:
        {
            if (loopsSinceLastFrame == FramesPerLogicLoop)
            { // Only do certain checks each frame rather than every loop
                // std::cout << "RENDER CALL" << std::endl;
                loopsSinceLastFrame = 0;
                renderer.drawUI();
                interp_moveEntitesToScreen(renderer);
                handleUtilityInputs(renderer, showFPS, botEnabled,
                                    fKeyPressed, bKeyPressed, hKeyPressed,
                                    glWindow, fpsCounter, shouldExit,
                                    worldSystem);
                // toggleFPS(renderer, showFPS, fKeyPressed, glWindow, fpsCounter);
                glWindow.windowSwapBuffers();
            }

            loopsSinceLastFrame++;

            worldSystem.movementProcessing(); // PROCESS MOVEMENTS BASED ON THE DECISIONS MADE BY FRAME BUFFER
            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);
            worldSystem.hitBoxCollisions();
            worldSystem.inputProcessing();

            // Update center for playable area
            PlayableArea &playableArea = registry.playableArea.get(renderer.m_playableArea);
            Motion &player1Motion = registry.motions.get(renderer.m_player1);
            Motion &player2Motion = registry.motions.get(renderer.m_player2);
            playableArea.updatePosition(player1Motion.position, player2Motion.position);
            playableArea.updateWorldModel(renderer.m_worldModel);

            worldSystem.playerCollisions(&renderer);
            physicsSystem.step();
            renderer.render();
            renderer.renderUI(timer);

            checkIsRoundOver(renderer, botInstance, worldSystem, game, botEnabled);
            worldSystem.handleInput();
            // toggleBot(botEnabled, bKeyPressed, glWindow);

            // time the next logic check
            auto end = std::chrono::steady_clock ::now();
            std::chrono::duration<double, std::milli> FastLoopIterTime = end - start;

            // Calculate the remaining time to sleep
            int sleepDuration = targetLogicDuration - static_cast<int>(FastLoopIterTime.count());
            // std::cout << "i wanna sleep for " << sleepDuration << std::endl;
            if (sleepDuration > 0)
            {
                auto sleepEnd = std::chrono::steady_clock::now() + std::chrono::milliseconds(sleepDuration);
                while (std::chrono::steady_clock::now() < sleepEnd)
                {
                    worldSystem.handleInput();
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
            renderer.drawUI();
            renderer.render();
            renderer.renderUI(timer);
            renderer.renderRoundOver(1);
            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                roundEnded = false;
                game.resetGame(renderer, worldSystem);
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
    registry.clear_all_components(); // Clear ECS components first
    renderer.shutdown();             // Then shutdown renderer
    glWindow.windowShutdown();       // Finally close window

    return 0;
}
