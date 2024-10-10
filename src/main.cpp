#define GL3W_IMPLEMENTATION
#include <gl3w.h>

#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>
#include "constants.hpp"
#include "ecs/ecs_registry.hpp" // TEMP: test it compiles. 
#include "world/world_init.hpp" // TEMP: test it compiles.

int main() {
    GLWindow glWindow(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    const int is_fine = gl3w_init();
    assert(is_fine == 0);

    // Enable vsync to avoid tearing. Whether synchronisation is enabled depends on the platform.
    // Siddh: Not sure if this is actually required for us, but it's here to prevent potential tearing.
    glfwSwapInterval(1);

	GlRender renderer;
    renderer.initialize();

    // Main loop
    while (!glWindow.shouldClose()) {
        renderer.render();
        glWindow.windowSwapBuffers();
    }

    // clean up
    renderer.shutdown();
    glWindow.windowShutdown();

    createPlayer1(&renderer, {0, 0}); // TEMP: tests itcompiles

    exit(0);
}