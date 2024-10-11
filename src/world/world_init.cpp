#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"

Entity createPlayer1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();

    // Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::PLAYER_1); 
    // registry.meshPtrs.emplace(entity, &mesh); 

    Health& health = registry.healths.emplace(entity);
    health.currentHealth = 100.f; 
    health.maxHealth = 100.f; 

    Motion&  motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.velocity = {0.f, 0.f};
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

Entity createPlayer2(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();
    return entity;
    // TODO 
};

Entity createOpponent1(GlRender* renderer, vec2 pos) {
    Entity entity = Entity();
    return entity;
    // TODO 
};

Entity createBoundary(vec2 pos, vec2 size) {
    Entity entity = Entity();
    return entity;
    // TODO 
};