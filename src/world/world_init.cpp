#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"
#include <stb_image.h>

Entity createPlayer1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();

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

    // TODO: Should have the registry map the entity to its mesh
    Mesh playerMesh(rectangleVertices);
    Shader* rectShader = new Shader(std::string("player1"));

    registry.renderable.insert(entity, Renderable{playerMesh, rectShader, renderer->m_bird_texture});

    Health& health = registry.healths.emplace(entity);
    health.currentHealth = 100.f; 
    health.maxHealth = 100.f; 

    Motion&  motion = registry.motions.emplace(entity);
    motion.lastPos = pos;
    motion.position = pos;
    motion.velocity = {0.f, 0.f};
    motion.direction = true; // facing right 
    motion.inAir = false;

    PostureBar& postureBar = registry.postureBars.emplace(entity);
    postureBar.currentBar = 10;
    postureBar.maxBar = 10;
    postureBar.recoverRate = 3; // 3 seconds per bar 

    HitBox& hitBox = registry.hitBoxes.emplace(entity);
    // hitBox.width = PLAYER_1_BB_WIDTH - 10; 
    // hitBox.height = PLAYER_1_BB_WIDTH / 2 - 10;

    // Use normalized device coordinates instead?
    hitBox.width = PLAYER_1_PUNCH_WIDTH;
	hitBox.height = PLAYER_1_PUNCH_HEIGHT;

    hitBox.xOffset = PLAYER_1_PUNCH_X_OFFSET;
    hitBox.yOffset = PLAYER_1_PUNCH_Y_OFFSET;
    hitBox.active = false;

    // currently set to same size as the player 
    // maybe the offset is not correct. 
    // TODO: double what's the x,y of the player? center or upperleft? Modify.
    // => x, y of the player is the center. We render the player box outward from the center.
    HurtBox& hurtBox = registry.hurtBoxes.emplace(entity);
    hurtBox.width = NDC_WIDTH;
    hurtBox.height = NDC_HEIGHT;
    hurtBox.xOffset = 0;
    hurtBox.yOffset = 0;

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.xOffset = pos.x;
    parryBox.yOffset = pos.y;
    parryBox.width = NDC_WIDTH;
    parryBox.height = NDC_HEIGHT;
    parryBox.active = false;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.xOffset = 0;
    perfectParryBox.yOffset = 0;
    perfectParryBox.width = PLAYER_1_BB_WIDTH;
    perfectParryBox.height = PLAYER_1_BB_HEIGHT;
    perfectParryBox.active = false;

    PlayerInput playerInput = registry.playerInputs.emplace(entity);
    
    // register player1 
    registry.players.emplace(entity);
    
    return entity;
}; 

// the code below is the same as the above function
// to be modified later.
Entity createPlayer2(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();

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

    // TODO: Should have the registry map the entity to its mesh
    Mesh playerMesh(rectangleVertices);
    Shader* rectShader = new Shader(std::string("player2"));
    // renderer->addMesh(playerMesh, rectShader);

    registry.renderable.insert(entity, Renderable{ playerMesh, rectShader, renderer->m_bird_texture });

    Health& health = registry.healths.emplace(entity);
    health.currentHealth = 100.f;
    health.maxHealth = 100.f;

    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.velocity = { 0.f, 0.f };
    motion.direction = true; // facing right 

    PostureBar& postureBar = registry.postureBars.emplace(entity);
    postureBar.currentBar = 10;
    postureBar.maxBar = 10;
    postureBar.recoverRate = 3; // 3 seconds per bar 

    HitBox& hitBox = registry.hitBoxes.emplace(entity);
    hitBox.width = PLAYER_1_BB_WIDTH - 10;
    hitBox.height = PLAYER_1_BB_WIDTH / 2 - 10;
    hitBox.xOffset = hitBox.width / 2;
    hitBox.yOffset = hitBox.height / 2;
    hitBox.active = false;

    // currently set to same size as the player
    // maybe the offset is not correct.
    // TODO: double what's the x,y of the player? center or upperleft? Modify. 
    HurtBox& hurtBox = registry.hurtBoxes.emplace(entity);
    hurtBox.width = PLAYER_1_BB_WIDTH;
    hurtBox.height = PLAYER_1_BB_WIDTH;
    hurtBox.xOffset = -PLAYER_1_BB_WIDTH / 2;
    hurtBox.yOffset = -PLAYER_1_BB_HEIGHT / 2;

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.xOffset = pos.x;
    parryBox.yOffset = pos.y;
    parryBox.width = PLAYER_1_BB_WIDTH;
    parryBox.height = PLAYER_1_BB_HEIGHT;
    parryBox.active = false;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.xOffset = 0;
    perfectParryBox.yOffset = 0;
    perfectParryBox.width = PLAYER_1_BB_WIDTH;
    perfectParryBox.height = PLAYER_1_BB_HEIGHT;
    perfectParryBox.active = false;

    PlayerInput playerInput = registry.playerInputs.emplace(entity);

    // register player1 
    registry.players.emplace(entity);

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

