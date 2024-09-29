#include <gainput/gainput.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        //glfw down
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        //window not open
        glfwTerminate();
        return -1;
    }

    // adapted from simpleGL
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        //glew not working
        return -1;
    }

    // Initialize Gainput
    gainput::InputManager manager;

    
    // Main loop
    while (!glfwWindowShouldClose(window)){ 
        // Poll for and process events
        manager.Update(); //gainput device poll (hopefully not tied to framerate)
        glfwPollEvents(); //glfw needs to poll or else it crashes (steals resources off gainput D:)
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}