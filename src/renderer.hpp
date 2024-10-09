#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include <vector>

class GlRender {
public:
    GlRender();
    void initialize();
    void prepareFrame();
    void render();
    void shutdown();

    ~GlRender();

private:
    Shader* m_shader;
    std::vector<Mesh> m_meshes;
};