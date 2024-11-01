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
    //// START INITS ////
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    const int is_fine = gl3w_init();
    assert(is_fine == 0);

    // glfwSwapInterval(1); // Enable vsync

    // init fighters config 
    FighterManager::init(); 
    
    GlRender renderer;
    renderer.initialize();

    // Initialize the world system which is responsible for creating and updating entities
    WorldSystem worldSystem;
    worldSystem.init(&renderer);
    // Initialize the physics system that will handle collisions
    PhysicsSystem physics;
    Bot botInstance; // init bot

    worldSystem.init(&renderer);

   
    Game game;
    renderer.setGameInstance(&game);
    FPSCounter fpsCounter(60); // Targeting 60 FPS

    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // for fps toggle
    bool showFPS = false;
    bool fKeyPressed = true;

    // Declare motion variables outside switch
    Motion *m1_ptr = nullptr;
    Motion *m2_ptr = nullptr;

    // Add these variables with the other toggles
    bool bKeyPressed = false;
    bool botEnabled = BOT_ENABLED; // Initialize with the default value

    // Define the target frame duration (in milliseconds)
    const float targetLogicDuration = 1000 / 240; // 240 logic+input checks per second
    const float targetFrameDuration = 1000/ 60; // 60 FPS
        //Conservative estimate on 10 year old cpu: 4ms per iteration(incl render). = 125hz -> 120hz logic rate + 60fps cap

    double frameTimer = 0.0;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    //// END INITS ////

     // Main loop
    while (!glWindow.shouldClose())
    {
        //start a timer for each loop
        auto start = std::chrono::high_resolution_clock::now();

        switch (game.getState())
        {
        case GameState::MENU:
            game.generateBackground(FLOOR_Y, renderer);
            game.renderMenu(renderer);

            if (game.handleMenuInput(glWindow.window))
            {
                game.setState(GameState::PLAYING);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::HELP:
            game.generateBackground(FLOOR_Y, renderer);
            game.renderHelpScreen(renderer);

            if (game.handleHelpInput(glWindow.window))
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

            // Use pointers declared outside switch
            m1_ptr = &registry.motions.get(renderer.m_player1);
            m2_ptr = &registry.motions.get(renderer.m_player2);

            // Only transition to PLAYING when interpolation is complete
            if (!isLoading) // Use the global isLoading variable from linearinterp.cpp
            {
                game.setState(GameState::PLAYING);
            }
            glWindow.windowSwapBuffers();
            break;

        case GameState::PLAYING:
            {
            // Calculate the elapsed time since the last frame
            auto currentFrameTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            // Increase frameTimer by the elapsed time
            frameTimer += elapsed.count();

            if(frameTimer >= targetFrameDuration){ //Only do certain checks each frame rather than every loop
                std::cout << frameTimer << "FRAME TIME" << std::endl;
                frameTimer = 0; //reset timer 
                
                renderer.drawUI();
                interp_moveEntitesToScreen(renderer);
                
                worldSystem.movementProcessing(); //PROCESS MOVEMENTS BASED ON THE DECISIONS MADE BY FRAME BUFFER
                worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP);
                worldSystem.hitBoxCollisions(); 

                checkIsRoundOver(renderer, botInstance, worldSystem, game, botEnabled);
                toggleFPS(renderer, showFPS, fKeyPressed, glWindow, fpsCounter);
                toggleBot(botEnabled, bKeyPressed, glWindow);
                glWindow.windowSwapBuffers();
            }
            worldSystem.inputProcessing(timer); //What are we passing a timer in for?
            physics.step();
            worldSystem.playerCollisions(&renderer);

            // auto end = std::chrono::high_resolution_clock::now();
            // std::chrono::duration<double, std::milli> elapsed = end - start;
            
            // // Calculate the remaining time to sleep
            // int sleepDuration = targetLogicDuration - static_cast<int>(elapsed.count());
            // if (sleepDuration > 0)
            // {
            //     std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
            // }

            // auto actualEnd = std::chrono::high_resolution_clock::now();
            // std::chrono::duration<double, std::milli> actualFrameDuration = actualEnd - start;
            // std::cout << "Actual frame duration: " << actualFrameDuration.count() << " ms" << std::endl;

            }
            break;

        case GameState::LOADING:
            // supress compilation warning 
            std::cout << "warning: enumeration value 'LOADING' not handled in switch [-Wswitch]" << std::endl;
            glWindow.windowSwapBuffers();
            break;

        default:
            std::cerr << "unhandled game state" << std::endl;
            glWindow.windowSwapBuffers();
            break;
        }
        if (glfwGetKey(glWindow.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }
        

    }

    // clean up
    renderer.shutdown();
    glWindow.windowShutdown();

    exit(0);
}
