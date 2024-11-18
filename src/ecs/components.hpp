#pragma once
#include "../common.hpp"
#include "../mesh.hpp"
#include "../shader.hpp"
#include "../constants.hpp"
#include "ecs.hpp"
#include "../fighters/fighters.hpp"

enum class PlayerState
{
    IDLE,
    WALKING,
    // JUMPING,
    CROUCHING,
    ATTACKING,
    KICKING,
    PARRYING,
    PERFECT_PARRYING,
    COUNTER_ATTACKING,
    STUNNED,
    BLOCKSTUNNED,
    RECOVERING
};

// constexpr: inline function, evaluated at compile time
constexpr const char *PlayerStateToString(PlayerState state)
{
    switch (state)
    {
    case PlayerState::IDLE:
        return "IDLE";
    case PlayerState::WALKING:
        return "WALKING";
    /*case PlayerState::JUMPING:
        return "JUMPING";*/
    case PlayerState::CROUCHING:
        return "CROUCHING";
    case PlayerState::ATTACKING:
        return "ATTACKING";
    case PlayerState::KICKING:
        return "KICKING";
    case PlayerState::PARRYING:
        return "PARRYING";
    case PlayerState::PERFECT_PARRYING:
        return "PERFECT_PARRYING";
    case PlayerState::COUNTER_ATTACKING:
        return "COUNTER_ATTACKING";
    case PlayerState::STUNNED:
        return "STUNNED";
    case PlayerState::BLOCKSTUNNED:
        return "BLOCKSTUNNED";
    case PlayerState::RECOVERING:
        return "RECOVERING";
    default:
        return "UNKNOWN";
    }
}

// Player component, from A1
struct Player
{
    int id;            // used to separate players, 1 and 2.
    int controller_id; // holds the player controller
    Fighters current_char;
};

/*
To transition a player to another state, you should:
1. set the currentState to the new state
2. set the player's stateTimer.duration to be how long the new state will last
p.s. player's default state is IDLE
p.s. when the state timer expires, the player will be transitioned to IDLE
*/
struct PlayerCurrentState
{
    PlayerState currentState = PlayerState::IDLE;
    PlayerState previousState = PlayerState::IDLE;
};

// AI opponent
struct Opponent
{
    int level; // increase difficulty by increasing level.
    Fighters current_char;
};

struct Health
{
    float maxHealth;
    float currentHealth;
    // float recoverRate; // optional, health recover after x seconds of not being hit.
};


struct PostureBar
{
    float maxBar;      // max number of bars.
    float currentBar;  // remaining bars.
    float recoverRate; // how many seconds to recover 1 bar
    float recoverBar;
};

struct Animation
{
    GLuint currentTexture;
    int currentFrame;
    bool playedOnce = false;
};

struct Motion
{
    vec2 lastPos = {0, 0};
    vec2 position = {0, 0};
    vec2 velocity = {0, 0};
    vec2 scale = {1, 1};
    float angle = 0;
    bool direction = true; // true is facing right, false is facing left.
    // vec2 acceleration = {0, 0}; // TODO: if we want to add acceleration during initial movement after idle.

    // temp variable to maintain whether character is jumping or not
    bool inAir = false;

    bool wasAbove = false;
    bool above = false;
};

struct StationaryTimer
{
    float counter_ms = 0.f; // period of time of a player can't move because of stun, or recovery time after actions (e.g. an attack)...
};

struct StateTimer
{
    float duration = 0.f;    // time length that this state will last.
    float elapsedTime = 0.f; // time elapsed since this state started.

    bool isAlive()
    {
        return this->elapsedTime < this->duration;
    }

    void update(float elapsed_ms)
    {
        this->elapsedTime += elapsed_ms;
    }

    void reset(float duration)
    {
        this->duration = duration;
        this->elapsedTime = 0.f;
    }
};

struct PlayerInput
{
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool punch = false;
    bool kick = false;
};

struct KnockBack
{
    bool active = false;
    float duration = 0.0f;
    vec2 force = {0.0f, 0.0f}; // both the x and y force of knockback.
};

struct Box
{
    float xOffset, yOffset; // offsets relative to player's position
    float width, height;

    virtual float getLeft(const vec2 &playerPosition, bool facingRight) const
    {
        // When facing right: hitbox starts at position + xOffset
        // When facing left: hitbox starts at position - xOffset - width
        return facingRight ? playerPosition.x + xOffset : playerPosition.x - xOffset - width;
    }

    virtual float getRight(const vec2 &playerPosition, bool facingRight) const
    {
        // When facing right: hitbox ends at position + xOffset + width
        // When facing left: hitbox ends at position - xOffset
        return facingRight ? playerPosition.x + xOffset + width : playerPosition.x - xOffset;
    }

    virtual float getTop(const vec2 &playerPosition, bool facingRight) const
    {
        return playerPosition.y + yOffset + height / 2;
    }

    virtual float getBottom(const vec2 &playerPosition, bool facingRight) const
    {
        return playerPosition.y + yOffset - height / 2;
    }
};

struct CollisionBox : public Box
{
    bool active = true;

    // hurtbox is just same as player's size. TODO
    float getLeft(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x - width / 2 - xOffset;
        return playerPosition.x - width / 2 + xOffset;
        // return playerPosition.x - width - xOffset;
    }

    float getRight(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x + width / 2 - xOffset;
        return playerPosition.x + width / 2 + xOffset;
        // return playerPosition.x + width + xOffset;
    }

    float getTop(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y + height;
    }

    float getBottom(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y - height;
    }
};

// TODO: how do we move the box along the player?
struct HitBox : public Box
{
    bool active = false; // TODO: active for how  many frames? 12?
    // if the current attack already caused damage
    bool hit = false; // flag to check before applying damage.

    float getLeft(const vec2 &playerPosition, bool facingRight) const override
    {
        // When facing right: hitbox starts at position + xOffset
        // When facing left: hitbox starts at position - xOffset - width
        return facingRight ? playerPosition.x + xOffset : playerPosition.x - xOffset - width;
    }

    float getRight(const vec2 &playerPosition, bool facingRight) const override
    {
        // When facing right: hitbox ends at position + xOffset + width
        // When facing left: hitbox ends at position - xOffset
        return facingRight ? playerPosition.x + xOffset + width : playerPosition.x - xOffset;
    }

    float getTop(const vec2 &playerPosition, bool facingRight) const override
    {
        return playerPosition.y + yOffset + height / 2;
    }

    float getBottom(const vec2 &playerPosition, bool facingRight) const override
    {
        return playerPosition.y + yOffset - height / 2;
    }
};

struct HurtBox : public Box
{
    // hurtbox is just same as player's size. TODO
    float getLeft(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x - width / 2 - xOffset;
        return playerPosition.x - width / 2 + xOffset;
        // return playerPosition.x - width - xOffset;
    }

    float getRight(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x + width / 2 - xOffset;
        return playerPosition.x + width / 2 + xOffset;
        // return playerPosition.x + width + xOffset;
    }

    float getTop(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y + height + yOffset; 
    }

    float getBottom(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y - height + yOffset;
    }
};

struct ParryBox : public Box
{
    // if hitbox collides ParryBox, the attack is parried.
    bool active = false; // active for 12 frames

    // if true, any parry that occurs is a perfect parry
    bool perfectParry = false;

    // parry is just same as player's size. TODO
    float getLeft(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x - width / 2 - xOffset;
        return playerPosition.x - width / 2 + xOffset;
        // return playerPosition.x - width - xOffset;
    }

    float getRight(const vec2 &playerPosition, bool facingRight = true) const override
    {
        if (!facingRight)
            return playerPosition.x + width / 2 - xOffset;
        return playerPosition.x + width / 2 + xOffset;
        // return playerPosition.x + width + xOffset;
    }

    float getTop(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y + height;
    }

    float getBottom(const vec2 &playerPosition, bool facingRight = true) const override
    {
        return playerPosition.y - height;
    }
};

struct PerfectParryBox : public Box
{
    bool active = false; // active for 3 frames, check if an attack collides with PerfectParryBox
};

struct Sprite
{
    // the sprite file to use for the entity.
};

struct Boundary
{
    float val; // integer to check when comparing the position, x for walls and y for floors
    int dir;   // 1: right side wall, 2: left side wall, 3: ground
};

struct PlayableArea
{
    vec2 position = vec2(0, 0);
    float width;
    float height;
    void updatePosition(const vec2 &player1Position, const vec2 &player2Position)
    {
        position.x = clamp((player1Position.x + player2Position.x) / 2.0f, -0.5f, 0.5f);
    }
    void updateWorldModel(mat4 &worldModel)
    {
        worldModel = mat4(1);
        worldModel = glm::translate(worldModel, vec3(-position.x, 0.0f, 0.0f));
    }
};

struct BoundaryCollision
{
    Entity boundary;
    BoundaryCollision(Entity &boundary) { this->boundary = boundary; };
};

// TODO: add render related components
struct Renderable
{
    Mesh mesh;
    Shader *shader;
    unsigned int texture;
};

struct HitboxRender
{
    Mesh mesh;
    Shader *shader;
    // Player that has this hitbox
    Entity player;
};

struct StaticRender
{
    Mesh mesh;
    Shader *shader;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
    vec3 position;
    vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
    vec3 position;
    vec2 texcoord;
};

struct ObjectMesh
{
    static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex> &out_vertices, std::vector<uint16_t> &out_vertex_indices, vec2 &out_size);
    vec2 original_size = {1, 1};
    std::vector<ColoredVertex> vertices;
    std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID
{
    TEXTURE_COUNT = 0
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID
{
    IDLE_BIRD = 0,
    EFFECT_COUNT = IDLE_BIRD + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID
{
    IDLE_BIRD = 0,
    GEOMETRY_COUNT = IDLE_BIRD + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest
{
    TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
    EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
    GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

// How do we implement boundries, i.e. player can't move beyond boundriess
// 1. what components are needed
// 2. how does system logic work on these components