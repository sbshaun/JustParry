#include "renderer.hpp"
#include "common.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GlRender::GlRender() {}

GlRender::~GlRender() {}

void GlRender::initialize()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    loadTextures();
    gltInit();
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
}

void GlRender::initializeUI()
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
            modelMatrix = glm::mat4(1.0f);
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
            modelMatrix = glm::mat4(1.0f);
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
}

void GlRender::handleHitboxRenders()
{
    for (Entity &hitbox_entity : registry.debugRenders.entities)
    {
        HitboxRender &hitboxRender = registry.debugRenders.get(hitbox_entity);
        Shader *shader = hitboxRender.shader;
        Mesh &mesh = hitboxRender.mesh;

        Entity &player = hitboxRender.player;
        HitBox &player_hitBox = registry.hitBoxes.get(player);

        if (player_hitBox.active)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            Motion &player_motion = registry.motions.get(player);
            Player &registry_player = registry.players.get(player);
            shader->use();

            GLint uniformLocation = glGetUniformLocation(shader->m_shaderProgram, "playerPosition");
            if (uniformLocation == -1)
            {
                std::cerr << "Warning: Uniform ' playerPosition ' doesn't exist or isn't used in the shader." << std::endl;
                return;
            }

            // Set the uniform value (glUniform3f for vec3)
            glUniform3f(uniformLocation, player_motion.position.x, player_motion.position.y, 0.0f);
            mesh.draw();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
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
    loadTexture(textures_path("menu 1.png"), m_backgroundTexture);
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
    // currently hard coded for two initial entities,
    const auto &playerhealths = registry.healths;

    const int p1Health = registry.healths.get(m_player1).currentHealth;
    const int p2Health = registry.healths.get(m_player2).currentHealth;

    if (m_timerText && m_leftText && m_rightText)
    {
        std::stringstream ss;
        ss << timer;
        // update timer,
        gltSetText(time, ss.str().c_str());

        // player 1 health update
        std::string strH1 = std::to_string(p1Health);
        gltSetText(h1, strH1.c_str());

        // player 2 health update
        std::string strH2 = std::to_string(p2Health);
        gltSetText(h2, strH2.c_str());

        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f); // White text color

        // Draw Player 1 health text
        gltDrawText2D(m_leftText, 100, 65, 1.5f);
        gltDrawText2D(h1, 140, 100, 2.f);

        // Draw timer text
        gltDrawText2D(m_timerText, 480, 70, 1.5f);
        gltDrawText2D(time, 490, 100, 2.5f);

        // Draw Player 2 health text
        gltDrawText2D(m_rightText, 800, 70, 1.5f);
        gltDrawText2D(h2, 840, 100, 2.f);

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
    if (m_timerText)
    {
        gltDeleteText(m_timerText);
    }
    if (m_leftText)
    {
        gltDeleteText(m_leftText);
    }
    if (m_rightText)
    {
        gltDeleteText(m_rightText);
    }
    if (h1)
    {
        gltDeleteText(h1);
    }
    if (h2)
    {
        gltDeleteText(h2);
    }
    if (time)
    {
        gltDeleteText(time);
    }
    if (m_roundOver)
    {
        gltDeleteText(m_roundOver);
    }
    gltTerminate();

    std::cout << "Resources cleaned up." << std::endl;
}
