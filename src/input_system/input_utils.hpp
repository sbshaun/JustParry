#pragma once
#include <GLFW/glfw3.h>
#include "../common.hpp"

inline bool isKeyPressed(int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_PRESS;
}

inline bool isKeyReleased(int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

inline bool isControllerKeyPressed(int cid, int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    int size;
    std::cout << "PRESS !!!  CONTROLLER ID: " << cid << "BUTTON " << key << std::endl;
    return glfwGetJoystickButtons(cid, &size)[key] == GLFW_PRESS;
}

inline bool isControllerKeyReleased(int cid, int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    int size;
    return glfwGetJoystickButtons(cid, &size)[key] == GLFW_RELEASE;
}
