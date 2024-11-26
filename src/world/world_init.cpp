#include "world_init.hpp"
#include "../ecs/ecs_registry.hpp"
#include "../constants.hpp"
#include "../bot/bot.hpp"
#include <stb_image.h>

int P1_cid = -1;

void setupFighterConfig(Entity entity, const FighterConfig &config, bool isPlayer1)
{
    Health &health = registry.healths.emplace(entity);
    health.currentHealth = config.MAX_HEALTH;
    health.maxHealth = config.MAX_HEALTH;

    PostureBar &postureBar = registry.postureBars.emplace(entity);
    postureBar.currentBar = float(config.POSTURE_MAX);
    postureBar.maxBar = float(config.POSTURE_MAX);
    postureBar.recoverRate = float(config.POSTURE_REGEN); // 3 seconds per bar

    float PUNCH_WIDTH;
    float PUNCH_HEIGHT;
    float PUNCH_X_OFFSET;
    float PUNCH_Y_OFFSET;

    // TODO: Shouldnt assume fighter, read player
    PUNCH_WIDTH = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_WIDTH;
    PUNCH_HEIGHT = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_HEIGHT;
    PUNCH_X_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_X_OFFSET;
    PUNCH_Y_OFFSET = FighterManager::getFighterConfig(Fighters::BIRDMAN).PUNCH_Y_OFFSET;

    HitBox &hitBox = registry.hitBoxes.emplace(entity);
    hitBox.width = config.PUNCH_WIDTH;
    hitBox.height = config.PUNCH_HEIGHT;
    hitBox.xOffset = isPlayer1 ? config.PUNCH_X_OFFSET : -config.PUNCH_X_OFFSET;
    hitBox.yOffset = config.PUNCH_Y_OFFSET;
    hitBox.active = false;

    CollisionBox &collisionBox = registry.collisionBoxes.emplace(entity);
    // apparently this is the width, height, and offset calculation of the collision box
    collisionBox.width = config.NDC_WIDTH / 2.2f;
    collisionBox.height = config.NDC_HEIGHT / 2.7f;
    collisionBox.xOffset = -config.NDC_WIDTH / 5.7f;
    collisionBox.yOffset = 0;

    HurtBox &hurtBox = registry.hurtBoxes.emplace(entity);
    // apparently this is the width, height, and offset calculation of the hurtbox
    hurtBox.width = config.NDC_WIDTH / 1.6f;
    hurtBox.height = config.NDC_HEIGHT / 2.0f;
    hurtBox.xOffset = -config.NDC_WIDTH / 5.7f;
    hurtBox.yOffset = 0;

    // slighly bigger than hurtbox to not overlap when rendering
    ParryBox &parryBox = registry.parryBoxes.emplace(entity);
    parryBox.width = config.NDC_WIDTH / 1.5f;
    parryBox.height = config.NDC_HEIGHT / 1.9f;
    parryBox.xOffset = -config.NDC_WIDTH / 5.7f;
    parryBox.yOffset = 0;
    parryBox.active = false;

    PerfectParryBox &perfectParryBox = registry.perfectParryBoxes.emplace(entity);
    perfectParryBox.width = config.NDC_WIDTH;
    perfectParryBox.height = config.NDC_HEIGHT;
    perfectParryBox.active = false;
    perfectParryBox.xOffset = 0;
    perfectParryBox.yOffset = 0;

    // TODO: Shouldnt  assume fighter, read player
    Animation &animation = registry.animations.emplace(entity);
    animation.currentTexture = config.m_bird_idle_f1_texture;
    animation.currentFrame = 0;
}

/*
helper function for player1, player2, and opponent1
p.s. entity: player entity
*/
static void createPlayerHelper(Entity &entity, vec2 pos, Shader *shader, GlRender *renderer, bool isPlayer1, Fighters fighter)
{
    // init player's currentState to be IDLE, init stateTimer to 0
    PlayerCurrentState &playerState = registry.playerCurrentStates.emplace(entity);
    playerState.currentState = PlayerState::IDLE;
    StateTimer &playerStateTimer = registry.stateTimers.emplace(entity);
    playerStateTimer.duration = 0.f;
    playerStateTimer.elapsedTime = 0.f;
    setupFighterConfig(entity, FighterManager::getFighterConfig(fighter), isPlayer1);

    Fighters current_char = registry.players.get(entity).current_char;
    FighterConfig config = FighterManager::getFighterConfig(current_char);

    Animation &animation = registry.animations.get(entity);

    ObjectMesh &mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::IDLE_BIRD);
    registry.objectMeshPtrs.emplace(entity, &mesh);

    // Convert 'player' width and height to normalized device coordinates
    std::vector<float> rectangleVertices = {
        // First triangle (Top-left, Bottom-left, Bottom-right)
        0 - config.NDC_WIDTH / 2, 0 + config.NDC_HEIGHT / 2, 0.0f, 0.0f, 1.0f, // Top-left
        0 - config.NDC_WIDTH / 2, 0 - config.NDC_HEIGHT / 2, 0.0f, 0.0f, 0.0f, // Bottom-left
        0 + config.NDC_WIDTH / 2, 0 - config.NDC_HEIGHT / 2, 0.0f, 1.0f, 0.0f, // Bottom-right

        // Second triangle (Bottom-right, Top-right, Top-left)
        0 + config.NDC_WIDTH / 2, 0 - config.NDC_HEIGHT / 2, 0.0f, 1.0f, 0.0f, // Bottom-right
        0 + config.NDC_WIDTH / 2, 0 + config.NDC_HEIGHT / 2, 0.0f, 1.0f, 1.0f, // Top-right
        0 - config.NDC_WIDTH / 2, 0 + config.NDC_HEIGHT / 2, 0.0f, 0.0f, 1.0f, // Top-left
    };

    Mesh playerMesh(rectangleVertices, true);
    registry.renderable.insert(entity, Renderable{playerMesh, shader, animation.currentTexture});

    Motion &motion = registry.motions.emplace(entity);
    motion.lastPos = pos;
    motion.position = pos;
    motion.velocity = {0.f, 0.f};
    motion.direction = isPlayer1; // player1 facing right
    motion.inAir = false;
    motion.scale = {1.0f, 1.0f};

    PlayerInput playerInput = registry.playerInputs.emplace(entity);

    registry.players.get(entity).controller_id = -1; // Represents no controller

    registry.knockbacks.emplace(entity);
}

/*
Create player1 entity, init and register components using the helper function above
*/
Entity createPlayer1(GlRender *renderer, vec2 pos, Fighters fighter)
{
    Entity entity = Entity();
    // set current_char to BIRDMAN by default v
    registry.players.insert(entity, Player{1, fighter});
    Shader *rectShader = new Shader(std::string("player1"));
    createPlayerHelper(entity, pos, rectShader, renderer, true, fighter);
    // set current_char to BIRDMAN by default
    // registry.players.get(entity).current_char = fighter;
    for (int cid = GLFW_JOYSTICK_1; cid <= GLFW_JOYSTICK_LAST; ++cid)
    {
        if (glfwJoystickPresent(cid))
        {
            registry.players.get(entity).controller_id = cid;
            P1_cid = cid;
            break;
        }
    }
    std::cout << "player 1 current_char: " << (int)registry.players.get(entity).current_char << std::endl;
    return entity;
};

/*
Create player2 entity, init and register components using the helper function above
*/
Entity createPlayer2(GlRender *renderer, vec2 pos, Fighters fighter)
{
    Entity entity = Entity();
    registry.players.insert(entity, Player{2, fighter});
    Shader *rectShader = new Shader(std::string("player2"));
    createPlayerHelper(entity, pos, rectShader, renderer, false, fighter);
    // registry.players.get(entity).current_char = fighter;
    for (int cid = GLFW_JOYSTICK_1; cid <= GLFW_JOYSTICK_LAST; ++cid)
    {
        if (glfwJoystickPresent(cid) && cid != P1_cid)
        {
            registry.players.get(entity).controller_id = cid;
            break;
        }
    }
    std::cout << "player 2 current_char: " << (int)registry.players.get(entity).current_char << std::endl;
    return entity;
};

Entity createOpponent1(GlRender *renderer, vec2 pos)
{
    Entity entity = Entity();
    return entity;
    // TODO
};

Entity createBoundary(float val, int type)
{
    Entity entity = Entity();

    Boundary &boundary = registry.boundaries.emplace(entity);
    boundary.val = val;
    boundary.dir = type;
    return entity;
};

Entity createPlayableArea(const vec2 &pos, float width, float height)
{
    Entity entity = Entity();

    PlayableArea &playableArea = registry.playableArea.emplace(entity);
    playableArea.position = pos;
    playableArea.width = width;
    playableArea.height = height;

    return entity;
}

Entity createNotification(float counter_ms, bool player1Side, GLuint texture_id)
{
    Entity entity = Entity();

    Notification &notification = registry.notifications.emplace(entity);
    notification.counter_ms = counter_ms;
    notification.player1Side = player1Side;
    notification.texture_id = texture_id;

    return entity;
}

Entity createFloor(float val, int type)
{
    Entity floor = Entity();

    std::vector<float> floorVertices = {
        -2.0f, val, 0.0f,
        -2.0f, -1.0f, 0.0f,
        2.0f, -1.0f, 0.0f,

        -2.0f, val, 0.0f,
        2.0f, val, 0.0f,
        2.0f, -1.0f, 0.0f};

    Mesh floorMesh(floorVertices, false);
    Shader *floorShader = new Shader(std::string("floor"));

    // registry.staticRenders.insert(floor, StaticRender{floorMesh, floorShader});

    Boundary &boundary = registry.boundaries.emplace(floor);

    boundary.val = val;
    boundary.dir = type; // floor

    return floor;
}

// Add cleanup function
void cleanupShaders(Entity entity)
{
    // Clean up renderable shaders
    if (registry.renderable.has(entity))
    {
        Renderable &renderable = registry.renderable.get(entity);
        if (renderable.shader)
        {
            delete renderable.shader;
            renderable.shader = nullptr;
        }
    }

    // Clean up debug render shaders
    for (Entity &debugEntity : registry.debugRenders.entities)
    {
        if (registry.debugRenders.has(debugEntity))
        {
            HitboxRender &hitboxRender = registry.debugRenders.get(debugEntity);
            if (hitboxRender.shader)
            {
                delete hitboxRender.shader;
                hitboxRender.shader = nullptr;
            }
        }
    }

    // Clean up static render shaders
    for (Entity &staticEntity : registry.staticRenders.entities)
    {
        if (registry.staticRenders.has(staticEntity))
        {
            StaticRender &staticRender = registry.staticRenders.get(staticEntity);
            if (staticRender.shader)
            {
                delete staticRender.shader;
                staticRender.shader = nullptr;
            }
        }
    }
}
