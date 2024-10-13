#define GL3W_IMPLEMENTATION
#include <gl3w.h>

#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "physics/physics_system.hpp"
#include "ecs/ecs_registry.hpp" // TEMP: test it compiles. 
#include "world/world_system.hpp" // TEMP: test it compiles.


// Global timer variables
int timer = 100;
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

    if (timer == 0) {
        return 1;
    }

    // Render the timer and additional text on the screen
    renderer.renderUI(timer);
    return 0;


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

    // Main loop
    while (!glWindow.shouldClose()) {
        worldSystem.handleInput(); //check if any devices keys are pressed

        
        worldSystem.inputProcessing(timer); //do the appropriate actions for key signals recieved
        worldSystem.movementProcessing(); //apply velocity for movement
        worldSystem.updateStateTimers(PLAYER_STATE_TIMER_STEP); // update player states 
        worldSystem.handle_collisions(); // check hitbox/hurtbox collisions 

        physics.step(); //check for collisions
        //worldSystem.handle_collisions(); //if there are collisions work through them accordingly
        renderer.render();

        // render the UI.
        int exit = generateUI(renderer);
        if (exit == 1) {
            break;
        }
        
        // TODO: Handle the movement and collision once implemented
        // worldSystem.step()
        // wordlSystem.handle_collisions()

        glWindow.windowSwapBuffers();
    }

    // clean up
    renderer.shutdown();
    glWindow.windowShutdown();

    exit(0);
}