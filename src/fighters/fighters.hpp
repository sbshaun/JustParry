#pragma once
#include <unordered_map>
#include "../constants.hpp"
// #include "../ecs/ecs.hpp"

// 1. types of fighters, they have different strengths (config values) 
enum Fighters{
    BIRDMAN
};

// 2. fighter config struct, stores fighter's strength values 
struct FighterConfig {
    // collision and size 
    float BB_WIDTH = 3.0f * 80.f; 
    float BB_HEIGHT = 3.0f * 100.f;
    float NDC_WIDTH;
    float NDC_HEIGHT;

    // properties 
    float MAX_HEALTH = 100.f;
    int POSTURE_MAX = 10;
    int POSTURE_REGEN = 3;

    // movement 
    float MOVE_SPEED = 0.005f;
    float JUMP_VELOCITY = 0.02f;

    // attack 1 (punch) 
    int HITBOX_DURATION = 500;
    float PUNCH_X_OFFSET = 0.f;
    float PUNCH_Y_OFFSET = 0.15f * NDC_HEIGHT;
    float PUNCH_WIDTH;
    float PUNCH_HEIGHT;
    float PUNCH_DAMAGE = 10.f;
    float PUNCH_STUN_DURATION = 3000.f;

    //Graphics 
    //path to main texture 
    std::string main_texture = "bird.png";
    //path to animations 
    std::string punch_texture = "bird_p.png";

    // default constructor 
    FighterConfig(int window_width_px = 1024, int window_height_px = 768)
        : NDC_WIDTH(BB_WIDTH / window_width_px * 2.0f),
          NDC_HEIGHT(BB_HEIGHT / window_height_px * 2.0f),
          PUNCH_Y_OFFSET(0.15f * (BB_HEIGHT / window_height_px * 2.0f)),
          PUNCH_WIDTH(0.50f * (BB_WIDTH / window_width_px * 2.0f)),
          PUNCH_HEIGHT(0.20f * (BB_HEIGHT / window_height_px * 2.0f))
    {};
};

// 3. fighter manager, this stores configs for all fighters 
// stores <Fighter, config> 
class FighterManager {
    private:
        static std::unordered_map<Fighters, FighterConfig> fighterConfigs;
        // <Entity player, Fighters> map 
        // static std::unordered_map<unsigned int, Fighters> playerFighterMap;
    public:
        static void init();
        static void setFighterConfig(Fighters fighter, FighterConfig& config);
        static const FighterConfig& getFighterConfig(Fighters fighter);
        // static void setPlayerFighter(Entity player, Fighters fighter) { playerFighterMap[player] = fighter; }
        // static Fighters getPlayerFighter(Entity player) { return playerFighterMap[player]; }
};
