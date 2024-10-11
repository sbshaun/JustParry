#include "renderer.hpp"
#include <iostream>

GlRender::GlRender() {}

GlRender::~GlRender() {}

void GlRender::initialize() {
    glEnable(GL_DEPTH_TEST);
}

void GlRender::render() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto& entity : m_entityMeshes) {
        Shader* shader = entity.second;
        shader->use();
        entity.first.draw();
    }
}

void GlRender::addMesh(const Mesh& mesh, Shader* shader) {
    m_entityMeshes.push_back({ mesh, shader });
}

void GlRender::shutdown() {
    m_entityMeshes.clear();

    std::cout << "Resources cleaned up." << std::endl;
}
