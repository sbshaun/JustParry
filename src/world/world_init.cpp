#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"
#include "../bot.hpp"
#include <stb_image.h>

static void renderHitbox(Entity& player, bool isPlayer1) {
    // register the player as an entity with a hitbox
    HitBox& hitBox = registry.hitBoxes.emplace(player);

    float PUNCH_WIDTH;
    float PUNCH_HEIGHT;
    float PUNCH_X_OFFSET;
    float PUNCH_Y_OFFSET;
    PUNCH_WIDTH = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_WIDTH;
    PUNCH_HEIGHT = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_HEIGHT;
    PUNCH_X_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_X_OFFSET;
    PUNCH_Y_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_Y_OFFSET;
    // TODO: use switch 
    // switch(registry.players.get(player).current_char){
    //     case Fighters::BIRDMAN:
    //     case Fighters::PLACEHOLDER:
    //         PUNCH_WIDTH = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_WIDTH;
    //         PUNCH_HEIGHT = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_HEIGHT;
    //         PUNCH_X_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_X_OFFSET;
    //         PUNCH_Y_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_Y_OFFSET;
    //         break;
    //     //add a default
    //     default:
    //         break;
    // }

    if (isPlayer1) {
        hitBox.width = PUNCH_WIDTH;
        hitBox.height = PUNCH_HEIGHT;
        hitBox.xOffset = PUNCH_X_OFFSET;
        hitBox.yOffset = PUNCH_Y_OFFSET;
        hitBox.active = false;
    }
    else {
        hitBox.width = -PUNCH_WIDTH;
        hitBox.height = PUNCH_HEIGHT;
        hitBox.xOffset = -PUNCH_X_OFFSET;
        hitBox.yOffset = PUNCH_Y_OFFSET;
        hitBox.active = false;
    }
    // Convert 'player' width and height to normalized device coordinates
    std::vector<float> hitboxVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        0 + hitBox.xOffset, 0 + hitBox.yOffset + hitBox.height / 2, 0.0f, // Top-left
        0 + hitBox.xOffset, 0 + hitBox.yOffset - hitBox.height / 2, 0.0f,// Bottom-left
        0 + hitBox.xOffset + hitBox.width, 0 + hitBox.yOffset - hitBox.height / 2, 0.0f, // Bottom-right
        
        // Second triangle (Bottom-right, Top-right, Top-left)
        0 + hitBox.xOffset + hitBox.width, 0 + hitBox.yOffset - hitBox.height / 2, 0.0f, // Bottom-right
        0 + hitBox.xOffset + hitBox.width, 0 + hitBox.yOffset + hitBox.height / 2, 0.0f, // Top-right
        0 + hitBox.xOffset, 0 + hitBox.yOffset + hitBox.height / 2, 0.0f,  // Top-left
    };

    Entity hitBoxEntity = Entity();

    Mesh hitboxMesh(hitboxVertices, false);
    Shader* shader = new Shader(std::string("hitboxes"));
    registry.debugRenders.insert(hitBoxEntity, HitboxRender{ hitboxMesh, shader, player });
}

void setupFighterConfig(Entity entity, const FighterConfig& config) {
    Health& health = registry.healths.emplace(entity);
    health.currentHealth = config.MAX_HEALTH;
    health.maxHealth = config.MAX_HEALTH; 

    PostureBar& postureBar = registry.postureBars.emplace(entity);
    postureBar.currentBar = config.POSTURE_MAX;
    postureBar.maxBar = config.POSTURE_MAX;
    postureBar.recoverRate = config.POSTURE_REGEN; // 3 seconds per bar 

    // HitBox& hitBox = registry.hitBoxes.emplace(entity);
    // hitBox.width = config.PUNCH_WIDTH;
    // hitBox.height = config.PUNCH_HEIGHT;
    // hitBox.xOffset = config.PUNCH_X_OFFSET;
    // hitBox.yOffset = config.PUNCH_Y_OFFSET;
    // hitBox.active = false;
    // hitBox.hit = false;

    CollisionBox& collisionBox = registry.collisionBoxes.emplace(entity);
    collisionBox.width = config.NDC_WIDTH / 8.0f;  
    collisionBox.height = config.NDC_HEIGHT / 8.0f;
    collisionBox.xOffset = 0;
    collisionBox.yOffset = 0;

    HurtBox& hurtBox = registry.hurtBoxes.emplace(entity);
    hurtBox.width = config.NDC_WIDTH / 6.0f;
    hurtBox.height = config.NDC_HEIGHT / 5.0f;
    hurtBox.xOffset = 0;  
    hurtBox.yOffset = 0;

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.width = config.NDC_WIDTH;
    parryBox.height = config.NDC_HEIGHT;
    parryBox.active = false;
    hurtBox.xOffset = 0;
    hurtBox.yOffset = 0;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.width = config.NDC_WIDTH;
    perfectParryBox.height = config.NDC_HEIGHT;
    perfectParryBox.active = false;
    perfectParryBox.xOffset = 0;
    perfectParryBox.yOffset = 0;
}

/*
helper function for player1, player2, and opponent1 
p.s. entity: player entity 
*/
static void createPlayerHelper(Entity& entity, vec2 pos, Shader* shader, GLuint texture, bool isPlayer1, Fighters fighter) {
    // init player's currentState to be IDLE, init stateTimer to 0 
    PlayerCurrentState& playerState = registry.playerCurrentStates.emplace(entity);
    playerState.currentState = PlayerState::IDLE;
    StateTimer& playerStateTimer = registry.stateTimers.emplace(entity);
    playerStateTimer.duration = 0.f;
    playerStateTimer.elapsedTime = 0.f;

    // Convert 'player' width and height to normalized device coordinates
    std::vector<float> rectangleVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f, 0.0f, 1.0f,  // Top-left
        0 - NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f, 0.0f, 0.0f, // Bottom-left
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f, 1.0f, 0.0f, // Bottom-right

        // Second triangle (Bottom-right, Top-right, Top-left)
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f, 1.0f, 0.0f, // Bottom-right
        0 + NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f, 1.0f, 1.0f, // Top-right
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f, 0.0f, 1.0f, // Top-left
    };

    Mesh playerMesh(rectangleVertices, true);
    registry.renderable.insert(entity, Renderable{playerMesh, shader, texture});

    setupFighterConfig(entity, FighterManager::getFighterConfig(fighter));

    Motion&  motion = registry.motions.emplace(entity);
    motion.lastPos = pos;
    motion.position = pos;
    motion.velocity = {0.f, 0.f};
    motion.direction = isPlayer1; // player1 facing right
    motion.inAir = false;   
    motion.scale = { 0.1, 0.1 };

    renderHitbox(entity, isPlayer1);

    PlayerInput playerInput = registry.playerInputs.emplace(entity);
    if (isPlayer1) {
        registry.players.insert(entity, Player{ 1 });
    }
    else {
		registry.players.insert(entity, Player{ 2 });
    }
}

/*
Create player1 entity, init and register components using the helper function above 
*/
Entity createPlayer1(GlRender* renderer, vec2 pos, Fighters fighter) {
    Entity entity = Entity();
    Shader* rectShader = new Shader(std::string("player1"));
    createPlayerHelper(entity, pos, rectShader, renderer->m_bird_texture, true, fighter);
    // set current_char to BIRDMAN by default 
    registry.players.get(entity).current_char = fighter;
    std::cout << "player 1 current_char: " << (int) registry.players.get(entity).current_char << std::endl;
    return entity;
}; 

/*
Create player2 entity, init and register components using the helper function above 
*/
Entity createPlayer2(GlRender* renderer, vec2 pos, Fighters fighter) {
    Entity entity = Entity();
    Shader* rectShader = new Shader(std::string("player2"));
    createPlayerHelper(entity, pos, rectShader, renderer->m_bird_texture, false, fighter);
    registry.players.get(entity).current_char = fighter;
    std::cout << "player 2 current_char: " << (int) registry.players.get(entity).current_char << std::endl;
    return entity;
};

Entity createOpponent1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();
    return entity;
    // TODO 
};

Entity createBoundary(float val, int type) {
    Entity entity = Entity();

    Boundary& boundary = registry.boundaries.emplace(entity);
    boundary.val = val;
    boundary.dir = type;
    return entity;
};

Entity createFloor(float val, int type) {
    Entity floor = Entity();

    std::vector<float> floorVertices = {
        -2.0f, val, 0.0f,
        -2.0f, -1.0f, 0.0f,
         2.0f, -1.0f, 0.0f,

         -2.0f, val, 0.0f,
         2.0f, val, 0.0f,
         2.0f, -1.0f, 0.0f
    };

    Mesh floorMesh(floorVertices, false);
    Shader* floorShader = new Shader(std::string("floor"));

    registry.staticRenders.insert(floor, StaticRender{ floorMesh, floorShader });

    Boundary& boundary = registry.boundaries.emplace(floor);

    boundary.val = val;
    boundary.dir = type; // floor

    return floor;
}

