#include "renderer.hpp"
#include "common.hpp"
#include "state/game.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GlRender::GlRender() {}

GlRender::~GlRender()
{
    shutdown();
}

void GlRender::initialize()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load all textures
    loadTextures();
    gltInit();
    initializeGlMeshes();

    // Preload all shaders at initialization
    std::cout << "\nInitializing shaders..." << std::endl;

    try
    {
        // Store debug shader as a member variable instead of static
        m_debugShader = new Shader("debug");
        std::cout << "Debug visualization shader loaded" << std::endl;

        m_player1Shader = new Shader("player1");
        std::cout << "Player 1 shader loaded" << std::endl;

        m_player2Shader = new Shader("player2");
        std::cout << "Player 2 shader loaded" << std::endl;

        m_floorShader = new Shader("floor");
        std::cout << "Floor shader loaded" << std::endl;

        m_hitboxShader = new Shader("hitboxes");
        std::cout << "Hitbox shader loaded" << std::endl;

        std::cout << "All shaders initialized successfully\n"
                  << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing shaders: " << e.what() << std::endl;
    }
}

void GlRender::renderRoundOver(int count)
{
    over = gltCreateText();
    assert(over); // Check if it's not NULL
    gltSetText(over, "GAME OVER!");

    won = gltCreateText();
    assert(won); // Check if it's not NULL
    if (registry.healths.get(m_player1).currentHealth <= 0)
    {
        gltSetText(won, "Player 2 Wins!");
    }
    else
    {
        gltSetText(won, "Player 1 Wins!");
    }

    if (over && count == 1)
    {
        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f); // White text color

        // Draw game over text
        gltDrawText2D(over, 285, 170, 5.f);

        // Draw player wins text
        gltDrawText2D(won, 350, 250, 2.5f);
        gltEndDraw();
    }
    else
    {
        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f); // White text color

        // Draw game over text
        gltDrawText2D(over, 285, 170, 5.f);
        gltEndDraw();
    }
    m_restart = gltCreateText();
    assert(m_restart); // Check if it's not NULL
    gltSetText(m_restart, "PRESS ENTER TO RESTART!");

    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f); // White text color

    // Draw game over text
    gltDrawText2D(m_restart, 305, 300, 2.f);
    gltEndDraw();
}

void GlRender::drawUI()
{
    m_timerText = gltCreateText();
    assert(m_timerText); // Check if it's not NULL
    gltSetText(m_timerText, "TIMER");

    m_leftText = gltCreateText();
    assert(m_leftText);
    gltSetText(m_leftText, "P1 HEALTH: ");

    m_rightText = gltCreateText();
    assert(m_rightText);
    gltSetText(m_rightText, "P2 HEALTH: ");

    h1 = gltCreateText();
    assert(h1);
    gltSetText(h1, "100");

    h2 = gltCreateText();
    assert(h2);
    gltSetText(h2, "100");

    time = gltCreateText();
    assert(time);
    gltSetText(time, "100");

    m_roundOver = gltCreateText();
    assert(m_roundOver);
    gltSetText(m_roundOver, "ROUND OVER (please press esc and reset)");

    m_fps = gltCreateText();
    assert(m_fps);
    gltSetText(m_fps, "-1");

    // Initialize score labels
    score1Label = gltCreateText();
    assert(score1Label);
    gltSetText(score1Label, "SCORE: ");

    score2Label = gltCreateText();
    assert(score2Label);
    gltSetText(score2Label, "SCORE: ");

    score1 = gltCreateText();
    assert(score1);
    gltSetText(score1, "0");

    score2 = gltCreateText();
    assert(score2);
    gltSetText(score2, "0");
}
void GlRender::renderLoadingText()
{
    // Create text if it hasn't been created yet
    m_loadingText = gltCreateText();
    assert(m_loadingText); // Check if it's not NULL
    gltSetText(m_loadingText, "PRESS ENTER TO START!");

    glClear(GL_DEPTH_BUFFER_BIT);

    // Get the current time in seconds
    auto currentTime = std::chrono::steady_clock::now();
    auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();

    // Check if the text should be visible (alternates each second)
    if (timeInSeconds % 2 == 0)
    {
        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f); // White text color

        // Draw the loading text at the specified position and scale
        gltDrawText2D(m_loadingText, 50, 600, 5.4f);

        gltEndDraw();
    }
}

void GlRender::renderFPS(int fps, bool showFPS)
{

    if (showFPS)
    {
        std::string strH1 = std::to_string(fps);
        gltSetText(m_fps, strH1.c_str());

        gltBeginDraw();
        gltColor(0.0f, 1.0f, 0.0f, 1.0f); // White text color

        // Draw the loading text at the specified position and scale
        gltDrawText2D(m_fps, 10, 10, 2.f);

        gltEndDraw();
    }
}

void GlRender::handleTexturedRenders()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    for (Entity &entity : registry.renderable.entities)
    {
        Renderable &mesh_shader = registry.renderable.get(entity);
        Shader *shader = mesh_shader.shader;
        Mesh &mesh = mesh_shader.mesh;

        Player &player = registry.players.get(entity);

        shader->use();

        if (player.id == 1)
        {
            Motion &motion = registry.motions.get(entity);
            modelMatrix = m_worldModel;
            modelMatrix = glm::translate(modelMatrix, glm::vec3(motion.position.x, motion.position.y, 0.0f));

            // If player 1 is facing left, flip the model
            if (!registry.motions.get(entity).direction)
            {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(-motion.scale.x, motion.scale.y, 1.0f));
            }
            else
            {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(motion.scale.x, motion.scale.y, 1.0f));
            }

            PlayerCurrentState &player1State = registry.playerCurrentStates.get(entity);
            Renderable &player1Renders = registry.renderable.get(entity);

            if (player1State.currentState == PlayerState::ATTACKING)
            {
                player1Renders.texture = m_bird_p_texture;
            }
            else
            {
                player1Renders.texture = m_bird_texture;
            }

            // if parryBox is active, turn to transparent and green tint to show player is parrying
            if (registry.parryBoxes.get(m_player1).active)
            {
                shader->setBool("isParrying", true);
            }
            else
            {
                shader->setBool("isParrying", false);
            }

            if (registry.playerCurrentStates.get(m_player1).currentState == PlayerState::STUNNED)
            {
                shader->setBool("stunned", true);
            }
            else
            {
                shader->setBool("stunned", false);
            }

            if (registry.hitBoxes.get(m_player2).hit)
            {
                shader->setBool("takenDamage", true);
            }
            else
            {
                shader->setBool("takenDamage", false);
            }
        }
        else if (player.id == 2)
        {
            Motion &motion = registry.motions.get(entity);
            modelMatrix = m_worldModel;
            modelMatrix = glm::translate(modelMatrix, glm::vec3(motion.position.x, motion.position.y, 0.0f));

            // If player 2 is facing right, flip the model
            if (!registry.motions.get(entity).direction)
            {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(-motion.scale.x, motion.scale.y, 1.0f));
            }
            else
            {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(motion.scale.x, motion.scale.y, 1.0f));
            }

            if (registry.motions.get(entity).angle != 0)
            {
                modelMatrix = glm::rotate(modelMatrix, motion.angle, glm::vec3(motion.position.x, motion.position.y, 0.f));
            }
            PlayerCurrentState &player2State = registry.playerCurrentStates.get(entity);
            Renderable &player2Renders = registry.renderable.get(entity);

            if (player2State.currentState == PlayerState::ATTACKING)
            {
                player2Renders.texture = m_bird_p_texture;
            }
            else
            {
                player2Renders.texture = m_bird_texture;
            }

            if (registry.parryBoxes.get(m_player2).active)
            {
                shader->setBool("isParrying", true);
            }
            else
            {
                shader->setBool("isParrying", false);
            }

            if (registry.playerCurrentStates.get(m_player2).currentState == PlayerState::STUNNED)
            {
                shader->setBool("stunned", true);
            }
            else
            {
                shader->setBool("stunned", false);
            }

            if (registry.hitBoxes.get(m_player1).hit)
            {
                shader->setBool("takenDamage", true);
            }
            else
            {
                shader->setBool("takenDamage", false);
            }
        }

        shader->setMat4("model", modelMatrix);

        if (mesh_shader.texture)
        {
            glDepthMask(GL_FALSE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh_shader.texture);
            shader->setInt("m_bird_texture", 0);
        }

        mesh.draw();
    }

    // Add debug visualization at the end of the function
    if (debugMode)
    {
        if (registry.playableArea.has(m_playableArea))
        {
            PlayableArea &playableArea = registry.playableArea.get(m_playableArea);
            renderPlayableArea(m_playableArea, playableArea.position, playableArea.width, playableArea.height, glm::vec3(1.0f, 0.0f, 1.0f));
        }

        // Render hitboxes and hurtboxes for both players
        if (registry.hitBoxes.has(m_player1))
        {
            HitBox &p1HitBox = registry.hitBoxes.get(m_player1);
            if (p1HitBox.active)
            {
                // Bright red for player 1's hitbox
                renderDebugBoxes(m_player1, p1HitBox, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        if (registry.hurtBoxes.has(m_player1))
        {
            HurtBox &p1HurtBox = registry.hurtBoxes.get(m_player1);
            // Cyan for player 1's hurtbox
            renderDebugBoxes(m_player1, p1HurtBox, glm::vec3(0.0f, 0.8f, 1.0f));
        }

        if (registry.parryBoxes.has(m_player1))
        {

            ParryBox &p1ParryBox = registry.parryBoxes.get(m_player1);
            if (p1ParryBox.active)
            {
                // Green for player 1's parrybox
                renderDebugBoxes(m_player1, p1ParryBox, glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }

        if (registry.hitBoxes.has(m_player2))
        {
            HitBox &p2HitBox = registry.hitBoxes.get(m_player2);
            if (p2HitBox.active)
            {
                // Orange for player 2's hitbox
                renderDebugBoxes(m_player2, p2HitBox, glm::vec3(1.0f, 0.5f, 0.0f));
            }
        }

        if (registry.hurtBoxes.has(m_player2))
        {
            HurtBox &p2HurtBox = registry.hurtBoxes.get(m_player2);
            // Yellow for player 2's hurtbox
            renderDebugBoxes(m_player2, p2HurtBox, glm::vec3(1.0f, 1.0f, 0.0f));
        }

        if (registry.parryBoxes.has(m_player2))
        {
            ParryBox &p2ParryBox = registry.parryBoxes.get(m_player2);
            if (p2ParryBox.active)
            {
                // Purple for player 2's parrybox
                renderDebugBoxes(m_player2, p2ParryBox, glm::vec3(1.0f, 0.0f, 1.0f));
            }
        }
    }
}

void GlRender::handleHitboxRenders()
{
    // Empty this function since we're handling hitbox visualization in handleTexturedRenders
}

void GlRender::handleStaticRenders()
{
    for (Entity &static_entity : registry.staticRenders.entities)
    {
        StaticRender &staticRender = registry.staticRenders.get(static_entity);
        Shader *shader = staticRender.shader;
        Mesh &mesh = staticRender.mesh;

        shader->use();

        mesh.draw();
    }
}

void GlRender::render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // White background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PlayableArea &playableArea = registry.playableArea.get(m_playableArea);

    renderTexturedQuadScaled(
        m_backgroundTexture,
        -M_WINDOW_WIDTH_PX / 2 - playableArea.position.x * M_WINDOW_WIDTH_PX / 1.5, 0,
        M_WINDOW_WIDTH_PX * 2, M_WINDOW_HEIGHT_PX,
        1.0f);

    renderTexturedQuadScaled(
        m_foregroundTexture,
        -M_WINDOW_WIDTH_PX / 2 - playableArea.position.x * M_WINDOW_WIDTH_PX / 2.0, 0,
        M_WINDOW_WIDTH_PX * 2, M_WINDOW_HEIGHT_PX,
        1.0f);

    // debugging wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    handleStaticRenders();
    handleTexturedRenders();
    handleHitboxRenders();

    glDepthMask(GL_TRUE);
}

void GlRender::loadTextures()
{
    // Load texture for player 1
    loadTexture(textures_path("bird.png"), m_bird_texture);
    loadTexture(textures_path("bird_p.png"), m_bird_p_texture);
    loadTexture(textures_path("menu_1.png"), m_menuTexture);
    loadTexture(textures_path("help-screen.png"), m_helpTexture);
    loadTexture(textures_path("settings-screen.png"), m_settingsTexture);
    loadTexture(textures_path("bg.png"), m_backgroundTexture);
    loadTexture(textures_path("bg_parallax.png"), m_foregroundTexture);
}

void GlRender::loadTexture(const std::string &path, GLuint &textureID)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture from: " << path << std::endl;
    }
    stbi_image_free(data);
}

void GlRender::renderUI(int timer)
{
    const auto &playerhealths = registry.healths;
    const int p1Health = registry.healths.get(m_player1).currentHealth;
    const int p2Health = registry.healths.get(m_player2).currentHealth;

    if (m_timerText && m_leftText && m_rightText && game)
    {
        // Get the current window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

        // Calculate positions based on window size
        float centerX = windowWidth / 2.0f;
        float topY = windowHeight * 0.1f; // 10% from top

        // Player 1 health positions (left side)
        float p1X = centerX * 0.4f;
        float healthY = topY;
        float valueY = topY + 35.0f;

        // Timer positions (center)
        float timerX = centerX - 20.0f;
        float timerValueX = centerX - 10.0f;

        // Player 2 health positions (right side)
        float p2X = centerX * 1.6f;

        // Score positions
        float scoreY = topY + 75.0f;

        // Set up text
        std::stringstream ss;
        ss << timer;
        gltSetText(time, ss.str().c_str());

        std::string strH1 = std::to_string(p1Health);
        gltSetText(h1, strH1.c_str());

        std::string strH2 = std::to_string(p2Health);
        gltSetText(h2, strH2.c_str());

        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Draw Player 1 health text and value
        gltDrawText2D(m_leftText, p1X, healthY, 1.5f);
        gltDrawText2D(h1, p1X + 40.0f, valueY, 2.0f);

        // Draw timer text and value
        gltDrawText2D(m_timerText, timerX, healthY, 1.5f);
        gltDrawText2D(time, timerValueX, valueY, 2.5f);

        // Draw Player 2 health text and value
        gltDrawText2D(m_rightText, p2X, healthY, 1.5f);
        gltDrawText2D(h2, p2X + 40.0f, valueY, 2.0f);

        // Draw scores
        gltDrawText2D(score1Label, p1X, scoreY, 1.5f);
        std::string strScore1 = std::to_string(game->getPlayer1Score());
        gltSetText(score1, strScore1.c_str());
        gltDrawText2D(score1, p1X + 85.0f, scoreY, 1.5f);

        gltDrawText2D(score2Label, p2X, scoreY, 1.5f);
        std::string strScore2 = std::to_string(game->getPlayer2Score());
        gltSetText(score2, strScore2.c_str());
        gltDrawText2D(score2, p2X + 85.0f, scoreY, 1.5f);

        gltEndDraw();
    }
}

void GlRender::renderTexturedQuad(GLuint texture)
{
    // Define vertices for a full-screen quad (x, y, z, u, v)

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear with black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader *m_quadShader = new Shader("menu");
    float vertices[] = {
        // Positions    // Texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Top-left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f    // Top-right
    };

    // Indices for the quad (two triangles)
    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    // Setup VAO, VBO, EBO for quad rendering (if not already done)
    static GLuint VAO, VBO, EBO;
    if (VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Use shader for textured quad
    Shader *quadShader = m_quadShader; // You should have a shader for textured quads
    quadShader->use();
    quadShader->setInt("texture1", 0); // Set the texture uniform

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Render quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GlRender::shutdown()
{
    // Delete all shaders first
    for (Entity entity : registry.debugRenders.entities)
    {
        if (registry.debugRenders.has(entity))
        {
            HitboxRender &hitboxRender = registry.debugRenders.get(entity);
            if (hitboxRender.shader)
            {
                delete hitboxRender.shader;
                hitboxRender.shader = nullptr;
            }
        }
    }

    for (Entity entity : registry.staticRenders.entities)
    {
        if (registry.staticRenders.has(entity))
        {
            StaticRender &staticRender = registry.staticRenders.get(entity);
            if (staticRender.shader)
            {
                delete staticRender.shader;
                staticRender.shader = nullptr;
            }
        }
    }

    for (Entity entity : registry.renderable.entities)
    {
        if (registry.renderable.has(entity))
        {
            Renderable &renderable = registry.renderable.get(entity);
            if (renderable.shader)
            {
                delete renderable.shader;
                renderable.shader = nullptr;
            }
        }
    }

    // Delete textures
    glDeleteTextures(1, &m_bird_texture);
    glDeleteTextures(1, &m_bird_p_texture);
    glDeleteTextures(1, &m_menuTexture);
    glDeleteTextures(1, &m_helpTexture);
    glDeleteTextures(1, &m_settingsTexture);
    glDeleteTextures(1, &m_backgroundTexture);
    glDeleteTextures(1, &m_foregroundTexture);

    // Delete text objects
    if (m_fps)
    {
        gltDeleteText(m_fps);
        m_fps = nullptr;
    }
    if (m_loadingText)
    {
        gltDeleteText(m_loadingText);
        m_loadingText = nullptr;
    }
    if (m_restart)
    {
        gltDeleteText(m_restart);
        m_restart = nullptr;
    }
    if (m_timerText)
    {
        gltDeleteText(m_timerText);
        m_timerText = nullptr;
    }
    if (m_leftText)
    {
        gltDeleteText(m_leftText);
        m_leftText = nullptr;
    }
    if (m_rightText)
    {
        gltDeleteText(m_rightText);
        m_rightText = nullptr;
    }
    if (m_roundOver)
    {
        gltDeleteText(m_roundOver);
        m_roundOver = nullptr;
    }
    if (h1)
    {
        gltDeleteText(h1);
        h1 = nullptr;
    }
    if (h2)
    {
        gltDeleteText(h2);
        h2 = nullptr;
    }
    if (time)
    {
        gltDeleteText(time);
        time = nullptr;
    }
    if (over)
    {
        gltDeleteText(over);
        over = nullptr;
    }
    if (won)
    {
        gltDeleteText(won);
        won = nullptr;
    }
    if (score1)
    {
        gltDeleteText(score1);
        score1 = nullptr;
    }
    if (score2)
    {
        gltDeleteText(score2);
        score2 = nullptr;
    }
    if (score1Label)
    {
        gltDeleteText(score1Label);
        score1Label = nullptr;
    }
    if (score2Label)
    {
        gltDeleteText(score2Label);
        score2Label = nullptr;
    }

    // Shutdown GLText last
    gltTerminate();

    std::cout << "Resources cleaned up." << std::endl;

    // Delete shaders
    delete m_debugShader;
    delete m_player1Shader;
    delete m_player2Shader;
    delete m_floorShader;
    delete m_hitboxShader;

    m_debugShader = nullptr;
    m_player1Shader = nullptr;
    m_player2Shader = nullptr;
    m_floorShader = nullptr;
    m_hitboxShader = nullptr;
}

void GlRender::renderButton(float x, float y, float width, float height, const char *text,
                            bool hovered, bool pressed, glm::vec3 color)
{
    // Save current depth test state
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLint currentDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFunc);

    // Disable depth testing for 2D rendering
    glDisable(GL_DEPTH_TEST);

    static Shader *buttonShader = nullptr;
    if (!buttonShader)
    {
        try
        {
            buttonShader = new Shader("button");
            if (!buttonShader->m_shaderProgram)
            {
                std::cerr << "Failed to create button shader program" << std::endl;
                return;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error creating button shader: " << e.what() << std::endl;
            return;
        }
    }

    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    // Add a small offset when pressed
    if (pressed)
    {
        ndcY -= 0.01f; // Slight downward shift when pressed
    }

    float vertices[] = {
        ndcX, ndcY, 0.0f,                        // top left
        ndcX + ndcWidth, ndcY, 0.0f,             // top right
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f, // bottom right
        ndcX, ndcY + ndcHeight, 0.0f             // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

    // Create and bind VAO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    buttonShader->use();

    // Set the button position and size uniforms
    GLint posLocation = glGetUniformLocation(buttonShader->m_shaderProgram, "buttonPos");
    GLint sizeLocation = glGetUniformLocation(buttonShader->m_shaderProgram, "buttonSize");

    if (posLocation != -1)
    {
        glUniform2f(posLocation, x, M_WINDOW_HEIGHT_PX - y - height); // Convert to OpenGL coordinates
    }
    if (sizeLocation != -1)
    {
        glUniform2f(sizeLocation, width, height);
    }

    GLint colorLoc = glGetUniformLocation(buttonShader->m_shaderProgram, "buttonColor");
    if (colorLoc != -1)
    {
        if (pressed)
        {
            glUniform3f(colorLoc, color.x * 0.7f, color.y * 0.7f, color.z * 0.7f);
        }
        else if (hovered)
        {
            glUniform3f(colorLoc,
                        std::min(color.x + 0.2f, 1.0f),
                        std::min(color.y + 0.2f, 1.0f),
                        std::min(color.z + 0.2f, 1.0f));
        }
        else
        {
            glUniform3f(colorLoc, color.x, color.y, color.z);
        }
    }

    // Enable blending for transparent edges
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw button background
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw text with slight offset when pressed
    GLTtext *buttonText = gltCreateText();
    if (buttonText)
    {
        gltSetText(buttonText, text);
        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Calculate text dimensions (approximate)
        float textWidth = strlen(text) * 20; // Increased from 15 to 20 for better spacing
        float textHeight = 30;               // Approximate height of text

        // Center both horizontally and vertically
        float textX = x + (width - textWidth) / 2;
        float textY = y + (height - textHeight) / 2;

        if (pressed)
        {
            textY += 2; // Move text down slightly when pressed
        }

        // Different scale for X button vs other buttons
        float scale = (strcmp(text, "X") == 0) ? 2.0f : 2.5f;
        gltDrawText2D(buttonText, textX, textY, scale);

        gltEndDraw();
        gltDeleteText(buttonText);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Restore previous OpenGL state
    if (depthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(currentDepthFunc);
    }
}

void GlRender::renderDialogBox(float x, float y, float width, float height, float alpha)
{
    static Shader *dialogShader = nullptr;
    if (!dialogShader)
    {
        try
        {
            dialogShader = new Shader("button"); // Reuse button shader
            if (!dialogShader->m_shaderProgram)
            {
                std::cerr << "Failed to create dialog shader program" << std::endl;
                return;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error creating dialog shader: " << e.what() << std::endl;
            return;
        }
    }

    // Convert to NDC
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    float vertices[] = {
        ndcX, ndcY, 0.0f,
        ndcX + ndcWidth, ndcY, 0.0f,
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f,
        ndcX, ndcY + ndcHeight, 0.0f};

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Save current blend state
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dialogShader->use();
    GLint colorLoc = glGetUniformLocation(dialogShader->m_shaderProgram, "buttonColor");
    if (colorLoc != -1)
    {
        glUniform3f(colorLoc, 0.1f, 0.1f, 0.1f); // Dark gray background
        GLint alphaLoc = glGetUniformLocation(dialogShader->m_shaderProgram, "alpha");
        if (alphaLoc != -1)
        {
            glUniform1f(alphaLoc, alpha);
        }
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Restore previous blend state
    if (!blendEnabled)
    {
        glDisable(GL_BLEND);
    }
    glBlendFunc(blendSrc, blendDst);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GlRender::renderTexturedQuadScaled(GLuint texture, float x, float y, float width, float height, float brightness)
{
    // Convert to normalized device coordinates
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    // Save current OpenGL state
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLint currentDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFunc);

    // Disable depth testing temporarily
    glDisable(GL_DEPTH_TEST);

    float vertices[] = {
        // Positions                       // Texture Coords
        ndcX, ndcY, 0.0f, 0.0f, 1.0f,                        // Top-left
        ndcX + ndcWidth, ndcY, 0.0f, 1.0f, 1.0f,             // Top-right
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f, 1.0f, 0.0f, // Bottom-right
        ndcX, ndcY + ndcHeight, 0.0f, 0.0f, 0.0f             // Bottom-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Use shader for textured quad
    static Shader *quadShader = nullptr;
    if (!quadShader)
    {
        quadShader = new Shader("menu");
    }

    quadShader->use();
    quadShader->setInt("texture1", 0);
    quadShader->setFloat("brightness", brightness);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Draw the quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Restore OpenGL state
    if (depthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    glDepthFunc(currentDepthFunc);
}

void GlRender::renderDebugBoxes(Entity entity, const Box &box, const glm::vec3 &color)
{
    if (!m_debugShader)
    {
        std::cerr << "Debug shader not initialized!" << std::endl;
        return;
    }

    Motion &motion = registry.motions.get(entity);
    PlayableArea &playableArea = registry.playableArea.get(m_playableArea);

    // Calculate box vertices in world space
    float left = box.getLeft(motion.position, motion.direction) - playableArea.position.x;
    float right = box.getRight(motion.position, motion.direction) - playableArea.position.x;
    float top = box.getTop(motion.position, motion.direction);
    float bottom = box.getBottom(motion.position, motion.direction);

    // Create vertices for the box outline
    float vertices[] = {
        left, top, 0.0f,     // Top-left
        right, top, 0.0f,    // Top-right
        right, bottom, 0.0f, // Bottom-right
        left, bottom, 0.0f   // Bottom-left
    };

    // Create and bind VAO/VBO with RAII-style cleanup
    GLuint VAO = 0, VBO = 0;
    try
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Use debug shader
        m_debugShader->use();
        m_debugShader->setVec3("color", color);

        // Save current line width
        GLfloat oldLineWidth;
        glGetFloatv(GL_LINE_WIDTH, &oldLineWidth);

        // Set line width for debug boxes
        glLineWidth(1.0f);

        // Draw box outline
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Restore previous line width
        glLineWidth(oldLineWidth);

        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    catch (...)
    {
        // Ensure cleanup even if an error occurs
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
        throw;
    }
}

void GlRender::renderPlayableArea(Entity entity, const vec2 &position, float width, float height, const glm::vec3 &color)
{
    if (!m_debugShader)
    {
        std::cerr << "Debug shader not initialized!" << std::endl;
        return;
    }

    PlayableArea &playableArea = registry.playableArea.get(m_playableArea);

    // Calculate box vertices in world space
    float left = position.x - width / M_WINDOW_WIDTH_PX - playableArea.position.x;
    float right = position.x + width / M_WINDOW_WIDTH_PX - playableArea.position.x;
    float top = position.y + height / M_WINDOW_HEIGHT_PX;
    float bottom = position.y - height / M_WINDOW_HEIGHT_PX;

    // Create vertices for the box outline
    float vertices[] = {
        left, top, 0.0f,     // Top-left
        right, top, 0.0f,    // Top-right
        right, bottom, 0.0f, // Bottom-right
        left, bottom, 0.0f   // Bottom-left
    };

    // Create and bind VAO/VBO with RAII-style cleanup
    GLuint VAO = 0, VBO = 0;
    try
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Use debug shader
        m_debugShader->use();
        m_debugShader->setVec3("color", color);

        // Save current line width
        GLfloat oldLineWidth;
        glGetFloatv(GL_LINE_WIDTH, &oldLineWidth);

        // Set line width for debug boxes
        glLineWidth(1.0f);

        // Draw box outline
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        // Restore previous line width
        glLineWidth(oldLineWidth);

        // Cleanup
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    catch (...)
    {
        // Ensure cleanup even if an error occurs
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
        throw;
    }
}

template <class T>
void GlRender::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    gl_has_errors();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
    gl_has_errors();
}

void GlRender::initializeGlMeshes()
{
    for (uint i = 0; i < mesh_paths.size(); i++)
    {
        // Initialize meshes
        GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
        std::string name = mesh_paths[i].second;
        ObjectMesh::loadFromOBJFile(name,
                                    meshes[(int)geom_index].vertices,
                                    meshes[(int)geom_index].vertex_indices,
                                    meshes[(int)geom_index].original_size);
    }
}
