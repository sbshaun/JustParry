#define GL3W_IMPLEMENTATION
#include <gl3w.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <iostream>

// ADAPTED FROM SIMPLEGL
class GLWindow {
private:
    int m_window_width_px;
    int m_window_height_px;

    bool m_close_window = false;

public:
    GLFWwindow* window;

    GLWindow()
    {
        // use default values
        m_window_width_px = 640;
        m_window_height_px = 480;
        window = create_window();
    }

    GLWindow(int window_width_px, int window_height_px)
        : m_window_width_px(window_width_px), m_window_height_px(window_height_px)
    {
        window = create_window();
    }

    ~GLWindow()
    {
        if (window) {
            // close and release the window
            glfwDestroyWindow(window);
        }
    }

    int window_width_px() { return m_window_width_px; }
    int window_height_px() { return m_window_height_px; }

    GLFWwindow* create_window()
    {
        // init GLFW first
        if (!glfwInit()) {
            std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
            return nullptr;
        }

        // OpenGL config -- version 3.3 core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // create the window
        window = glfwCreateWindow(m_window_width_px, m_window_height_px, "simpleGL", nullptr, nullptr);
        if (window == nullptr) {
            std::cerr << "ERROR: Failed to glfwCreateWindow" << std::endl;
            // glfwTerminate();
            return nullptr;
        }

        // register calls for key presses
        // Setting callbacks to member functions (that's why the redirect is needed)
        // Input is handled using GLFW, for more info see
        // http://www.glfw.org/docs/latest/input_guide.html
        glfwSetWindowUserPointer(window, this);
        auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GLWindow*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
        glfwSetKeyCallback(window, key_redirect);

        return window;
    }

    bool shouldClose() {
        return bool(glfwWindowShouldClose(window)) || m_close_window;
    }

    void on_key(int key, int, int action, int mod) {

        (void)mod;	// remove unused warning

        // ESC key to exit
        if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
            std::cout << "INFO: ESC key pressed, close window" << std::endl;
            m_close_window = true;
        }
    }
};

int main() {

    GLWindow glWindow(1024, 768);
    glfwMakeContextCurrent(glWindow.window);

    // Load OpenGL function pointers... before we render or you will segfault below...
    const int is_fine = gl3w_init();
    assert(is_fine == 0);

    // viewport - necessary if we allow re-sizing of window, which we do not
    glViewport(0, 0, glWindow.window_width_px(), glWindow.window_width_px());

    GLuint frame_buffer;
    frame_buffer = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // check if the pixel buffer is as expected... apparently not the case for some high DPI displays (ex. Retina Display on Macbooks)
    // https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
    int frame_buffer_width_px, frame_buffer_height_px;
    glfwGetFramebufferSize(glWindow.window, &frame_buffer_width_px, &frame_buffer_height_px);  // Note, this will be 2x the resolution given to glfwCreateWindow on Mac Retina displays
    if (frame_buffer_width_px != glWindow.window_width_px())
    {
        std::cerr << "WARNING: retina display! https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value" << std::endl;
        std::cerr << "glfwGetFramebufferSize = " << frame_buffer_width_px << ", " << frame_buffer_height_px << std::endl;
        std::cerr << "window width_height = " << glWindow.window_width_px() << ", " << glWindow.window_height_px() << std::endl;
    }

    // game loop
    while (!glWindow.shouldClose()) {

        // we must poll for event or the window will stop responding
        glfwPollEvents();

        // clear the screen (RGB)
        glClearColor((255 / 255.0f), (255 / 255.0f), (255 / 255.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render
        //render.render(glWindow);

        glfwSwapBuffers(glWindow.window);
    }

    glDeleteFramebuffers(1, &frame_buffer);

    return ERROR_SUCCESS;
}