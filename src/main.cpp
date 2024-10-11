#define GL3W_IMPLEMENTATION
#include <gl3w.h>

#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "ecs/ecs_registry.hpp" // TEMP: test it compiles. 
#include "world/world_system.hpp" // TEMP: test it compiles.

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