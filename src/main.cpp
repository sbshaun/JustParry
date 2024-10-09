#define GL3W_IMPLEMENTATION
#include <gl3w.h>

#include "window.hpp"
#include "renderer.hpp"
#include <assert.h>

int main() {
    GLWindow glWindow(1024, 768);
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

    exit(0);
}