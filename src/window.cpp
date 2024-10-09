#include "window.hpp";

GLWindow::GLWindow() {
    m_window_width_px = 1024;
    m_window_height_px = 768;
    window = create_window();
}

GLWindow::GLWindow(int window_width_px, int window_height_px)
    : m_window_width_px(window_width_px), m_window_height_px(window_height_px)
{
    window = create_window();
}

GLWindow::~GLWindow()
{
    if (window) {
        // close and release the window
        glfwDestroyWindow(window);
    }
}

int GLWindow::window_width_px() { return m_window_width_px; }
int GLWindow::window_height_px() { return m_window_height_px; }

GLFWwindow* GLWindow::create_window() {
    // Initialize GLFWX
    if (!glfwInit()) {
        //glfw down
        exit(EXIT_FAILURE);
    }

    // Tell GLFW we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Tell GLFW we want core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enable debug context
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Tell GLFW to not be able to resize the window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(m_window_width_px, m_window_height_px, "Just Parry", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "ERROR: Failed to glfwCreateWindow" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // register calls for key presses
    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GLWindow*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    glfwSetKeyCallback(window, key_redirect);

    glViewport(0, 0, window_width_px(), window_width_px());

    glfwMakeContextCurrent(window);

    return window;
}

bool GLWindow::shouldClose() {
    return bool(glfwWindowShouldClose(window)) || m_close_window;
}

void GLWindow::on_key(int key, int, int action, int mod) {
    // ESC key to exit
    if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
        std::cout << "INFO: ESC key pressed, close window" << std::endl;
        m_close_window = true;
    }
}

void GLWindow::windowSwapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void GLWindow::windowShutdown() {
	glfwDestroyWindow(window);
	glfwTerminate();
}