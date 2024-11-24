#include "../constants.hpp"
#include "../common.hpp"
#include "../renderer.hpp"
#include "fighters.hpp"

FighterConfig &setUpBirdmanConfig()
{
    static FighterConfig birdmanConfig(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    birdmanConfig.BB_WIDTH = PLAYER_BB_WIDTH;
    birdmanConfig.BB_HEIGHT = PLAYER_BB_HEIGHT;

    birdmanConfig.MAX_HEALTH = 100.f;
    birdmanConfig.POSTURE_MAX = 7;
    birdmanConfig.POSTURE_REGEN = 2000;

    birdmanConfig.MOVESPEED = MOVE_SPEED;
    birdmanConfig.JUMP_VELOCITY = 0.06f / FPS_LOGIC_FACTOR;

    birdmanConfig.HITBOX_DURATION = 1000.f;
    birdmanConfig.PUNCH_X_OFFSET = 0.f;
    birdmanConfig.PUNCH_Y_OFFSET = 0.0f;
    birdmanConfig.PUNCH_DAMAGE = 10.f;
    birdmanConfig.PUNCH_STUN_DURATION = 700.f;
    birdmanConfig.PARRY_STUN_DURATION = 3000.f;
    birdmanConfig.BLOCK_STUN_DURATION = 600.f;

    birdmanConfig.KICK_HITBOX_DURATION = 1000.f;
    birdmanConfig.KICK_X_OFFSET = 0.f;
    birdmanConfig.KICK_DAMAGE = 20.f;
    birdmanConfig.KICK_STUN_DURATION = 700.f;

    birdmanConfig.KNOCKBACK_FORCE_X = 0.015f;
    birdmanConfig.KNOCKBACK_FORCE_Y = 0.0f;

    birdmanConfig.PARRY_DURATION = 700.f;
    birdmanConfig.PERFECT_PARRY_DURATION = 300.f;
    return birdmanConfig;
}

// allocate memory for static variable
std::unordered_map<Fighters, FighterConfig> FighterManager::fighterConfigs;

void FighterManager::setFighterConfig(Fighters fighter, FighterConfig &config)
{
    fighterConfigs[fighter] = config;
}

FighterConfig &FighterManager::getFighterConfig(Fighters fighter)
{
    // check it exists, adn retutrn correct type
    if (fighterConfigs.find(fighter) != fighterConfigs.end())
    {
        return fighterConfigs[fighter];
    }
    else
    {
        std::cerr << "FighterManager::getFighterConfig Fighter not found, returning default." << std::endl;
        return setUpBirdmanConfig();
    }
}

void FighterManager::loadBirdTextures(GlRender &renderer)
{
    auto &birdConfig = FighterManager::getFighterConfig(Fighters::BIRDMAN);
    renderer.loadTexture(textures_path("bird_idle_f1.png"), birdConfig.m_bird_idle_f1_texture);
    renderer.loadTexture(textures_path("bird_idle_f2.png"), birdConfig.m_bird_idle_f2_texture);
    renderer.loadTexture(textures_path("bird_idle_f3.png"), birdConfig.m_bird_idle_f3_texture);
    renderer.loadTexture(textures_path("bird_idle_f4.png"), birdConfig.m_bird_idle_f4_texture);
    renderer.loadTexture(textures_path("bird_punch_f1.png"), birdConfig.m_bird_punch_f1_texture);
    renderer.loadTexture(textures_path("bird_punch_f2.png"), birdConfig.m_bird_punch_f2_texture);
    renderer.loadTexture(textures_path("bird_punch_f3.png"), birdConfig.m_bird_punch_f3_texture);
    renderer.loadTexture(textures_path("bird_kick_f1.png"), birdConfig.m_bird_kick_f1_texture);
    renderer.loadTexture(textures_path("bird_kick_f2.png"), birdConfig.m_bird_kick_f2_texture);
    renderer.loadTexture(textures_path("bird_kick_f3.png"), birdConfig.m_bird_kick_f3_texture);
    renderer.loadTexture(textures_path("bird_kick_f4.png"), birdConfig.m_bird_kick_f4_texture);
    renderer.loadTexture(textures_path("bird_walk_f1.png"), birdConfig.m_bird_walk_f1_texture);
    renderer.loadTexture(textures_path("bird_walk_f2.png"), birdConfig.m_bird_walk_f2_texture);
    renderer.loadTexture(textures_path("bird_walk_f3.png"), birdConfig.m_bird_walk_f3_texture);
    renderer.loadTexture(textures_path("bird_walk_f4.png"), birdConfig.m_bird_walk_f4_texture);
    renderer.loadTexture(textures_path("bird_stun_f1.png"), birdConfig.m_bird_stun_f1_texture);
    renderer.loadTexture(textures_path("bird_parry_f1.png"), birdConfig.m_bird_parry_f1_texture);
};

void FighterManager::deleteBirdTextures()
{
    auto &birdConfig = FighterManager::getFighterConfig(Fighters::BIRDMAN);
    glDeleteTextures(1, &birdConfig.m_bird_idle_f1_texture);
    glDeleteTextures(1, &birdConfig.m_bird_idle_f2_texture);
    glDeleteTextures(1, &birdConfig.m_bird_idle_f3_texture);
    glDeleteTextures(1, &birdConfig.m_bird_idle_f4_texture);
    glDeleteTextures(1, &birdConfig.m_bird_punch_f1_texture);
    glDeleteTextures(1, &birdConfig.m_bird_punch_f2_texture);
    glDeleteTextures(1, &birdConfig.m_bird_punch_f3_texture);
    glDeleteTextures(1, &birdConfig.m_bird_kick_f1_texture);
    glDeleteTextures(1, &birdConfig.m_bird_kick_f2_texture);
    glDeleteTextures(1, &birdConfig.m_bird_kick_f3_texture);
    glDeleteTextures(1, &birdConfig.m_bird_kick_f4_texture);
    glDeleteTextures(1, &birdConfig.m_bird_walk_f1_texture);
    glDeleteTextures(1, &birdConfig.m_bird_walk_f2_texture);
    glDeleteTextures(1, &birdConfig.m_bird_walk_f3_texture);
    glDeleteTextures(1, &birdConfig.m_bird_walk_f4_texture);
    glDeleteTextures(1, &birdConfig.m_bird_stun_f1_texture);
    glDeleteTextures(1, &birdConfig.m_bird_parry_f1_texture);
}

// reigster fighter configs to FighterManager
void FighterManager::init()
{
    FighterManager::setFighterConfig(Fighters::BIRDMAN, setUpBirdmanConfig());
}