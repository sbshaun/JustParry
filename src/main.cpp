#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include "bot.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "physics/physics_system.hpp"
#include "ecs/ecs_registry.hpp"   // TEMP: test it compiles.
#include "world/world_system.hpp" // TEMP: test it compiles.
#include "linearinterp.hpp"
#include "state/game.hpp"
#include "fps/fps.cpp"

#include "input_system/input_handler.hpp" // test compilation
#include "input_system/state_machine.hpp" // test compilation

int timer = timer_length; // Global timer init var moved to constants
auto last_time = std::chrono::high_resolution_clock::now();

int generateUI(GlRender &renderer)
{

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_time;
    // update timer each second, game stops at 0.
    if (elapsed.count() >= 1.0 && timer > 0)
    {
        last_time = current_time;
        timer--;
        // std::cout << timer << std::endl;
    }

    if (timer <= 0)
    {
        return 1;
    }

    // Render the timer and additional text on the screen
    renderer.renderUI(timer);
    return 0;
}

// [m1] temp fn to check is round is over.
void checkIsRoundOver(GlRender &renderer, Bot &botInstance, WorldSystem &worldSystem, Game &game, bool &botEnabled)
{
    static bool roundEnded = false;

    int exit = 0;
    Health &h1 = registry.healths.get(renderer.m_player1);
    Health &h2 = registry.healths.get(renderer.m_player2);
    PlayerInput &p1 = registry.playerInputs.get(renderer.m_player1);
    PlayerInput &p2 = registry.playerInputs.get(renderer.m_player2);

    // First condition: Check for health-based game over
    if (h1.currentHealth <= 0 || h2.currentHealth <= 0)
    {
        // Update scores only once when the round ends
        if (!roundEnded)
        {
            game.updateScores(h1, h2);
            roundEnded = true;
        }

        // pause the timer and game controls
        p1 = PlayerInput();
        p2 = PlayerInput();

        renderer.render();
        renderer.renderUI(timer);
        renderer.renderRoundOver(1);

        // Check for restart input
        GLFWwindow *window = glfwGetCurrentContext();
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            roundEnded = false; // Reset the flag when restarting
            game.resetGame(renderer);
        }
    }
    else
    {
        renderer.render();
        exit = generateUI(renderer);

        // Second condition: Check for timer expiration
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

            // Check for restart input
            GLFWwindow *window = glfwGetCurrentContext();
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                roundEnded = false; // Reset the flag when restarting
                game.resetGame(renderer);
            }
        }
        else
        {
            roundEnded = false; // Reset the flag during normal gameplay
            if (botEnabled)
            {
                botInstance.pollBotRng(renderer);
            }
            worldSystem.handleInput();
        }
    }
}

void toggleFPS(GlRender &renderer, bool &showFPS, bool &fKeyPressed, GLWindow &glWindow, FPSCounter &fpsCounter)
{
    if ((glfwGetKey(glWindow.window, GLFW_KEY_F) == GLFW_PRESS))
    {
        if (!fKeyPressed)
        {
            showFPS = !showFPS;
            fKeyPressed = true; // Update the state to prevent multiple toggles
            std::cout << "FPS COUNTER TOGGLED ON :" << fpsCounter.getFPS() << std::endl;
        }
    }
    else
    {
        // Reset fKeyPressed when the key is released
        fKeyPressed = false;
    }

    fpsCounter.update(renderer, showFPS);
}

void toggleBot(bool &botEnabled, bool &bKeyPressed, GLWindow &glWindow)
{
    if (glfwGetKey(glWindow.window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (!bKeyPressed)
        {
            botEnabled = !botEnabled;
            bKeyPressed = true; // Update the state to prevent multiple toggles
            std::cout << "BOT " << (botEnabled ? "ENABLED" : "DISABLED") << std::endl;
        }
    }
    else
    {
        // Reset bKeyPressed when the key is released
        bKeyPressed = false;
    }
}

int main()
{
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    const int is_fine = gl3w_init();
    assert(is_fine == 0);

    glfwSwapInterval(1); // Enable vsync

        // init fighters config 
        FighterManager::init(); 
        
        GlRender renderer;
        renderer.initialize();

    // Initialize the world system which is responsible for creating and updating entities
    WorldSystem worldSystem;
    // Initialize the world system which is responsible for creating and updating entities
    WorldSystem worldSystem;

    // Initialize the physics system that will handle collisions
    PhysicsSystem physics;
    Bot botInstance; // init bot

    worldSystem.init(&renderer);
    // Initialize the physics system that will handle collisions
    PhysicsSystem physics;
    Bot botInstance; // init bot

    worldSystem.init(&renderer);

    // Main loop

    Game game;
    renderer.setGameInstance(&game);
    FPSCounter fpsCounter(60); // Targeting 60 FPS

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // for fps toggle
    bool showFPS = false;
    bool fKeyPressed = false;

    // Declare motion variables outside switch
    Motion *m1_ptr = nullptr;
    Motion *m2_ptr = nullptr;

    // Add these variables with the other toggles
    bool bKeyPressed = false;
    bool botEnabled = BOT_ENABLED; // Initialize with the default value

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
            interp_moveEntitesToScreen(renderer); // Move players into position
            renderer.render();
            renderer.renderUI(timer);

            // Use pointers declared outside switch
            m1_ptr = &registry.motions.get(renderer.m_player1);
            m2_ptr = &registry.motions.get(renderer.m_player2);

            // Only transition to PLAYING when interpolation is complete
            if (!isLoading) // Use the global isLoading variable from linearinterp.cpp
            {
                game.setState(GameState::PLAYING);
            }
            break;

        case GameState::PLAYING:
            renderer.initializeUI();
    // Main loop

    Game game;
    renderer.setGameInstance(&game);
    FPSCounter fpsCounter(60); // Targeting 60 FPS

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // for fps toggle
    bool showFPS = false;
    bool fKeyPressed = false;

    // Declare motion variables outside switch
    Motion *m1_ptr = nullptr;
    Motion *m2_ptr = nullptr;

    // Add these variables with the other toggles
    bool bKeyPressed = false;
    bool botEnabled = BOT_ENABLED; // Initialize with the default value

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
            interp_moveEntitesToScreen(renderer); // Move players into position
            renderer.render();
            renderer.renderUI(timer);

            // Use pointers declared outside switch
            m1_ptr = &registry.motions.get(renderer.m_player1);
            m2_ptr = &registry.motions.get(renderer.m_player2);

            // Only transition to PLAYING when interpolation is complete
            if (!isLoading) // Use the global isLoading variable from linearinterp.cpp
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
            toggleFPS(renderer, showFPS, fKeyPressed, glWindow, fpsCounter);
            toggleBot(botEnabled, bKeyPressed, glWindow);
            break;
        }

        if (glfwGetKey(glWindow.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        glWindow.windowSwapBuffers();
    }

    // clean up
    renderer.shutdown();
    glWindow.windowShutdown();

    exit(0);
}
