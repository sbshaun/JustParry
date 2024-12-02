#include "renderer.hpp"
#include "../common.hpp"
#include "../state/game.hpp"
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

        m_fontShader = new Shader("font");
        std::cout << "Font shader loaded" << std::endl;

        m_buttonShader = new Shader("button");
        std::cout << "Button shader loaded" << std::endl;

        m_simpleButtonShader = new Shader("simple_button");
        std::cout << "Simple button shader loaded" << std::endl;

        m_redRectangleShader = new Shader("red_rectangle");
        std::cout << "Red rectangle shader loaded" << std::endl;

        std::cout << "All shaders initialized successfully\n"
                  << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing shaders: " << e.what() << std::endl;
    }

    // Reset animation values
    m_animationStarted = false;
    m_delayTimer = 0.0f;
    m_roundOverY = -600.0f;
    m_animationProgress = 0.0f;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

void GlRender::renderRoundOver(int count)
{
    // Calculate time since last frame
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - m_lastUpdateTime).count();
    m_lastUpdateTime = currentTime;
    Health &h1 = registry.healths.get(m_player1);
    Health &h2 = registry.healths.get(m_player2);

    // Check if enter is pressed to start exit animation
    if (isKeyPressed(GLFW_KEY_ENTER) && !m_exitAnimationStarted)
    {
        m_exitAnimationStarted = true;
        m_animationProgress = 0.0f; // Reset progress for exit animation
    }

    // Handle exit animation
    if (m_exitAnimationStarted)
    {
        m_animationProgress += deltaTime * 1.0f;
        m_animationProgress = std::min(m_animationProgress, 1.0f);

        // Ease-in cubic function for upward movement
        float easeIn = m_animationProgress * m_animationProgress * m_animationProgress;
        m_roundOverY = m_targetY + (m_exitY - m_targetY) * easeIn;

        // Render at current position
        if (h1.currentHealth < h2.currentHealth)
        {
            renderTexturedQuadScaled(
                m_roundOverP2Texture,
                0, m_roundOverY,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f);
        }
        else if (h1.currentHealth > h2.currentHealth)
        {
            renderTexturedQuadScaled(
                m_roundOverP1Texture,
                0, m_roundOverY,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f);
        }
        else
        {
            renderTexturedQuadScaled(
                m_roundOverDrawTexture,
                0, m_roundOverY,
                M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
                1.0f);
        }

        // Only mark animation as complete when it has fully finished
        if (m_animationProgress >= 1.0f && m_roundOverY <= m_exitY)
        {
            m_animationComplete = true;
        }

        return;
    }

    // Handle entrance animation
    if (!m_animationStarted)
    {
        m_delayTimer += deltaTime;
        if (m_delayTimer >= ANIMATION_DELAY)
        {
            m_animationStarted = true;
            m_roundOverY = -600.0f;
            m_animationProgress = 0.0f;
        }
        return;
    }

    // Update animation if not complete
    if (m_animationProgress < 1.0f)
    {
        // Progress animation based on time
        m_animationProgress += deltaTime * 0.8f;
        m_animationProgress = std::min(m_animationProgress, 1.0f);

        // Apply cubic ease-out function for smooth deceleration
        float easeOut = 1.0f - powf(1.0f - m_animationProgress, 3.0f);

        // Calculate current position using easing
        m_roundOverY = -600.0f + (m_targetY + 600.0f) * easeOut;
    }

    if (h1.currentHealth < h2.currentHealth)
    {
        renderTexturedQuadScaled(
            m_roundOverP2Texture,
            0, m_roundOverY,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }
    else if (h1.currentHealth > h2.currentHealth)
    {
        renderTexturedQuadScaled(
            m_roundOverP1Texture,
            0, m_roundOverY,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }
    else
    {
        renderTexturedQuadScaled(
            m_roundOverDrawTexture,
            0, m_roundOverY,
            M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX,
            1.0f);
    }

    // Only show text once the image is mostly in position
    float textRevealThreshold = m_targetY - 50.0f;
    if (m_roundOverY >= textRevealThreshold)
    {

        // Check if it's a draw (both players have same health)
        Health &h1 = registry.healths.get(m_player1);
        Health &h2 = registry.healths.get(m_player2);
        std::string overText = "";
        if (h1.currentHealth == h2.currentHealth)
        {
            overText = "DRAW!";
        }
        else
        {
            overText = "ROUND " + std::to_string(game->getCurrentRound() - 1);
        }

        // Create and render winner text
        std::string wonText = "";
        if (h1.currentHealth > h2.currentHealth)
        {
            wonText = "Player 1 Wins!";
        }
        else if (h1.currentHealth < h2.currentHealth)
        {
            wonText = "Player 2 Wins!";
        }

        // Get window size and calculate scaling factors
        int windowWidth, windowHeight;
        glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

        int framew_width, frame_height;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &framew_width, &frame_height);
        float xscale = (float)framew_width / windowWidth;
        float yscale = (float)frame_height / windowHeight;

        // Calculate text positions
        float baseX = windowWidth * 0.49f;
        float baseY = windowHeight * 0.55f;

        // Render game over and winner text
        if (count == 1)
        {
            if (h1.currentHealth < h2.currentHealth)
            {
                renderText(overText, (baseX + 210.f) * xscale, (baseY - 325.f) * yscale, (0.7f * xscale), glm::vec3(0.3f, 0.0f, 0.0f));
                renderText("PRESS ENTER TO CONTINUE!", (baseX - 460.f) * xscale, (baseY + 275.0f) * yscale, 0.4f * xscale, glm::vec3(0.0f, 0.0f, 0.0f));
                renderText(wonText, (baseX - 450.f) * xscale, (baseY - 50.f) * yscale, (0.6f * xscale), glm::vec3(0.0f, 0.0f, 0.0f));
            }
            else if (h1.currentHealth > h2.currentHealth)
            {
                renderText(overText, (baseX - 470.f) * xscale, (baseY - 350.f) * yscale, (0.75f * xscale), glm::vec3(0.3f, 0.0f, 0.0f));
                renderText("PRESS ENTER TO CONTINUE!", (baseX - 25.f) * xscale, (baseY + 275.0f) * yscale, 0.4f * xscale, glm::vec3(0.0f, 0.0f, 0.0f));
                renderText(wonText, (baseX + 50.f) * xscale, (baseY - 75.f) * yscale, (0.6f * xscale), glm::vec3(0.0f, 0.0f, 0.0f));
            }
            else
            {
                renderText(overText, (baseX - 90.f) * xscale, (baseY - 330.f) * yscale, (0.95f * xscale), glm::vec3(0.3f, 0.0f, 0.0f));
                renderText("PRESS ENTER TO CONTINUE!", (baseX - 275.f) * xscale, (baseY - 275.0f) * yscale, 0.5f * xscale, glm::vec3(0.4f, 0.4f, 0.4f));
            }
        }
    }
}

// Reset animation state
void GlRender::resetRoundOverAnimation()
{
    m_animationStarted = false;
    m_exitAnimationStarted = false;
    m_delayTimer = 0.0f;
    m_roundOverY = -600.0f;
    m_animationProgress = 0.0f;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

void GlRender::renderFPS(int fps, bool showFPS)
{
    if (showFPS)
    {
        std::string strH1 = std::to_string(fps);
        renderText(strH1, M_WINDOW_WIDTH_PX - 45.0f, 30.f, 0.3f, glm::vec3(0.0f, 1.f, 0.0f));
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
                modelMatrix = glm::scale(modelMatrix, glm::vec3(motion.scale.x, motion.scale.y, 1.0f));
            }
            else
            {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(motion.scale.x, motion.scale.y, 1.0f));
            }

            PlayerCurrentState &player1State = registry.playerCurrentStates.get(entity);
            Renderable &player1Renders = registry.renderable.get(entity);

            Animation &animation = registry.animations.get(entity);
            player1Renders.texture = animation.currentTexture;

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

            if (registry.playerCurrentStates.get(m_player1).currentState == PlayerState::BLOCKSTUNNED)
            {
                shader->setBool("isBlockStunned", true);
            }
            else
            {
                shader->setBool("isBlockStunned", false);
            }

            if (registry.hitBoxes.get(m_player2).hit)
            {
                shader->setBool("takenDamage", true);
            }
            else
            {
                shader->setBool("takenDamage", false);
            }

            shader->setInt("color", player.color);
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
                modelMatrix = glm::scale(modelMatrix, glm::vec3(-motion.scale.x, motion.scale.y, 1.0f));
            }

            if (registry.motions.get(entity).angle != 0)
            {
                modelMatrix = glm::rotate(modelMatrix, motion.angle, glm::vec3(motion.position.x, motion.position.y, 0.f));
            }
            PlayerCurrentState &player2State = registry.playerCurrentStates.get(entity);
            Renderable &player2Renders = registry.renderable.get(entity);
            Animation &animation = registry.animations.get(entity);
            player2Renders.texture = animation.currentTexture;

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

            if (registry.playerCurrentStates.get(m_player2).currentState == PlayerState::BLOCKSTUNNED)
            {
                shader->setBool("isBlockStunned", true);
            }
            else
            {
                shader->setBool("isBlockStunned", false);
            }

            if (registry.hitBoxes.get(m_player1).hit)
            {
                shader->setBool("takenDamage", true);
            }
            else
            {
                shader->setBool("takenDamage", false);
            }

            shader->setInt("color", player.color);
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

        if (registry.collisionBoxes.has(m_player1))
        {
            CollisionBox &p1CollisionBox = registry.collisionBoxes.get(m_player1);
            // Yellow for collision boxes
            renderDebugBoxes(m_player1, p1CollisionBox, glm::vec3(1.0f, 1.0f, 0.0f));
        }

        if (registry.hitBoxes.has(m_player1))
        {
            HitBox &p1HitBox = registry.hitBoxes.get(m_player1);
            if (p1HitBox.active)
            {
                // Red for hitboxes
                renderDebugBoxes(m_player1, p1HitBox, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        if (registry.hurtBoxes.has(m_player1))
        {
            HurtBox &p1HurtBox = registry.hurtBoxes.get(m_player1);
            // Green for hurtboxes
            renderDebugBoxes(m_player1, p1HurtBox, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (registry.parryBoxes.has(m_player1))
        {

            ParryBox &p1ParryBox = registry.parryBoxes.get(m_player1);
            if (p1ParryBox.active)
            {
                // Purple for parry boxes
                renderDebugBoxes(m_player1, p1ParryBox, glm::vec3(1.0f, 0.0f, 1.0f));
            }
        }

        if (registry.collisionBoxes.has(m_player2))
        {
            CollisionBox &p1CollisionBox = registry.collisionBoxes.get(m_player2);
            // Yellow for collision boxes
            renderDebugBoxes(m_player2, p1CollisionBox, glm::vec3(1.0f, 1.0f, 0.0f));
        }

        if (registry.hitBoxes.has(m_player2))
        {
            HitBox &p2HitBox = registry.hitBoxes.get(m_player2);
            if (p2HitBox.active)
            {
                // Red for hitboxes
                renderDebugBoxes(m_player2, p2HitBox, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        if (registry.hurtBoxes.has(m_player2))
        {
            HurtBox &p2HurtBox = registry.hurtBoxes.get(m_player2);
            // Green for hurtboxes
            renderDebugBoxes(m_player2, p2HurtBox, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (registry.parryBoxes.has(m_player2))
        {
            ParryBox &p2ParryBox = registry.parryBoxes.get(m_player2);
            if (p2ParryBox.active)
            {
                // Purple for parry boxes
                renderDebugBoxes(m_player2, p2ParryBox, glm::vec3(1.0f, 0.0f, 1.0f));
            }
        }
    }
}

void GlRender::handleNotifications(float elapsed_ms)
{
    for (Entity &notification_entity : registry.notifications.entities)
    {
        Notification &notification = registry.notifications.get(notification_entity);

        float opacity = 1.0f;
        notification.counter_ms -= elapsed_ms;

        if (notification.counter_ms <= 0.f)
        {
            registry.remove_all_components_of(notification_entity);
            continue;
        }

        if (notification.counter_ms < 150.f)
        {
            opacity = notification.counter_ms / 150.f;
        }

        int x = notification.player1Side ? 0 : M_WINDOW_WIDTH_PX / 2;
        float y = 15.f * opacity;

        renderTexturedQuadScaled(
            notification.texture_id,
            static_cast<float>(x), y,
            M_WINDOW_WIDTH_PX / 2, M_WINDOW_HEIGHT_PX,
            1.0f, opacity);
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

    PlayableArea &playableArea = registry.playableArea.get(m_playableArea);

    renderTexturedQuadScaled(
        m_bg2Texture,
        -M_WINDOW_WIDTH_PX / 2.0f - playableArea.position.x * M_WINDOW_WIDTH_PX / 1.5f, 0.0f,
        M_WINDOW_WIDTH_PX * 2.0f, M_WINDOW_HEIGHT_PX,
        1.0f);

    renderTexturedQuadScaled(
        m_bg3Texture,
        -M_WINDOW_WIDTH_PX / 2.0f - playableArea.position.x * M_WINDOW_WIDTH_PX / 2.0f, 0.0f,
        M_WINDOW_WIDTH_PX * 2.0f, M_WINDOW_HEIGHT_PX,
        1.0f);

    // debugging wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    handleStaticRenders();
    handleTexturedRenders();

    glDepthMask(GL_TRUE);

    // std::cout << "PLAYER 1" << std::endl;
    //
    // std::cout << registry.players.get(m_player1).perfectParries << std::endl;
    // std::cout << registry.players.get(m_player1).parries << std::endl;

    // std::cout << "PLAYER 2" << std::endl;

    // std::cout << registry.players.get(m_player2).perfectParries << std::endl;
    // std::cout << registry.players.get(m_player2).parries << std::endl;
}
void GlRender::loadTextures()
{
    // Load texture for player 1
    loadTexture(textures_path("menu_1.png"), m_menuTexture);
    loadTexture(textures_path("arcade_menu.png"), m_arcadeMenuTexture);
    loadTexture(textures_path("t1.png"), m_helpTexture1);
    loadTexture(textures_path("t2.png"), m_helpTexture2);
    loadTexture(textures_path("t3.png"), m_helpTexture3);
    loadTexture(textures_path("settings-screen.png"), m_settingsTexture);
    loadTexture(textures_path("paused.png"), m_pauseMenuTexture);
    loadTexture(textures_path("bg1.png"), m_bg1Texture);
    loadTexture(textures_path("bg2.png"), m_bg2Texture);
    loadTexture(textures_path("bg3.png"), m_bg3Texture);
    loadTexture(textures_path("bg4.png"), m_bg4Texture);
    loadTexture(textures_path("3.png"), m_countdown_3);
    loadTexture(textures_path("2.png"), m_countdown_2);
    loadTexture(textures_path("1.png"), m_countdown_1);
    loadTexture(textures_path("fight.png"), m_countdown_fight);
    loadTexture(textures_path("notification_parried.png"), m_notif_parried);
    loadTexture(textures_path("notification_hit.png"), m_notif_hit);
    loadTexture(textures_path("notification_stunned.png"), m_notif_stunned);
    loadTexture(textures_path("round_over_p1.png"), m_roundOverP1Texture);
    loadTexture(textures_path("round_over_p2.png"), m_roundOverP2Texture);
    loadTexture(textures_path("round_over_draw.png"), m_roundOverDrawTexture);
    loadTexture(textures_path("timer.png"), m_timerBackgroundTexture);
    loadTexture(textures_path("bar_square_gloss_large.png"), m_barTexture);
    loadTexture(textures_path("avatar.png"), m_avatarTexture);
    loadTexture(textures_path("character_select.png"), m_characterSelectTexture);
    loadTexture(textures_path("character_select_arcade.png"), m_characterSelectTextureArcade);
    // white characters
    loadTexture(textures_path("bird_idle_f1.png"), m_character1);
    loadTexture(textures_path("bird_idle_f1_flipped.png"), m_character1_flip);
    loadTexture(textures_path("bird_parry_f1.png"), m_character1_ready);
    loadTexture(textures_path("bird_parry_f1_flipped.png"), m_character1_flip_ready);
    // red characters
    loadTexture(textures_path("bird_idle_f1_red.png"), m_character1_red);
    loadTexture(textures_path("bird_idle_f1_flipped_red.png"), m_character1_flip_red);
    loadTexture(textures_path("bird_parry_f1_red.png"), m_character1_ready_red);
    loadTexture(textures_path("bird_parry_f1_flipped_red.png"), m_character1_flip_ready_red);
    // green characters
    loadTexture(textures_path("bird_idle_f1_green.png"), m_character1_green);
    loadTexture(textures_path("bird_idle_f1_flipped_green.png"), m_character1_flip_green);
    loadTexture(textures_path("bird_parry_f1_green.png"), m_character1_ready_green);
    loadTexture(textures_path("bird_parry_f1_flipped_green.png"), m_character1_flip_ready_green);
    // blue characters
    loadTexture(textures_path("bird_idle_f1_blue.png"), m_character1_blue);
    loadTexture(textures_path("bird_idle_f1_flipped_blue.png"), m_character1_flip_blue);
    loadTexture(textures_path("bird_parry_f1_blue.png"), m_character1_ready_blue);
    loadTexture(textures_path("bird_parry_f1_flipped_blue.png"), m_character1_flip_ready_blue);

    loadTexture(textures_path("key_R.png"), m_p1SelectKey);
    loadTexture(textures_path("key_X.png"), m_p2SelectKey);
    loadTexture(textures_path("Bird_Story_1_1.png"), bird_Story_1_1);
    loadTexture(textures_path("Bird_Story_1_2.png"), bird_Story_1_2);
    loadTexture(textures_path("Bird_Story_1_3.png"), bird_Story_1_3);
    loadTexture(textures_path("Bird_Story_1_4.png"), bird_Story_1_4);
    loadTexture(textures_path("Bird_Story_1_5.png"), bird_Story_1_5);
    loadTexture(textures_path("Bird_Story_1_6.png"), bird_Story_1_6);
    loadTexture(textures_path("Bird_Story_1_7.png"), bird_Story_1_7);
    loadTexture(textures_path("Bird_Story_2_1.png"), bird_Story_2_1);
    loadTexture(textures_path("Bird_Story_2_2.png"), bird_Story_2_2);
    loadTexture(textures_path("Bird_Story_2_3.png"), bird_Story_2_3);
    loadTexture(textures_path("Bird_Story_2_4.png"), bird_Story_2_4);
    loadTexture(textures_path("Bird_Story_2_5.png"), bird_Story_2_5);
    loadTexture(textures_path("Bird_Story_2_6.png"), bird_Story_2_6);
    loadTexture(textures_path("Bird_Story_3_1.png"), bird_Story_3_1);
    loadTexture(textures_path("Bird_Story_3_2.png"), bird_Story_3_2);
    loadTexture(textures_path("Bird_Story_3_3.png"), bird_Story_3_3);
    loadTexture(textures_path("Bird_Story_3_4.png"), bird_Story_3_4);
    loadTexture(textures_path("Bird_Story_3_5.png"), bird_Story_3_5);
    loadTexture(textures_path("Bird_Story_3_6.png"), bird_Story_3_6);
    loadTexture(textures_path("Bird_Story_3_7.png"), bird_Story_3_7);
    loadTexture(textures_path("Bird_Story_3_8.png"), bird_Story_3_8);
    loadTexture(textures_path("Bird_Story_3_9.png"), bird_Story_3_9);
    loadTexture(textures_path("Bird_Story_3_10.png"), bird_Story_3_10);
    loadTexture(textures_path("Bird_Story_4_1.png"), bird_Story_4_1);
    loadTexture(textures_path("Bird_Story_4_2.png"), bird_Story_4_2);
    loadTexture(textures_path("Bird_Story_4_3.png"), bird_Story_4_3);
    loadTexture(textures_path("Bird_Story_4_4.png"), bird_Story_4_4);
    loadTexture(textures_path("Bird_Story_4_5.png"), bird_Story_4_5);
    loadTexture(textures_path("Bird_Story_4_6.png"), bird_Story_4_6);
    loadTexture(textures_path("Bird_Story_4_7.png"), bird_Story_4_7);
    loadTexture(textures_path("Bird_Story_4_8.png"), bird_Story_4_8);
    loadTexture(textures_path("Bird_Story_4_9.png"), bird_Story_4_9);
    loadTexture(textures_path("Bird_Story_E_1.png"), bird_Story_E_1);
    loadTexture(textures_path("Bird_Story_E_2.png"), bird_Story_E_2);
    loadTexture(textures_path("Bird_Story_E_3.png"), bird_Story_E_3);
    loadTexture(textures_path("Bird_Story_E_4.png"), bird_Story_E_4);
    loadTexture(textures_path("Bird_Story_E_5.png"), bird_Story_E_5);
    loadTexture(textures_path("Bird_Story_E_6.png"), bird_Story_E_6);
    loadTexture(textures_path("Bird_Story_E_7.png"), bird_Story_E_7);
    loadTexture(textures_path("Bird_Story_E_8.png"), bird_Story_E_8);
    loadTexture(textures_path("Bird_Story_E_9.png"), bird_Story_E_9);
    loadTexture(textures_path("level_win.png"), m_levelWonTexture);
    loadTexture(textures_path("level_lost.png"), m_levelLostTexture);
    loadTexture(textures_path("match_over_p1.png"), m_matchOverP1Texture);
    loadTexture(textures_path("match_over_p2.png"), m_matchOverP2Texture);
    loadTexture(textures_path("arcade_story_bg.png"), m_arcadeStoryTexture);
    FighterManager::loadBirdTextures(*this);
}

void GlRender::loadTexture(const std::string &path, GLuint &textureID)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    float p1Health = registry.healths.get(m_player1).currentHealth;
    float p2Health = registry.healths.get(m_player2).currentHealth;

    float p1Posture = registry.postureBars.get(m_player1).currentBar;
    float p2Posture = registry.postureBars.get(m_player2).currentBar;

    // Get the current window size and scaling factors
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    int framew_width, frame_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &framew_width, &frame_height);
    float xscale = (float)framew_width / windowWidth;
    float yscale = (float)frame_height / windowHeight;

    // Calculate positions
    float centerX = (windowWidth) / 2.0f;
    float topY = (windowHeight) * 0.1f;

    float p1X = centerX * 0.4f;
    float healthY = topY;
    float valueY = topY + (35.0f * yscale);
    float p2X = centerX * 1.6f;
    float scoreY = topY + (75.0f * yscale);

    // Timer background dimensions and position
    float timerBgWidth = 115.0f;
    float timerBgHeight = 92.0f;
    float timerBgX = centerX - (timerBgWidth / 2.0f);
    float timerBgY = topY - 10.0f;

    // Bar dimensions and positions
    float barWidth = 275.0f;
    float barHeight = 40.0f;
    float smallBarHeight = 20.0f;  // Height for the smaller bars
    float barPadding = -5.0f;      // Space between timer and bars
    float verticalPadding = 10.0f; // Space between main bar and small bar

    // Left bar position
    float leftBarX = timerBgX - barWidth - barPadding;
    float leftBarY = timerBgY + (timerBgHeight - barHeight) / 2.0f;
    float leftSmallBarY = leftBarY + barHeight + verticalPadding; // Position below main bar

    // Right bar position
    float rightBarX = timerBgX + timerBgWidth + barPadding;
    float rightBarY = leftBarY;
    float rightSmallBarY = rightBarY + barHeight + verticalPadding; // Position below main bar

    // Draw scores P1
    renderText("P1", (p1X - 20.f), (scoreY - 65.f) - 10 * (yscale - 1), 0.25f, glm::vec3(1.0f, 1.0f, 1.0f));
    std::string strScore1 = "WINS: " + std::to_string(game->getPlayer1Score());
    renderText(strScore1.c_str(), (p1X - 105.f), (scoreY + 25.f) - 70 * (yscale - 1), 0.25f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Draw scores P2
    renderText("P2", (p2X - 15.f), (scoreY - 65.f) - 10 * (yscale - 1), 0.25f, glm::vec3(1.0f, 1.0f, 1.0f));
    std::string strScore2 = "WINS: " + std::to_string(game->getPlayer2Score());
    renderText(strScore2.c_str(), (p2X + 30.0f), (scoreY + 25.f) - 70 * (yscale - 1), 0.25f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Avatar dimensions and positions
    float avatarSize = barHeight * 1.8f; // Make avatar 1.8x the height of the health bar
    float avatarPadding = 10.0f * xscale;

    // Left avatar position
    float leftAvatarX = leftBarX - avatarSize - avatarPadding;
    float leftAvatarY = leftBarY - (avatarSize - barHeight) / 2.0f;

    // Right avatar position
    float rightAvatarX = rightBarX + barWidth + avatarPadding;
    float rightAvatarY = leftAvatarY;

    // Disable depth testing for UI elements
    glDisable(GL_DEPTH_TEST);

    // Render left avatar
    renderTexturedQuadScaled(
        m_avatarTexture,
        leftAvatarX, leftAvatarY,
        avatarSize, avatarSize,
        1.0f);

    // Render left bar
    renderTexturedQuadScaled(
        m_barTexture,
        leftBarX, leftBarY,
        barWidth, barHeight,
        1.0f);

    // Render right bar
    renderTexturedQuadScaled(
        m_barTexture,
        rightBarX, rightBarY,
        barWidth, barHeight,
        1.0f);

    // Render right avatar
    renderTexturedQuadScaled(
        m_avatarTexture,
        rightAvatarX, rightAvatarY,
        avatarSize, avatarSize,
        1.0f);

    // Render timer background
    // renderTexturedQuadScaled(
    //    m_timerBackgroundTexture,
    //    timerBgX, timerBgY,
    //    timerBgWidth, timerBgHeight,
    //    1.0f);

    // Render main health bars
    renderTexturedQuadScaled(
        m_barTexture,
        leftBarX, leftBarY,
        barWidth, barHeight,
        1.0f);

    renderTexturedQuadScaled(
        m_barTexture,
        rightBarX, rightBarY,
        barWidth, barHeight,
        1.0f);

    // Render smaller bars below
    renderTexturedQuadScaled(
        m_barTexture,
        leftBarX + 75, leftSmallBarY,
        barWidth - 75, smallBarHeight + 10,
        1.0f);

    renderTexturedQuadScaled(
        m_barTexture,
        rightBarX, rightSmallBarY,
        barWidth - 75, smallBarHeight + 10,
        1.0f);

    // draw health boxes according to player health
    handleP1Health(p1Health);
    handleP2Health(p2Health);

    handleP1Posture(static_cast<int>(p1Posture));
    handleP2Posture(static_cast<int>(p2Posture));

    // render timer
    float textWidth = std::to_string(timer).length() * 30.0f * xscale;
    float timerX = centerX - (textWidth / 2.0f);

    renderText(std::to_string(timer), (timerX - 0.f) + 30 * (xscale - 1), (valueY + 17.5f) - 35 * (xscale - 1), 0.5f, glm::vec3(0.102f, 0.102f, 0.102f));

    if (game->isVersusMode())
    {
        renderText("ROUND " + std::to_string(game->getCurrentRound()), 425.f, 75.0f, 0.5f, glm::vec3(0.3f, 0.0f, 0.0f));
    }
    else
    {
        renderText("LEVEL " + std::to_string(game->getCurrentLevel()), 420.f, 50.0f, 0.5f, glm::vec3(0.1f, 0.1f, 0.1f));
        renderText("ROUND " + std::to_string(game->getCurrentRound()), 450.f, 80.0f, 0.3f, glm::vec3(0.3f, 0.3f, 0.3f));
    }

    // Restore depth testing
    glEnable(GL_DEPTH_TEST);
}

void GlRender::handleP1Health(float p1Health)
{ // render health values
    float wx = 23.75f;
    float off = 2.35f;
    if (p1Health == 100)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 90)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 80)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 70)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 60)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 50)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 40)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 30)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 20)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(195.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
    }
    else if (p1Health == 10)
    {
        renderRedHealthRectangle(192.5f, 97.5f, wx, 30.f);
    }
    else if (p1Health == 0)
    {
        return;
    }
}
void GlRender::handleP2Health(float p2Health)
{ // render health values
    float wx = 23.75f;
    float off = 2.35f;
    if (p2Health == 100)
    {
        renderRedHealthRectangle(572.5f, 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 90)
    {
        renderRedHealthRectangle(575.f + (0 * off) + (1 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 80)
    {
        renderRedHealthRectangle(575.f + (1 * off) + (2 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 70)
    {
        renderRedHealthRectangle(575.f + (2 * off) + (3 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 60)
    {
        renderRedHealthRectangle(575.f + (3 * off) + (4 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 50)
    {
        renderRedHealthRectangle(575.f + (4 * off) + (5 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 40)
    {
        renderRedHealthRectangle(575.f + (5 * off) + (6 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 30)
    {
        renderRedHealthRectangle(575.f + (6 * off) + (7 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 20)
    {
        renderRedHealthRectangle(575.f + (7 * off) + (8 * wx), 97.5f, wx, 30.f);
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 10)
    {
        renderRedHealthRectangle(575.f + (8 * off) + (9 * wx), 97.5f, wx, 30.f);
    }
    else if (p2Health == 0)
    {
        return;
    }
}

void GlRender::handleP1Posture(int p1Posture)
{ // render health values
    float wx = 23.75f;
    float off = 2.35f;
    if (p1Posture == 7)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (2 * off) + (2 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (3 * off) + (3 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (4 * off) + (4 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (5 * off) + (5 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (6 * off) + (6 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 6)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (2 * off) + (2 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (3 * off) + (3 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (4 * off) + (4 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (5 * off) + (5 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 5)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (2 * off) + (2 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (3 * off) + (3 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (4 * off) + (4 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 4)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (2 * off) + (2 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (3 * off) + (3 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 3)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (2 * off) + (2 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 2)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
        renderBlueHealthRectangle(270.f + (1 * off) + (1 * wx), 150.f, wx, 15.f);
    }
    else if (p1Posture == 1)
    {
        renderBlueHealthRectangle(270.f, 150.f, wx, 15.f);
    }
    else if (p1Posture == 0)
    {
        return;
    }
}

void GlRender::handleP2Posture(int p2Posture)
{ // render health values
    float wx = 23.75f;
    float off = 2.35f;

    if (p2Posture == 7)
    {
        renderBlueHealthRectangle(500.f + (2 * off) + (3 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (3 * off) + (4 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (4 * off) + (5 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (5 * off) + (6 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (6 * off) + (7 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 6)
    {
        renderBlueHealthRectangle(500.f + (3 * off) + (4 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (4 * off) + (5 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (5 * off) + (6 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (6 * off) + (7 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 5)
    {
        renderBlueHealthRectangle(500.f + (4 * off) + (5 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (5 * off) + (6 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (6 * off) + (7 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 4)
    {
        renderBlueHealthRectangle(500.f + (5 * off) + (6 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (6 * off) + (7 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 3)
    {
        renderBlueHealthRectangle(500.f + (6 * off) + (7 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 2)
    {
        renderBlueHealthRectangle(500.f + (7 * off) + (8 * wx), 150.f, wx, 15.f);
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 1)
    {
        renderBlueHealthRectangle(500.f + (8 * off) + (9 * wx), 150.f, wx, 15.f);
    }
    else if (p2Posture == 0)
    {
        return;
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
    // Delete shaders associated with entities in the registry
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
    GLuint textures[] = {
        m_menuTexture, m_arcadeMenuTexture, m_helpTexture1, m_helpTexture2, m_helpTexture3, m_settingsTexture, m_pauseMenuTexture,
        m_bg1Texture, m_bg2Texture, m_bg3Texture, m_bg4Texture,
        m_roundOverP1Texture, m_roundOverP2Texture, m_roundOverDrawTexture, m_timerBackgroundTexture, m_barTexture,
        m_avatarTexture, m_characterSelectTexture, m_characterSelectTextureArcade, m_character1,
        m_character1_flip, m_character1_ready, m_character1_flip_ready, m_p1SelectKey, m_p2SelectKey,
        m_levelWonTexture, m_levelLostTexture, m_matchOverP1Texture, m_matchOverP2Texture, m_arcadeStoryTexture,
        bird_Story_1_1, bird_Story_1_2, bird_Story_1_3, bird_Story_1_4};

    glDeleteTextures(sizeof(textures) / sizeof(GLuint), textures);

    // Clear texture IDs after deletion to prevent dangling references
    m_menuTexture = m_helpTexture1 = m_helpTexture2 = m_helpTexture3 = m_settingsTexture = m_pauseMenuTexture = 0;
    m_arcadeStoryTexture = 0;
    m_bg1Texture = m_bg2Texture = m_bg3Texture = m_bg4Texture = 0;
    m_countdown_1 = m_countdown_2 = m_countdown_3 = m_countdown_fight = 0;
    m_notif_parried = m_notif_hit = m_notif_stunned = 0;
    m_roundOverP1Texture = m_roundOverP2Texture = m_roundOverDrawTexture = m_timerBackgroundTexture = m_barTexture = 0;
    m_avatarTexture = m_characterSelectTexture = m_characterSelectTextureArcade = m_character1 = 0;
    m_character1_ready = m_character1_flip_ready = 0;
    m_character1_flip = m_p1SelectKey = m_p2SelectKey = 0;
    m_levelWonTexture = m_levelLostTexture = m_matchOverP1Texture = m_matchOverP2Texture = 0;
    bird_Story_1_1 = bird_Story_1_2 = bird_Story_1_3 = bird_Story_1_4 = 0;

    // Free font resources
    if (m_font_VAO)
    {
        glDeleteVertexArrays(1, &m_font_VAO);
        m_font_VAO = 0;
    }

    if (m_font_VBO)
    {
        glDeleteBuffers(1, &m_font_VBO);
        m_font_VBO = 0;
    }

    m_ftCharacters.clear(); // Clear character map for font

    // Delete individual shaders used in the render
    delete m_debugShader;
    delete m_player1Shader;
    delete m_player2Shader;
    delete m_floorShader;
    delete m_hitboxShader;
    delete m_fontShader;
    delete m_buttonShader;
    delete m_simpleButtonShader;
    delete m_redRectangleShader;

    // Nullify shader pointers to avoid dangling references
    m_debugShader = m_player1Shader = m_player2Shader = nullptr;
    m_floorShader = m_hitboxShader = m_fontShader = nullptr;
    m_buttonShader = m_simpleButtonShader = m_redRectangleShader = nullptr;

    std::cout << "Render resources cleaned up successfully." << std::endl;
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

    if (!m_buttonShader->m_shaderProgram)
    {
        std::cerr << "Failed to load button shader program" << std::endl;
        return;
    }
    // Get the current window size
    int windowWidth, windowHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &windowWidth, &windowHeight);

    int framew_width, frame_height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &framew_width, &frame_height);
    float xscale = (float)framew_width / windowWidth;
    float yscale = (float)frame_height / windowHeight;

    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float ndcX = (2.0f * x * xscale / framew_width) - 1.0f;
    float ndcY = 1.0f - (2.0f * y * yscale / frame_height);
    float ndcWidth = 2.0f * width * xscale / framew_width;
    float ndcHeight = -2.0f * height * yscale / frame_height;

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

    m_buttonShader->use();

    // Set the button position and size uniforms
    GLint posLocation = glGetUniformLocation(m_buttonShader->m_shaderProgram, "buttonPos");
    GLint sizeLocation = glGetUniformLocation(m_buttonShader->m_shaderProgram, "buttonSize");

    if (posLocation != -1)
    {
        glUniform2f(posLocation, x * xscale, (M_WINDOW_HEIGHT_PX - y - height) * yscale); // Convert to OpenGL coordinates
    }
    if (sizeLocation != -1)
    {
        glUniform2f(sizeLocation, width * xscale, height * yscale);
    }

    GLint colorLoc = glGetUniformLocation(m_buttonShader->m_shaderProgram, "buttonColor");
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

    // Calculate text dimensions (approximate)
    float textWidth = static_cast<float>(strlen(text)) * 20.0f;
    float textHeight = 30.0f;
    float textX = x + (width - textWidth) / 2.0f - 10 * (xscale - 1);
    float textY = y + (height - textHeight) / 2.0f + 15 * (yscale - 1);

    if (pressed)
    {
        textY += 2; // Move text down slightly when pressed
    }

    // Different scale for X button vs other buttons
    float scale;
    float finalX;
    float finalY;
    glm::vec3 finalColor;
    if (strcmp(text, "X") == 0)
    {
        scale = 0.25f;
        finalColor = glm::vec3(1.0f, 1.0f, 1.0f);
        finalX = textX * xscale + 1.f;
        finalY = textY * yscale + 23.5f;
    }
    else if (strcmp(text, "=") == 0)
    {
        scale = 0.6f;
        finalColor = glm::vec3(0.0f, 0.0f, 0.0f);
        finalX = textX * xscale - 8.f;
        finalY = textY * yscale + 35.f;
    }
    else if (strcmp(text, "MAIN MENU") == 0)
    {
        scale = 0.4f;
        finalColor = glm::vec3(0.0f, 0.0f, 0.0f);
        finalX = textX * xscale - 15.f;
        finalY = textY * yscale + 27.5f;
    }
    else if (strcmp(text, "RESUME") == 0)
    {
        scale = 0.4f;
        finalColor = glm::vec3(0.0f, 0.0f, 0.0f);
        finalX = textX * xscale - 15.f;
        finalY = textY * yscale + 27.5f;
    }
    else if ((x == M_WINDOW_WIDTH_PX / 2.0f + 50.0f) && (y == M_WINDOW_HEIGHT_PX / 2.0f - 65.0f))
    {
        scale = 0.2f;
        finalColor = glm::vec3(0.0f, 0.0f, 0.0f);
        finalX = textX * xscale - 7.f;
        finalY = textY * yscale + 27.5f;
    }
    else
    {
        scale = 0.4f;
        finalColor = glm::vec3(0.0f, 0.0f, 0.0f);
        finalX = textX * xscale - 7.f;
        finalY = textY * yscale + 27.5f;
    }

    float finalScale = scale;
    // std::cout << "finalX: " << finalX << " finalY: " << finalY << " finalScale: " << finalScale << std::endl;
    renderText(text, finalX / xscale, finalY / yscale, finalScale, finalColor);

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

void GlRender::renderTexturedQuadScaled(GLuint texture, float x, float y, float width, float height,
                                        float brightness, float alpha)
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    static Shader *quadShader = nullptr;
    if (!quadShader)
    {
        quadShader = new Shader("menu");
    }

    quadShader->use();
    quadShader->setInt("texture1", 0);
    quadShader->setFloat("brightness", brightness);
    quadShader->setFloat("alpha", alpha);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

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

bool GlRender::fontInit(const std::string &font_filename, unsigned int font_default_size)
{
    if (!m_fontShader)
    {
        std::cerr << "Failed to create font shader" << std::endl;
        return false;
    }

    // Set up VAO/VBO for font rendering
    glGenVertexArrays(1, &m_font_VAO);
    glGenBuffers(1, &m_font_VBO);
    glBindVertexArray(m_font_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);

    gl_has_errors();
    // Reserve space for 6 vertices with 4 components each (pos + tex coords)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    gl_has_errors();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    gl_has_errors();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    gl_has_errors();

    // Initialize FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    gl_has_errors();
    // Load font file
    std::string font_filepath = PROJECT_SOURCE_DIR + std::string("/assets/fonts/") + font_filename;
    FT_Face face;
    if (FT_New_Face(ft, font_filepath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font: " << font_filepath << std::endl;
        FT_Done_FreeType(ft);
        return false;
    }
    gl_has_errors();
    // Set font size
    FT_Set_Pixel_Sizes(face, 0, font_default_size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load ASCII characters
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character data
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x),
            static_cast<char>(c)};
        m_ftCharacters.insert(std::pair<char, Character>(c, character));
    }
    gl_has_errors();
    // Cleanup FreeType resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    gl_has_errors();
    std::cout << "Font initialization completed successfully" << std::endl;

    return true;
}

void GlRender::renderText(std::string text, float x, float y, float scale, const glm::vec3 &color)
{
    m_fontShader->use();
    gl_has_errors();

    // Save current blend state
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create orthographic projection matrix
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(M_WINDOW_WIDTH_PX),
                                      static_cast<float>(M_WINDOW_HEIGHT_PX), 0.0f,
                                      -1.0f, 1.0f);

    glm::mat4 transform = glm::mat4(1.0f);

    // Set uniforms
    GLint textColor_location = glGetUniformLocation(m_fontShader->m_shaderProgram, "textColor");
    GLint projectionLoc = glGetUniformLocation(m_fontShader->m_shaderProgram, "projection");
    GLint transformLoc = glGetUniformLocation(m_fontShader->m_shaderProgram, "transform");

    if (textColor_location < 0 || projectionLoc < 0 || transformLoc < 0)
    {
        std::cout << "Failed to get uniform locations" << std::endl;
        return;
    }

    // Set shader uniforms
    glUniform3f(textColor_location, color.x, color.y, color.z);
    gl_has_errors();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    gl_has_errors();
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    gl_has_errors();

    // Configure vertex attributes
    glBindVertexArray(m_font_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
    gl_has_errors();

    // Enable vertex attrib array for position+texture coords
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    gl_has_errors();

    float x_pos = x;
    for (char c : text)
    {
        auto it = m_ftCharacters.find(c);
        if (it == m_ftCharacters.end())
        {
            std::cout << "Character " << c << " not found in font atlas" << std::endl;
            continue;
        }

        Character ch = it->second;

        float xpos = x_pos + ch.Bearing.x * scale;
        float ypos = y + (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            {xpos, ypos - h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos - h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos - h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        gl_has_errors();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        gl_has_errors();

        glDrawArrays(GL_TRIANGLES, 0, 6);
        gl_has_errors();

        x_pos += (ch.Advance >> 6) * scale;
    }

    // Cleanup
    glDisableVertexAttribArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    gl_has_errors();

    // Restore previous blend state
    glBlendFunc(blendSrc, blendDst);
}

void GlRender::renderSimpleButton(float x, float y, float width, float height,
                                  bool isSelected, bool isHovered, bool isPressed,
                                  glm::vec3 tintColor)
{
    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

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

    m_simpleButtonShader->use();
    m_simpleButtonShader->setBool("isSelected", isSelected);
    m_simpleButtonShader->setBool("isHovered", isHovered);
    m_simpleButtonShader->setBool("isPressed", isPressed);
    m_simpleButtonShader->setVec3("tintColor", tintColor); // Add tint color uniform

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GlRender::renderRedHealthRectangle(float x, float y, float width, float height)
{
    // Convert screen coordinates (0, 1280) and (0, 720) to NDC (-1, 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    glUseProgram(m_redRectangleShader->m_shaderProgram);

    // Set uniform values
    GLint fillColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "fillColor");
    glUniform3f(fillColorLoc, 0.4f, 0.0f, 0.0f); // Red fill

    // Define vertices for the rectangle
    float vertices[] = {
        ndcX, ndcY + ndcHeight, 0.0f,           // Top-left
        ndcX, ndcY, 0.0f,                       // Bottom-left
        ndcX + ndcWidth, ndcY, 0.0f,            // Bottom-right
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f // Top-right
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw the rectangle
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GlRender::renderBlueHealthRectangle(float x, float y, float width, float height)
{
    // Convert screen coordinates (0, 1280) and (0, 720) to NDC (-1, 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    glUseProgram(m_redRectangleShader->m_shaderProgram);

    // Set uniform values
    GLint fillColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "fillColor");
    glUniform3f(fillColorLoc, 0.0f, 0.0f, 0.9f); // Blue fill

    // Define vertices for the rectangle
    float vertices[] = {
        ndcX, ndcY + ndcHeight, 0.0f,           // Top-left
        ndcX, ndcY, 0.0f,                       // Bottom-left
        ndcX + ndcWidth, ndcY, 0.0f,            // Bottom-right
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f // Top-right
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw the rectangle
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GlRender::renderSelectorTriangleP1(float x, float y, float width, float height, bool p1)
{
    // Convert screen coordinates (0, 1280) and (0, 720) to NDC (-1, 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    glUseProgram(m_redRectangleShader->m_shaderProgram);

    // Set uniform values
    GLint borderColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "borderColor");
    GLint fillColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "fillColor");
    GLint borderThicknessLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "borderThickness");

    glUniform3f(borderColorLoc, 0.0f, 0.0f, 0.0f); // Black border
    if (game->getState() == GameState::ARCADE_MENU || game->getState() == GameState::CHARACTER_SELECT)
    {

        glUniform3f(fillColorLoc, 0x9e / 255.0f, 0x9b / 255.0f, 0x9e / 255.0f); // gray fill
    }
    else
    {
        if (p1)
        {
            glUniform3f(fillColorLoc, 0.0f, 0.6f, 0.0f); // Green fill
        }
        else
        {
            glUniform3f(fillColorLoc, 0.5f, 0.0f, 0.0f); // Red fill
        }
    }

    // Set border thickness as a fraction of the width

    float borderThickness = 0.05f; // 5% of the rectangle's width
    glUniform1f(borderThicknessLoc, borderThickness);

    // Define vertices for the rectangle
    float vertices[] = {
        ndcX, ndcY, 0.0f,                            // Left-bottom (tip of the triangle)
        ndcX + ndcWidth, ndcY + ndcHeight / 2, 0.0f, // Right-middle (base of the triangle)
        ndcX, ndcY + ndcHeight, 0.0f                 // Left-top (tip of the triangle)
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw the rectangle
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GlRender::renderSelectorTriangleP2(float x, float y, float width, float height, bool p2)
{
    // Convert screen coordinates (0, 1280) and (0, 720) to NDC (-1, 1)
    float ndcX = (2.0f * x / M_WINDOW_WIDTH_PX) - 1.0f;
    float ndcY = 1.0f - (2.0f * y / M_WINDOW_HEIGHT_PX);
    float ndcWidth = 2.0f * width / M_WINDOW_WIDTH_PX;
    float ndcHeight = -2.0f * height / M_WINDOW_HEIGHT_PX;

    glUseProgram(m_redRectangleShader->m_shaderProgram);

    // Set uniform values
    GLint borderColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "borderColor");
    GLint fillColorLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "fillColor");
    GLint borderThicknessLoc = glGetUniformLocation(m_redRectangleShader->m_shaderProgram, "borderThickness");

    glUniform3f(borderColorLoc, 0.0f, 0.0f, 0.0f); // Black border
    if (game->getState() == GameState::ARCADE_MENU || game->getState() == GameState::CHARACTER_SELECT)
    {

        glUniform3f(fillColorLoc, 0x9e / 255.0f, 0x9b / 255.0f, 0x9e / 255.0f); // gray fill
    }
    else
    {
        if (p2)
        {
            glUniform3f(fillColorLoc, 0.0f, 0.6f, 0.0f); // Green fill
        }
        else
        {
            glUniform3f(fillColorLoc, 0.5f, 0.0f, 0.0f); // Red fill
        }
    }

    float borderThickness = 0.05f; // 5% of the rectangle's width
    glUniform1f(borderThicknessLoc, borderThickness);

    // Define vertices for the rectangle
    float vertices[] = {
        ndcX, ndcY + ndcHeight / 2, 0.0f,       // Left (base of the triangle)
        ndcX + ndcWidth, ndcY, 0.0f,            // Right-bottom (tip of the triangle)
        ndcX + ndcWidth, ndcY + ndcHeight, 0.0f // Right-top (tip of the triangle)
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Draw the rectangle
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}