#pragma once
#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "common.hpp"

#define GLT_IMPLEMENTATION
#include <GLText.h>

class GlRender {
public:
    GlRender();
    void initialize();
    void initializeUI();
    void render();
    void renderUI(int timer);
    void shutdown();

    void addMesh(const Mesh& mesh, Shader* shader);

    ~GlRender();

private:
    std::vector<std::pair<Mesh, Shader*>> m_entityMeshes;

    // Place holders for timer and health subtexts
    GLTtext* m_timerText;
    GLTtext* m_leftText;
    GLTtext* m_rightText;

    // actual values
    GLTtext* h1;
    GLTtext* h2;
    GLTtext* time;
};