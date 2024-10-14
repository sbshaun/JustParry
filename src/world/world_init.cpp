#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"
#include "../bot.hpp"
#include <stb_image.h>

/*
helper function for player1, player2, and opponent1 
p.s. entity: player entity 
*/
static void createPlayerHelper(GlRender* renderer, Entity entity, vec2 pos, Shader* shader, GLuint texture, bool isPlayer1) {
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

    Mesh playerMesh(rectangleVertices);
    registry.renderable.insert(entity, Renderable{playerMesh, shader, texture});

    Health& health = registry.healths.emplace(entity);
    health.currentHealth = 100.f; 
    health.maxHealth = 100.f; 

    Motion&  motion = registry.motions.emplace(entity);
    motion.lastPos = pos;
    motion.position = pos;
    motion.velocity = {0.f, 0.f};
    motion.direction = isPlayer1; // player1 facing right
    motion.inAir = false;   

    PostureBar& postureBar = registry.postureBars.emplace(entity);
    postureBar.currentBar = 10;
    postureBar.maxBar = 10;
    postureBar.recoverRate = 3; // 3 seconds per bar 

    HitBox& hitBox = registry.hitBoxes.emplace(entity);
    // Use normalized device coordinates instead?
    hitBox.width = PLAYER_1_PUNCH_WIDTH;
	hitBox.height = PLAYER_1_PUNCH_HEIGHT;
    hitBox.xOffset = PLAYER_1_PUNCH_X_OFFSET;
    hitBox.yOffset = PLAYER_1_PUNCH_Y_OFFSET;
    hitBox.active = false;

    HurtBox& hurtBox = registry.hurtBoxes.emplace(entity);
    hurtBox.xOffset = 0;
    hurtBox.yOffset = 0;
    hurtBox.width = NDC_WIDTH;
    hurtBox.height = NDC_HEIGHT;

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.active = false;
    parryBox.xOffset = pos.x;
    parryBox.yOffset = pos.y;
    parryBox.width = NDC_WIDTH;
    parryBox.height = NDC_HEIGHT;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.xOffset = 0;
    perfectParryBox.yOffset = 0;
    perfectParryBox.active = false;
    perfectParryBox.width = NDC_WIDTH;
    perfectParryBox.height = NDC_HEIGHT;

    PlayerInput playerInput = registry.playerInputs.emplace(entity);
    registry.players.emplace(entity);
}

/*
Create player1 entity, init and register components using the helper function above 
*/
Entity createPlayer1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();
    Shader* rectShader = new Shader(std::string("player1"));
    createPlayerHelper(renderer, entity, pos, rectShader, renderer->m_bird_texture, true);    
    return entity;
}; 

/*
Create player2 entity, init and register components using the helper function above 
*/
Entity createPlayer2(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();
    Shader* rectShader = new Shader(std::string("player2"));
    createPlayerHelper(renderer, entity, pos, rectShader, renderer->m_bird_texture, false);    
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

Entity createFloor(GlRender* renderer) {
    Entity floor = Entity();

    /*std::vector<float> floorVertices = {
        -2.0f, -0.5f, 0.0f,
        -2.0f, -1.0f, 0.0f,
         2.0f, -1.0f, 0.0f,

         -2.0f, -0.5f, 0.0f,
         2.0f, -0.5f, 0.0f,
         2.0f, -1.0f, 0.0f
    };

    Mesh floorMesh(floorVertices);
    Shader* floorShader = new Shader(std::string("floor"));

    registry.renderable.insert(floor, Renderable{ floorMesh, floorShader });*/

    Boundary& boundary = registry.boundaries.emplace(floor);

    boundary.val = -0.7f;
    boundary.dir = 3; // floor

    return floor;
}

