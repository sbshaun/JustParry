#pragma once
#include "common.hpp"


class GLWindow {
private:
    int m_window_width_px;
    int m_window_height_px;

    bool m_close_window = false;
public:
    GLFWwindow* window;

    GLWindow();
	GLWindow(int window_width_px, int window_height_px);

    ~GLWindow();

    int window_width_px();
    int window_height_px();

    GLFWwindow* create_window();

    bool shouldClose();

	void on_key(int key, int, int action, int mod);

    void windowSwapBuffers();

    void windowShutdown();
};