#include "renderer.hpp"
#include <iostream>

GlRender::GlRender() {
    m_shader = nullptr;
    std::cout << "GlRender initialized." << std::endl;
}

GlRender::~GlRender() { 
    shutdown();
    std::cout << "GlRender destroyed." << std::endl;
}

void GlRender::initialize() {
    m_shader = new Shader(PROJECT_SOURCE_DIR + std::string("shaders/triangle/vs.glsl"), 
        PROJECT_SOURCE_DIR + std::string("shaders/triangle/fs.glsl"));

    if (!m_shader) {
        std::cerr << "Failed to create shader." << std::endl;
    }
    else {
        std::cout << "Shader created successfully." << std::endl;
    }

    std::vector<float> triangleVertices = {
        0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    m_meshes.push_back(Mesh(triangleVertices));

    std::cout << "Triangle vertices added." << std::endl;
}

void GlRender::prepareFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void GlRender::render() {
    prepareFrame();
    if (m_shader) {
        m_shader->use();
    }
    else {
        std::cerr << "Shader program is null!" << std::endl;
        return;
    }

    for (auto& mesh : m_meshes) {
        mesh.draw();
    }
}

void GlRender::shutdown() {
    delete m_shader;
    m_meshes.clear();

    std::cout << "Resources cleaned up." << std::endl;
}
