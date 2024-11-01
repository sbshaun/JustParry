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
    static bool roundEnded = false;

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

        renderer.render();
        renderer.renderUI(timer);
        renderer.renderRoundOver(1);

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            roundEnded = false;
            game.resetGame(renderer);
        }
    }
    else
    {
        renderer.render();
        int exit = generateUI(renderer);

        if (exit == 1)
        {
            if (!roundEnded)
            {
                game.updateScores(h1, h2);
                roundEnded = true;
            }

            p1 = PlayerInput();
            p2 = PlayerInput();
            renderer.renderRoundOver(0);
            renderer.renderUI(timer);

            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                roundEnded = false;
                game.resetGame(renderer);
            }
        }
        else
        {
            roundEnded = false;
            if (botEnabled)
            {
                botInstance.pollBotRng(renderer);
            }
            worldSystem.handleInput();
        }
    }
}

int main()
{
    // Initialize window and OpenGL
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    assert(gl3w_init() == 0);
    glfwSwapInterval(1); // Enable vsync

    // Initialize game systems
    FighterManager::init();
    GlRender renderer;
    renderer.initialize();

    WorldSystem worldSystem;
    worldSystem.init(&renderer);
    PhysicsSystem physics;
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

    // Main game loop
    while (!glWindow.shouldClose())
    {
        switch (game.getState())
        {
        case GameState::MENU:
            game.generateBackground(FLOOR_Y, renderer);
            game.renderMenu(renderer);
            if (game.handleMenuInput(glWindow.window))
            {
                game.setState(GameState::PLAYING);
            }
            break;

        case GameState::HELP:
            game.generateBackground(FLOOR_Y, renderer);
            game.renderHelpScreen(renderer);
            if (game.handleHelpInput(glWindow.window))
            {
                game.setState(GameState::MENU);
            }
            break;

        case GameState::ROUND_START:
            renderer.initializeUI();
            interp_moveEntitesToScreen(renderer);
            renderer.render();
            renderer.renderUI(timer);

            if (!isLoading)
            {
                game.setState(GameState::PLAYING);
            }
            break;

        case GameState::PLAYING:
            renderer.initializeUI();
            interp_moveEntitesToScreen(renderer);
            worldSystem.inputProcessing(timer);
            worldSystem.movementProcessing();
            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);
            worldSystem.handle_collisions();
            worldSystem.playerCollisions(&renderer);
            physics.step();

            checkIsRoundOver(renderer, botInstance, worldSystem, game, botEnabled);
            handleUtilityInputs(renderer, showFPS, botEnabled,
                                fKeyPressed, bKeyPressed, hKeyPressed,
                                glWindow, fpsCounter, shouldExit,
                                worldSystem);
            if (shouldExit)
                break;
            break;

        default:
            std::cerr << "Unhandled game state" << std::endl;
            break;
        }

        if (shouldExit)
            break;

        glWindow.windowSwapBuffers();
    }

    // Cleanup
    renderer.shutdown();
    glWindow.windowShutdown();
    return 0;
}
