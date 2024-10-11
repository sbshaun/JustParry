#include "renderer.hpp"
#include "ecs/ecs_registry.hpp"
#include <iostream>
#include <string>
#include <sstream>

#define GLT_IMPLEMENTATION
#include <GLText.h>


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
}

void GlRender::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto& entity : m_entityMeshes) {
        Shader* shader = entity.second;
        shader->use();
        entity.first.draw();
    }
}


void GlRender::renderUI(int timer) {
    // currently hard coded for two initial entities, 
    // to be updated once everything is added
    const auto& playerhealths = registry.healths;
    const int p1Health = playerhealths.components.at(0).currentHealth;
    const int p2Health = playerhealths.components.at(1).currentHealth;

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

void GlRender::addMesh(const Mesh& mesh, Shader* shader) {
    m_entityMeshes.push_back({ mesh, shader });
}

void GlRender::shutdown() {
    m_entityMeshes.clear();

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
    gltTerminate();

    std::cout << "Resources cleaned up." << std::endl;
}
