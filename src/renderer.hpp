#pragma once
#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "common.hpp"

class GlRender {
public:
    GlRender();
    void initialize();
    void render();
    void shutdown();

    void addMesh(const Mesh& mesh, Shader* shader);

    ~GlRender();

private:
    std::vector<std::pair<Mesh, Shader*>> m_entityMeshes;
};