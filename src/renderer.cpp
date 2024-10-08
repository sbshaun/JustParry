#include "renderer.hpp"
#include <iostream>

void GlRender::initialize() {
    if (gl3w_init()) {
        std::cerr << "Failed to initialize GL3W" << std::endl;
        return;
    }

    loadShaders();
    setupTriangle();
}

std::string GlRender::readShaderFile(const std::string& filename)
{
    std::cout << "Loading shader filename: " << filename << std::endl;

    std::ifstream ifs(filename);

    if (!ifs.good())
    {
        std::cerr << "ERROR: invalid filename loading shader from file: " << filename << std::endl;
        return "";
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::cout << oss.str() << std::endl;
    return oss.str();
}

void GlRender::setupTriangle() {
    float vertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set up the vertex attributes pointers
    // 0 = location (see vertex shader); 3 = vec3 [# vertices]; float type
    // GL_FALSE = don't normalize; 3 * sizeof = stride; 0 = offset in buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // enable the first attribute in the vertex shader (location = 0)
    glEnableVertexAttribArray(0);

    // unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GlRender::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(255.0f, 255.0f, 255.0f, 1.0f);

    // Use shader program and draw the triangle
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);

    // TODO: Extend this with the rest of the vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void GlRender::shutdown() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

void GlRender::loadShaders() {
    std::string vertexShaderSource = readShaderFile(PROJECT_SOURCE_DIR + std::string("shaders/triangle/vs.glsl"));
    std::string fragmentShaderSource = readShaderFile(PROJECT_SOURCE_DIR + std::string("shaders/triangle/fs.glsl"));
    const char* vertexShaderSource_c = vertexShaderSource.c_str();
    const char* fragmentShaderSource_c = fragmentShaderSource.c_str();

    // create and compile our vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource_c, NULL);
    glCompileShader(vertexShader);

    // create and compile our fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource_c, NULL);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    // clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}