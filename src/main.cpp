#define GL3W_IMPLEMENTATION
#include <gl3w.h>

#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "ecs/ecs_registry.hpp" // TEMP: test it compiles. 
#include "world/world_system.hpp" // TEMP: test it compiles.
#include <chrono>

// Global timer variables
int timer = 100;
auto last_time = std::chrono::high_resolution_clock::now();


void generateUI(GlRender& renderer) {
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_time;

    if (elapsed.count() >= 1.0 && timer >= 0) {
        last_time = current_time;
        std::cout << timer << std::endl;
        timer--;
    }

    // Render the timer and additional text on the screen
    renderer.renderUI(timer);
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

    // Main loop
    while (!glWindow.shouldClose()) {
        renderer.render();

        // render the UI.
        generateUI(renderer);
        
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