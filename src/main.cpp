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
    
    glfwSwapInterval(1); // Enable vsync

	GlRender renderer;
    renderer.initialize();

    std::vector<float> triangleVertices1 = {
        -0.75f,  0.5f, 0.0f,  // Left-top vertex
        -1.0f,  -0.5f, 0.0f,  // Left-bottom vertex
        -0.5f,  -0.5f, 0.0f   // Right-bottom vertex
    };

    std::vector<float> rectangleVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        -0.5f,  0.5f, 0.0f,   // Top-left
        -0.5f, -0.5f, 0.0f,   // Bottom-left
         0.5f, -0.5f, 0.0f,   // Bottom-right

         // Second triangle (Bottom-right, Top-right, Top-left)
          0.5f, -0.5f, 0.0f,   // Bottom-right
          0.5f,  0.5f, 0.0f,   // Top-right
         -0.5f,  0.5f, 0.0f    // Top-left
    };

    Mesh mesh1(triangleVertices1);
    Mesh mesh2(rectangleVertices);

    Shader* triangleShader = new Shader(std::string("triangle"));
    Shader* rectShader = new Shader(std::string("rectangle"));

    renderer.addMesh(mesh1, triangleShader);
    renderer.addMesh(mesh2, rectShader);

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