#include "renderer.hpp"


GlRender::GlRender() {}

GlRender::~GlRender() {}

void GlRender::initialize() {
    glEnable(GL_DEPTH_TEST);
    gltInit();
    initializeUI();
}


void GlRender::initializeUI() {
    m_timerText = gltCreateText();
    assert(m_timerText);  // Check if it's not NULL
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
}

void GlRender::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // debugging wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    for (Entity& entity : registry.renderable.entities) {
        Renderable& mesh_shader = registry.renderable.get(entity);
        Shader* shader = mesh_shader.shader;
        Mesh &mesh = mesh_shader.mesh;
        shader->use();
        
        Motion& motion = registry.motions.get(entity);
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(motion.position.x, motion.position.y, 0.0f));

        shader->setMat4("model", modelMatrix);

        mesh.draw();
    }
}


void GlRender::renderUI(int timer) {
    // currently hard coded for two initial entities, 
    // to be updated once everything is added
    const auto& playerhealths = registry.healths;

    const int p1Health = registry.healths.get(m_player1).currentHealth;
    const int p2Health = registry.healths.get(m_player2).currentHealth;
    
    // if(timer == 0){
    //     gltDrawText2D(m_roundOver, 512, 400, 1.f);
    // }

    // Render timer and health text
    if (m_timerText && m_leftText && m_rightText) {
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
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);  // White text color

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

void GlRender::shutdown() {
    if (m_timerText) {
        gltDeleteText(m_timerText);
    }
    if (m_leftText) {
        gltDeleteText(m_leftText);
    }
    if (m_rightText) {
        gltDeleteText(m_rightText);
    }
    if (h1) {
        gltDeleteText(h1);
    }
    if (h2) {
        gltDeleteText(h2);
    }
    if (time) {
        gltDeleteText(time);
    }
    if (m_roundOver) {
        gltDeleteText(m_roundOver);
    }
    gltTerminate();

    std::cout << "Resources cleaned up." << std::endl;
}
