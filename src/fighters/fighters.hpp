#pragma once
#include <unordered_map>
#include "../constants.hpp"
// #include "../ecs/ecs.hpp"

// 1. types of fighters, they have different strengths (config values)
enum Fighters
{
    BIRDMAN,
    PLACEHOLDER
};

class GlRender;

// 2. fighter config struct, stores fighter's strength values
struct FighterConfig
{
    // collision and size
    float BB_WIDTH = PLAYER_BB_WIDTH;
    float BB_HEIGHT = PLAYER_BB_HEIGHT;
    float NDC_WIDTH = BB_WIDTH / M_WINDOW_WIDTH_PX * 2.0f;
    float NDC_HEIGHT = BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f;

    // properties
    float MAX_HEALTH = 100.f;
    int POSTURE_MAX = 7;
    int POSTURE_REGEN = 3;

    // movement
    float MOVESPEED = MOVE_SPEED;
    float JUMP_VELOCITY = 0.2f;

    // attack 1 (punch)
    float HITBOX_DURATION; // this is the time the hitbox is active
    float PUNCH_X_OFFSET = 0.f;
    //float PUNCH_Y_OFFSET = 0.10f * (BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);
	float PUNCH_Y_OFFSET = 0.0f;
    float PUNCH_WIDTH;
    float PUNCH_HEIGHT = 0.15f * (BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f);
    float PUNCH_DAMAGE = 10.f;
    float PUNCH_STUN_DURATION; // time of how long the player itself is stunned after being hit by a punch, not to the other player
    float PARRY_STUN_DURATION; // if a hit is perfect parried, the player is stunned longer than being punched, heavy panelty on failed attack, this prevent abuse of attacking
    float BLOCK_STUN_DURATION; // if a hit is block, the player that blocked will be stunned for a period of time

    // attack 2 (kick) 
    float KICK_HITBOX_DURATION; 
    float KICK_X_OFFSET = 0.f;  
    float KICK_Y_OFFSET; 
    float KICK_WIDTH; 
    float KICK_HEIGHT = 0.15f * (BB_HEIGHT / M_WINDOW_HEIGHT_PX * 2.0f); 
    float KICK_DAMAGE = 10.f; 
    float KICK_STUN_DURATION; 

    float CROUCH_TIMER = 1500.f; // crouching period 

    float PARRY_DURATION; // parry duration (parryBox active time)
    float PERFECT_PARRY_DURATION;


    // knock-back parameters
    float KNOCKBACK_FORCE_X = 0.02f;
    float KNOCKBACK_FORCE_Y = 0.0f;
    float KNOCKBACK_DURATION = 300.f;

    GLuint m_bird_idle_f1_texture;
    GLuint m_bird_idle_f2_texture;
    GLuint m_bird_idle_f3_texture;
    GLuint m_bird_idle_f4_texture;
    GLuint m_bird_punch_f1_texture;
    GLuint m_bird_punch_f2_texture;
    GLuint m_bird_punch_f3_texture;
    GLuint m_bird_kick_f1_texture;
    GLuint m_bird_kick_f2_texture;
    GLuint m_bird_kick_f3_texture;
    GLuint m_bird_kick_f4_texture;
    GLuint m_bird_walk_f1_texture;
    GLuint m_bird_walk_f2_texture;
    GLuint m_bird_walk_f3_texture;
    GLuint m_bird_walk_f4_texture;
    GLuint m_bird_stun_f1_texture;
    GLuint m_bird_parry_f1_texture;
    GLuint m_bird_crouch_f3_texture;

    // default constructor
    FighterConfig(int window_width_px = 1024, int window_height_px = 768)
        : NDC_WIDTH(BB_WIDTH / window_width_px * 2.0f),
          NDC_HEIGHT(BB_HEIGHT / window_height_px * 2.0f),
          PUNCH_Y_OFFSET(0.20f * (BB_HEIGHT / window_height_px * 2.0f)),
          PUNCH_WIDTH(0.50f * (BB_WIDTH / window_width_px * 2.0f)),
          KICK_WIDTH(0.50f * (BB_WIDTH / window_width_px * 2.0f)),
          KICK_Y_OFFSET(-0.40f * (BB_HEIGHT / window_height_px * 2.0f)) {};
};

// 3. fighter manager, this stores configs for all fighters
// stores <Fighter, config>
class FighterManager
{
private:
    static std::unordered_map<Fighters, FighterConfig> fighterConfigs;

public:
    static void init();
    static void setFighterConfig(Fighters fighter, FighterConfig &config);
    static FighterConfig &getFighterConfig(Fighters fighter);
    static void loadBirdTextures(GlRender &renderer);
    static void deleteBirdTextures();
};
