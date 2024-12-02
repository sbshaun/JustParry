#pragma once
#include "../external/project_path.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "../common.hpp"
#include "../ecs/ecs.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../ecs/components.hpp"
#include "mesh.hpp"
#include "../window.hpp"
#include <array>
#include <chrono>

// fonts
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

// Forward declare Game class
class Game;

// font character structure
struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
    char character;
};

class GlRender
{
public:
    GlRender();
    ~GlRender();
    void setGameInstance(Game *gameInstance) { game = gameInstance; }

    void initialize();
    void render();
    void renderRoundOver(int count);
    void loadTextures();
    void loadTexture(const std::string &path, GLuint &textureID);
    void renderUI(int timer);
    void shutdown();

    // helpers
    Game *getGameInstance() { return game; }
    void handleTexturedRenders();
    void handleNotifications(float elapsed_ms);
    void handleStaticRenders();

    void renderTexturedQuad(GLuint texture);
    void renderFPS(int fps, bool showFPS);

    void renderButton(float x, float y, float width, float height, const char *text,
                      bool hovered = false, bool pressed = false,
                      glm::vec3 color = glm::vec3(0.4078f, 0.4549f, 0.5020f));

    void renderTexturedQuadScaled(GLuint texture, float x, float y, float width, float height,
                                  float brightness = 1.0f, float alpha = 1.0f);

    void renderDebugBoxes(Entity entity, const Box &box, const glm::vec3 &color);
    void renderPlayableArea(Entity entity, const vec2 &position, float width, float height, const glm::vec3 &color);

    // bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
    void initializeGlMeshes();

    template <class T>
    void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);
    ObjectMesh &getMesh(GEOMETRY_BUFFER_ID gid) { return meshes[(int)gid]; }

    bool debugMode = false;

    mat4 m_worldModel = mat4(1);
    Entity m_player1;
    Entity m_player2;
    Entity m_playableArea;
    GLuint m_menuTexture;
    GLuint m_arcadeMenuTexture;
    GLuint m_helpTexture1;
    GLuint m_helpTexture2;
    GLuint m_helpTexture3;
    GLuint m_settingsTexture;
    GLuint m_bg1Texture;
    GLuint m_bg2Texture;
    GLuint m_bg3Texture;
    GLuint m_bg4Texture;
    GLuint m_countdown_3;
    GLuint m_countdown_2;
    GLuint m_countdown_1;
    GLuint m_countdown_fight;

    GLuint m_notif_parried;
    GLuint m_notif_hit;
    GLuint m_notif_stunned;

    GLuint m_roundOverP1Texture;
    GLuint m_roundOverP2Texture;
    GLuint m_timerBackgroundTexture;
    GLuint m_barTexture;
    GLuint m_avatarTexture;
    GLuint m_pauseMenuTexture;

    GLuint m_characterSelectTexture;
    GLuint m_characterSelectTextureArcade;

    GLuint m_character1;
    GLuint m_character1_flip;
    GLuint m_character1_ready;
    GLuint m_character1_flip_ready;

    GLuint m_character1_bear;
    GLuint m_character1_bear_flip;
    GLuint m_character1_bear_ready;
    GLuint m_character1_bear_flip_ready;

    GLuint m_character1_red;
    GLuint m_character1_flip_red;
    GLuint m_character1_ready_red;
    GLuint m_character1_flip_ready_red;

    GLuint m_character1_green;
    GLuint m_character1_flip_green;
    GLuint m_character1_ready_green;
    GLuint m_character1_flip_ready_green;

    GLuint m_character1_blue;
    GLuint m_character1_flip_blue;
    GLuint m_character1_ready_blue;
    GLuint m_character1_flip_ready_blue;

    GLuint m_p1SelectKey;
    GLuint m_p2SelectKey;
    GLuint bird_Story_1_1;
    GLuint bird_Story_1_2;
    GLuint bird_Story_1_3;
    GLuint bird_Story_1_4;
    GLuint bird_Story_1_5;
    GLuint bird_Story_1_6;
    GLuint bird_Story_1_7;
    GLuint bird_Story_2_1;
    GLuint bird_Story_2_2;
    GLuint bird_Story_2_3;
    GLuint bird_Story_2_4;
    GLuint bird_Story_2_5;
    GLuint bird_Story_2_6;
    GLuint bird_Story_3_1;
    GLuint bird_Story_3_2;
    GLuint bird_Story_3_3;
    GLuint bird_Story_3_4;
    GLuint bird_Story_3_5;
    GLuint bird_Story_3_6;
    GLuint bird_Story_3_7;
    GLuint bird_Story_3_8;
    GLuint bird_Story_3_9;
    GLuint bird_Story_3_10;
    GLuint bird_Story_4_1;
    GLuint bird_Story_4_2;
    GLuint bird_Story_4_3;
    GLuint bird_Story_4_4;
    GLuint bird_Story_4_5;
    GLuint bird_Story_4_6;
    GLuint bird_Story_4_7;
    GLuint bird_Story_4_8;
    GLuint bird_Story_4_9;
    GLuint bird_Story_E_1;
    GLuint bird_Story_E_2;
    GLuint bird_Story_E_3;
    GLuint bird_Story_E_4;
    GLuint bird_Story_E_5;
    GLuint bird_Story_E_6;
    GLuint bird_Story_E_7;
    GLuint bird_Story_E_8;
    GLuint bird_Story_E_9;

    GLuint m_levelWonTexture;
    GLuint m_levelLostTexture;
    GLuint m_matchOverP1Texture;
    GLuint m_matchOverP2Texture;
    GLuint m_roundOverDrawTexture;
    GLuint m_arcadeStoryTexture;

    // font elements
    std::map<char, Character> m_ftCharacters;
    GLuint m_font_VAO;
    GLuint m_font_VBO;

    // Make sure these paths remain in sync with the associated enumerators.
    // Associated id with .obj path
    const std::vector<std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
        {
            std::pair<GEOMETRY_BUFFER_ID, std::string>(
                GEOMETRY_BUFFER_ID::IDLE_BIRD, mesh_path("idle_bird.obj")),
            // specify meshes of other assets here
    };

    std::array<GLuint, geometry_count> vertex_buffers;
    std::array<GLuint, geometry_count> index_buffers;
    std::array<ObjectMesh, geometry_count> meshes;

    void resetRoundOverAnimation();

    bool isExitAnimationStarted() const { return m_exitAnimationStarted; }
    bool isExitAnimationComplete() const { return m_animationComplete; }
    void startExitAnimation()
    {
        m_exitAnimationStarted = true;
        m_animationProgress = 0.0f;
    }

    // Make these accessible to Game class
    void setAnimationComplete(bool value) { m_animationComplete = value; }
    void setExitAnimationStarted(bool value) { m_exitAnimationStarted = value; }

    bool fontInit(const std::string &font_filename, unsigned int font_default_size);
    void renderText(std::string text, float x, float y, float scale, const glm::vec3 &color);

    void renderSimpleButton(float x, float y, float width, float height,
                            bool isSelected, bool isHovered, bool isPressed,
                            glm::vec3 tintColor = glm::vec3(0.6f, 0.6f, 0.6f));

    // for rendering player health box
    void renderRedHealthRectangle(float x, float y, float width, float height);
    void renderBlueHealthRectangle(float x, float y, float width, float height);
    void handleP1Health(float p1Health);
    void handleP2Health(float p2Health);

    void handleP1Posture(int p1Posture);
    void handleP2Posture(int p2Posture);

    void renderSelectorTriangleP1(float x, float y, float width, float height, bool p1);
    void renderSelectorTriangleP2(float x, float y, float width, float height, bool p2);

private:
    Game *game = nullptr;

    // Add shader member variables
    Shader *m_debugShader = nullptr;
    Shader *m_player1Shader = nullptr;
    Shader *m_player2Shader = nullptr;
    Shader *m_floorShader = nullptr;
    Shader *m_hitboxShader = nullptr;
    Shader *m_fontShader = nullptr;
    Shader *m_buttonShader = nullptr;
    Shader *m_simpleButtonShader = nullptr;
    Shader *m_redRectangleShader = nullptr;

    // Round over screen animation properties
    float m_roundOverY = -600.0f;        // Starting Y position (off screen)
    float m_targetY = 0;                 // Final Y position
    const float SLIDE_SPEED = 50.0f;     // Animation speed in pixels per second
    float m_animationProgress = 0.0f;    // Animation progress (0 to 1)
    const float ANIMATION_DELAY = 1.0f;  // Delay before animation starts
    float m_delayTimer = 0.0f;           // Timer to track delay
    bool m_animationStarted = false;     // Track if animation has started
    bool m_exitAnimationStarted = false; // Track if exit animation has started
    float m_exitY = -750.0f;             // Changed from -600 to -750 pixels
    std::chrono::steady_clock::time_point m_lastUpdateTime = std::chrono::steady_clock::now();
    bool m_animationComplete = false; // Track if exit animation is complete
};