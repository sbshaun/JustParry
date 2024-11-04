#pragma once
#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "common.hpp"
#include "ecs/ecs.hpp"
#include "ecs/ecs_registry.hpp"
#include "ecs/components.hpp"
#include "mesh.hpp"
#include <array>

#define GLT_IMPLEMENTATION
#include <GLText.h>

// Forward declare Game class
class Game;

class GlRender
{
public:
    GlRender();
    ~GlRender();
    void setGameInstance(Game *gameInstance) { game = gameInstance; }

    void initialize();
    void drawUI();
    void render();
    void renderRoundOver(int count);
    void loadTextures();
    void loadTexture(const std::string &path, GLuint &textureID);
    void renderUI(int timer);
    void shutdown();

    // helpers
    void handleTexturedRenders();
    void handleHitboxRenders();
    void handleStaticRenders();

    void renderTexturedQuad(GLuint texture);
    void renderLoadingText();
    void renderFPS(int fps, bool showFPS);

    void renderButton(float x, float y, float width, float height, const char *text,
                      bool hovered = false, bool pressed = false,
                      glm::vec3 color = glm::vec3(0.4078f, 0.4549f, 0.5020f));
    void renderDialogBox(float x, float y, float width, float height, float alpha = 0.8f);

    void renderTexturedQuadScaled(GLuint texture, float x, float y, float width, float height, float brightness = 1.0f);

    void renderDebugBoxes(Entity entity, const Box &box, const glm::vec3 &color);
    // bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
    void initializeGlMeshes();

    template <class T>
    void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);
    ObjectMesh& getMesh(GEOMETRY_BUFFER_ID gid) { return meshes[(int)gid]; }

    bool debugMode = false;

    Entity m_player1;
    Entity m_player2;
    GLuint m_bird_texture;
    GLuint m_bird_p_texture;
    GLuint m_backgroundTexture;

    // Make sure these paths remain in sync with the associated enumerators.
    // Associated id with .obj path
    const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
    {
          std::pair<GEOMETRY_BUFFER_ID, std::string>(
              GEOMETRY_BUFFER_ID::IDLE_BIRD, mesh_path("idle_bird.obj")
          ),
          // specify meshes of other assets here
    };

    std::array<GLuint, geometry_count> vertex_buffers;
    std::array<GLuint, geometry_count> index_buffers;
    std::array<ObjectMesh, geometry_count> meshes;

private:
    Game *game = nullptr;

    // Add shader member variables
    Shader *m_debugShader = nullptr;
    Shader *m_player1Shader = nullptr;
    Shader *m_player2Shader = nullptr;
    Shader *m_floorShader = nullptr;
    Shader *m_hitboxShader = nullptr;

    // Place holders for timer and health subtexts
    GLTtext *m_fps = nullptr;
    GLTtext *m_loadingText = nullptr;

    GLTtext *m_restart = nullptr;

    GLTtext *m_timerText = nullptr;
    GLTtext *m_leftText = nullptr;
    GLTtext *m_rightText = nullptr;
    GLTtext *m_roundOver = nullptr;

    // actual values
    GLTtext *h1 = nullptr;
    GLTtext *h2 = nullptr;
    GLTtext *time = nullptr;

    // round over
    GLTtext *over = nullptr;
    GLTtext *won = nullptr;

    // Add score text objects
    GLTtext *score1 = nullptr;
    GLTtext *score2 = nullptr;
    GLTtext *score1Label = nullptr;
    GLTtext *score2Label = nullptr;
};