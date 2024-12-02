#include "../constants.hpp"
#include "../common.hpp"
#include "../graphics/renderer.hpp"
#include "fighters.hpp"

FighterConfig &setUpBirdmanConfig()
{
    static FighterConfig birdmanConfig(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    birdmanConfig.BB_WIDTH = 2.5f * 80.f;
    birdmanConfig.BB_HEIGHT = 2.5f * 100.f;

    birdmanConfig.NDC_WIDTH = birdmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f;
    birdmanConfig.NDC_HEIGHT = birdmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f;

    birdmanConfig.PUNCH_WIDTH = 0.50f * (birdmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f);
    birdmanConfig.PUNCH_HEIGHT = 0.15f * (birdmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    birdmanConfig.KICK_WIDTH = 0.50f * (birdmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f);
    birdmanConfig.KICK_HEIGHT = 0.15f * (birdmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    birdmanConfig.KICK_Y_OFFSET = -0.40f * (birdmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    birdmanConfig.MAX_HEALTH = 100.f;
    birdmanConfig.POSTURE_MAX = 7;
    birdmanConfig.POSTURE_REGEN = 5000.f;
    birdmanConfig.PERFECT_PARRY_TIME = 300.f;

    birdmanConfig.MOVESPEED = 0.02f / FPS_LOGIC_FACTOR;
    birdmanConfig.JUMP_VELOCITY = 0.06f / FPS_LOGIC_FACTOR;



    birdmanConfig.HITBOX_DURATION = 1000.f;
    birdmanConfig.PUNCH_X_OFFSET = 0.f;
    birdmanConfig.PUNCH_Y_OFFSET = 0.0f;
    birdmanConfig.PUNCH_DAMAGE = 10.f;
    birdmanConfig.PUNCH_STUN_DURATION = 700.f;
    birdmanConfig.PARRY_STUN_DURATION = 3000.f;
    birdmanConfig.BLOCK_STUN_DURATION = 600.f;

    birdmanConfig.KICK_HITBOX_DURATION = 1500.f;
    birdmanConfig.KICK_X_OFFSET = 0.f;
    birdmanConfig.KICK_DAMAGE = 20.f;
    birdmanConfig.KICK_STUN_DURATION = 900.f;

    birdmanConfig.KNOCKBACK_FORCE_X = 0.015f;
    birdmanConfig.KNOCKBACK_FORCE_Y = 0.0f;

    birdmanConfig.PARRY_DURATION = 700.f;
    birdmanConfig.PERFECT_PARRY_DURATION = 300.f;
    return birdmanConfig;
}

// Bearman setup 
// 
FighterConfig &setUpBearmanConfig() {
    static FighterConfig bearmanConfig(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    bearmanConfig.BB_WIDTH = 3.5f * 80.f; // wider hitbox than birdman, making it easier to be hit s
    bearmanConfig.BB_HEIGHT = 2.5f * 100.f;

    bearmanConfig.NDC_WIDTH = bearmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f;
    bearmanConfig.NDC_HEIGHT = bearmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f;

    bearmanConfig.PUNCH_WIDTH = 0.65f * (bearmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f); // 30% longer than birdman 
    bearmanConfig.PUNCH_HEIGHT = 0.15f * (bearmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    bearmanConfig.KICK_WIDTH = 0.65f * (bearmanConfig.BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f); // approx 30% longer than birdman 
    bearmanConfig.KICK_HEIGHT = 0.15f * (bearmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    bearmanConfig.KICK_Y_OFFSET = -0.40f * (bearmanConfig.BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);

    bearmanConfig.MAX_HEALTH = 100.f;
    bearmanConfig.POSTURE_MAX = 4; // less posture than birdman, making it less able to parry 
    bearmanConfig.POSTURE_REGEN = 7000.f; // slower regen than birdman 
    bearmanConfig.PERFECT_PARRY_TIME = 200.f; // shorter than birdman 

    bearmanConfig.MOVESPEED = 0.015f / FPS_LOGIC_FACTOR; // 25% slower than birdman 
    bearmanConfig.JUMP_VELOCITY = 0.06f / FPS_LOGIC_FACTOR;

    bearmanConfig.HITBOX_DURATION = 1500.f; // longer than birdman 
    bearmanConfig.PUNCH_X_OFFSET = 0.f;
    bearmanConfig.PUNCH_Y_OFFSET = 0.0f;
    bearmanConfig.PUNCH_DAMAGE = 13.f; // 30% more damage than birdman 
    bearmanConfig.PUNCH_STUN_DURATION = 900.f; // longer stun than birdman
    bearmanConfig.PARRY_STUN_DURATION = 3500.f; // stunned itslef longger than birdman 
    bearmanConfig.BLOCK_STUN_DURATION = 600.f;

    bearmanConfig.KICK_HITBOX_DURATION = 1300.f; // 30% longer than birdman 
    bearmanConfig.KICK_X_OFFSET = 0.f;
    bearmanConfig.KICK_DAMAGE = 25.f; // 25% more damage than birdman 
    bearmanConfig.KICK_STUN_DURATION = 900.f;

    bearmanConfig.KNOCKBACK_FORCE_X = 0.025f; // more knockback than birdman
    bearmanConfig.KNOCKBACK_FORCE_Y = 0.0f;

    bearmanConfig.PARRY_DURATION = 1000.f; // longer normal parry window, but shorter perfect parry window
    bearmanConfig.PERFECT_PARRY_DURATION = 250.f; // shorter perfect parry window than birdman 
    return bearmanConfig;
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
    renderer.loadTexture(textures_path("bird_crouch_f3.png"), birdConfig.m_bird_crouch_f3_texture);
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
    glDeleteTextures(1, &birdConfig.m_bird_crouch_f3_texture);
}

void FighterManager::loadBearTextures(GlRender &renderer) {
    auto &bearman = FighterManager::getFighterConfig(Fighters::BEARMAN);
    renderer.loadTexture(textures_path("bear_idle_f1.png"), bearman.m_bird_idle_f1_texture);
    renderer.loadTexture(textures_path("bear_idle_f2.png"), bearman.m_bird_idle_f2_texture);
    renderer.loadTexture(textures_path("bear_idle_f3.png"), bearman.m_bird_idle_f3_texture);
    renderer.loadTexture(textures_path("bear_idle_f4.png"), bearman.m_bird_idle_f4_texture);
    renderer.loadTexture(textures_path("bear_punch_f1.png"), bearman.m_bird_punch_f1_texture);
    renderer.loadTexture(textures_path("bear_punch_f2.png"), bearman.m_bird_punch_f2_texture);
    renderer.loadTexture(textures_path("bear_punch_f3.png"), bearman.m_bird_punch_f3_texture);
    renderer.loadTexture(textures_path("bear_kick_f1.png"), bearman.m_bird_kick_f1_texture);
    renderer.loadTexture(textures_path("bear_kick_f2.png"), bearman.m_bird_kick_f2_texture);
    renderer.loadTexture(textures_path("bear_kick_f3.png"), bearman.m_bird_kick_f3_texture);
    renderer.loadTexture(textures_path("bear_kick_f4.png"), bearman.m_bird_kick_f4_texture);
    renderer.loadTexture(textures_path("bear_walk_f1.png"), bearman.m_bird_walk_f1_texture);
    renderer.loadTexture(textures_path("bear_walk_f2.png"), bearman.m_bird_walk_f2_texture);
    renderer.loadTexture(textures_path("bear_walk_f3.png"), bearman.m_bird_walk_f3_texture);
    renderer.loadTexture(textures_path("bear_walk_f4.png"), bearman.m_bird_walk_f4_texture);
    renderer.loadTexture(textures_path("bear_stun_f1.png"), bearman.m_bird_stun_f1_texture);
    renderer.loadTexture(textures_path("bear_parry_f1.png"), bearman.m_bird_parry_f1_texture);
    renderer.loadTexture(textures_path("bear_crouch_f3.png"), bearman.m_bird_crouch_f3_texture);
}

void FighterManager::deleteBearTextures() {
    auto &bearman = FighterManager::getFighterConfig(Fighters::BEARMAN);
    glDeleteTextures(1, &bearman.m_bird_idle_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_idle_f2_texture);
    glDeleteTextures(1, &bearman.m_bird_idle_f3_texture);
    glDeleteTextures(1, &bearman.m_bird_idle_f4_texture);
    glDeleteTextures(1, &bearman.m_bird_punch_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_punch_f2_texture);
    glDeleteTextures(1, &bearman.m_bird_punch_f3_texture);
    glDeleteTextures(1, &bearman.m_bird_kick_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_kick_f2_texture);
    glDeleteTextures(1, &bearman.m_bird_kick_f3_texture);
    glDeleteTextures(1, &bearman.m_bird_kick_f4_texture);
    glDeleteTextures(1, &bearman.m_bird_walk_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_walk_f2_texture);
    glDeleteTextures(1, &bearman.m_bird_walk_f3_texture);
    glDeleteTextures(1, &bearman.m_bird_walk_f4_texture);
    glDeleteTextures(1, &bearman.m_bird_stun_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_parry_f1_texture);
    glDeleteTextures(1, &bearman.m_bird_crouch_f3_texture);
}

// reigster fighter configs to FighterManager
void FighterManager::init()
{
    FighterManager::setFighterConfig(Fighters::BIRDMAN, setUpBirdmanConfig());
    FighterManager::setFighterConfig(Fighters::BEARMAN, setUpBearmanConfig());
}