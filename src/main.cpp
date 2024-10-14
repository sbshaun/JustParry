#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include "bot.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "physics/physics_system.hpp"
#include "ecs/ecs_registry.hpp" // TEMP: test it compiles. 
#include "world/world_system.hpp" // TEMP: test it compiles.

int timer = timer_length; //Global timer init var moved to constants
auto last_time = std::chrono::high_resolution_clock::now();

int generateUI(GlRender& renderer) {


    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_time;
    // update timer each second, game stops at 0.
    if (elapsed.count() >= 1.0 && timer > 0) {
        last_time = current_time;
        timer--;
        std::cout << timer << std::endl;
    }

    if (timer <= 0) {
        return 1;
    }

    // Render the timer and additional text on the screen
    renderer.renderUI(timer);
    return 0;


}

// [m1] temp fn to check is round is over.
void checkIsRoundOver(GlRender& renderer, Bot& botInstance, WorldSystem& worldSystem) {
    int exit = 0;
    Health& h2 = registry.healths.get(renderer.m_player2);
    PlayerInput& p1 = registry.playerInputs.get(renderer.m_player1);
    PlayerInput& p2 = registry.playerInputs.get(renderer.m_player2);

    if (h2.currentHealth <= 0 || exit == 1) {
        // pause the timer and game cotrols, and display Game over text 
        h2.currentHealth = 0;
        p1 = PlayerInput();
        p2 = PlayerInput();

        renderer.render();
        renderer.renderUI(timer);
        renderer.renderRoundOver(1);
    }
    else {
        renderer.render();
        exit = generateUI(renderer);
        if (exit == 1) {
            renderer.renderRoundOver(0);
            p1 = PlayerInput();
            p2 = PlayerInput();
            renderer.renderUI(timer);
        }
        else {
            // run bot movements
            if(BOT_ENABLED)botInstance.pollBotRng(renderer);
            worldSystem.handleInput(); //check if any devices keys are pressed
        }
    }
}

    int main() {
        GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
        const int is_fine = gl3w_init();
        assert(is_fine == 0);

        glfwSwapInterval(1); // Enable vsync

        GlRender renderer;
        renderer.initialize();

        // Initialize the world system which is responsible for creating and updating entities
        WorldSystem worldSystem;
        worldSystem.init(&renderer);

        // Initialize the physics system that will handle collisions
        PhysicsSystem physics;
        Bot botInstance; //init bot

        // Main loop
        while (!glWindow.shouldClose()) {
            worldSystem.inputProcessing(timer); //do the appropriate actions for key signals recieved
            worldSystem.movementProcessing(); //apply velocity for movement
            worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP); // update player states 
            worldSystem.handle_collisions(); // check hitbox/hurtbox collisions 
            worldSystem.playerCollisions(&renderer);
            physics.step(); //check for collisions

            // this fn calls renderer.render and also renders the UI.
            checkIsRoundOver(renderer, botInstance, worldSystem);
            if (glfwGetKey(glWindow.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                break; // Exit the loop and close the game
            }

            // worldSystem.step()
            glWindow.windowSwapBuffers();
        }
    

    // clean up
    renderer.shutdown();
    glWindow.windowShutdown();

    exit(0);
}