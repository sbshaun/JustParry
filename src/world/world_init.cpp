#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"

Entity createPlayer1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();

    // Convert 'player' width and height to normalized device coordinates

    std::vector<float> rectangleVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f,  // Top-left
        0 - NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-left
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-right

        // Second triangle (Bottom-right, Top-right, Top-left)
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-right
        0 + NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f,  // Top-right
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f   // Top-left
    };

    // TODO: Should have the registry map the entity to its mesh
    Mesh playerMesh(rectangleVertices);
    Shader* rectShader = new Shader(std::string("player1"));
    // renderer->addMesh(playerMesh, rectShader);

    registry.renderable.insert(entity, Renderable{playerMesh, rectShader});

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
    hitBox.width = PLAYER_1_BB_WIDTH - 10; 
    hitBox.height = PLAYER_1_BB_WIDTH / 2 - 10;
    hitBox.x = pos.x + hitBox.width / 2;
    hitBox.y = pos.y * 0.6; 

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.x = pos.x;
    parryBox.y = pos.y;
    parryBox.width = PLAYER_1_BB_WIDTH;
    parryBox.height = PLAYER_1_BB_HEIGHT;
    parryBox.active = false;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.x = pos.x;
    perfectParryBox.y = pos.y;
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

    // Convert 'player' width and height to normalized device coordinates

    std::vector<float> rectangleVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f,  // Top-left
        0 - NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-left
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-right

        // Second triangle (Bottom-right, Top-right, Top-left)
        0 + NDC_WIDTH / 2, 0 - NDC_HEIGHT / 2, 0.0f,  // Bottom-right
        0 + NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f,  // Top-right
        0 - NDC_WIDTH / 2, 0 + NDC_HEIGHT / 2, 0.0f   // Top-left
    };

    // TODO: Should have the registry map the entity to its mesh
    Mesh playerMesh(rectangleVertices);
    Shader* rectShader = new Shader(std::string("player2"));
    // renderer->addMesh(playerMesh, rectShader);

    registry.renderable.insert(entity, Renderable{ playerMesh, rectShader });

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
    hitBox.x = pos.x + hitBox.width / 2;
    hitBox.y = pos.y * 0.6;

    ParryBox& parryBox = registry.parryBoxes.emplace(entity);
    parryBox.x = pos.x;
    parryBox.y = pos.y;
    parryBox.width = PLAYER_1_BB_WIDTH;
    parryBox.height = PLAYER_1_BB_HEIGHT;
    parryBox.active = false;

    PerfectParryBox& perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.x = pos.x;
    perfectParryBox.y = pos.y;
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

    std::vector<float> floorVertices = {
        -2.0f, -0.5f, 0.0f,
        -2.0f, -1.0f, 0.0f,
         2.0f, -1.0f, 0.0f,

         -2.0f, -0.5f, 0.0f,
         2.0f, -0.5f, 0.0f,
         2.0f, -1.0f, 0.0f
    };

    Mesh floorMesh(floorVertices);
    Shader* floorShader = new Shader(std::string("floor"));

    registry.renderable.insert(floor, Renderable{ floorMesh, floorShader });

    Boundary& boundary = registry.boundaries.emplace(floor);

    boundary.val = -0.5f;
    boundary.dir = 3; // floor

    return floor;
}

