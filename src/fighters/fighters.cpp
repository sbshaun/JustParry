#include "../constants.hpp"
#include "../common.hpp"
#include "fighters.hpp"

FighterConfig& setUpBirdmanConfig() {
    static FighterConfig birdmanConfig(M_WINDOW_WIDTH_PX, M_WINDOW_HEIGHT_PX);
    birdmanConfig.BB_WIDTH = 3.0f * 80.f;  
    birdmanConfig.BB_HEIGHT = 3.0f * 100.f; 

    birdmanConfig.MAX_HEALTH = 100.f;
    birdmanConfig.POSTURE_MAX = 10;
    birdmanConfig.POSTURE_REGEN = 3;

    birdmanConfig.MOVE_SPEED = 0.005f/FPS_LOGIC_FACTOR;
    birdmanConfig.JUMP_VELOCITY = 0.06f/FPS_LOGIC_FACTOR;

    birdmanConfig.HITBOX_DURATION = 1500.f;
    birdmanConfig.PUNCH_X_OFFSET = 0.f;
    birdmanConfig.PUNCH_DAMAGE = 10.f;
    birdmanConfig.PUNCH_STUN_DURATION = 700.f;
    birdmanConfig.PARRY_STUN_DURATION = 3000.f;

    birdmanConfig.PARRY_DURATION = 700.f;

    birdmanConfig.main_texture = "bird.png";
    birdmanConfig.punch_texture = "bird_p.png";
    return birdmanConfig; 
}

// allocate memory for static variable 
std::unordered_map<Fighters, FighterConfig> FighterManager::fighterConfigs;

void FighterManager::setFighterConfig(Fighters fighter, FighterConfig& config) {
    fighterConfigs[fighter] = config;
} 

const FighterConfig& FighterManager::getFighterConfig(Fighters fighter) {
    // check it exists, adn retutrn correct type 
    if (fighterConfigs.find(fighter) != fighterConfigs.end()) {
        return fighterConfigs[fighter];
    } else {
        std::cerr << "FighterManager::getFighterConfig Fighter not found, returning default." << std::endl;
        return setUpBirdmanConfig();
    }
}

// reigster fighter configs to FighterManager 
void FighterManager::init() {
    FighterManager::setFighterConfig(Fighters::BIRDMAN, setUpBirdmanConfig());
}