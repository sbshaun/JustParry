#pragma once
#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "common.hpp"
#include "ecs/ecs.hpp"
#include "ecs/ecs_registry.hpp"

#define GLT_IMPLEMENTATION
#include <GLText.h>

class GlRender {
public:
    GlRender();
    void initialize();
    void initializeUI();
    void render();
    void loadTextures();
    void loadTexture(const std::string& path, GLuint& textureID);
    void renderUI(int timer);
    void shutdown();

    ~GlRender();
    Entity m_player1;
    Entity m_player2;
    GLuint m_bird_texture;

private:
    // Place holders for timer and health subtexts
    GLTtext* m_timerText;
    GLTtext* m_leftText;
    GLTtext* m_rightText;
    GLTtext* m_roundOver;

    // actual values
    GLTtext* h1;
    GLTtext* h2;
    GLTtext* time;
};