#pragma once
#include "../common.hpp"
#include "../mesh.hpp"
#include "../shader.hpp"

enum class PlayerState { 
    IDLE,
    WALKING,
    JUMPING,
    CROUCHING,
    ATTACKING,
    PARRYING,
    PERFECT_PARRYING,
    COUNTER_ATTACKING,
    STUNNED,
    RECOVERING
};

// Player component, from A1 
struct Player {
    int id; // used to separate players, 1 and 2.
};

// AI opponent 
struct Opponent {
    int level; // increase difficulty by increasing level. 
};

struct Health {
    float maxHealth = 100.f;
    float currentHealth = 100.f; 
    // float recoverRate; // optional, health recover after x seconds of not being hit. 
};

struct Motion {
    vec2 position = {0, 0};
    vec2 velocity = {0, 0};
    bool direction = true; // true is facing right, false is facing left. 
    // vec2 acceleration = {0, 0}; // TODO: if we want to add acceleration during initial movement after idle. 
};

struct StationaryTimer {
    float counter_ms = 0.f; // period of time of a player can't move because of stun, or recovery time after actions (e.g. an attack)... 
};

struct PostureBar {
    int maxBar =  10; // max number of bars. 
    int currentBar = 10; // remaining bars.  
    int recoverRate = 3; // how many seconds to recover 1 bar. use int for implicity. 
};

struct StateTimer {
    float duration = 0.f; // time length that this state will last.
    float elapsedTime = 0.f;  // time elapsed since this state started. 
};

struct PlayerInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool punch = false;
    bool kick = false;
};

// TODO: how do we move the box along the player? 
struct HitBox {
    float x, y; 
    float width, height; 
};

struct HurtBox {
    float x, y; 
    float width, height; 
};

struct ParryBox {
    // if hitbox collides ParryBox, the attack is parried. 
    float x, y; 
    float width, height; 
    bool active = false; // active for 12 frames 
};

struct PerfectParryBox {
    float x, y;
    float width, height;
    bool active = false; // active for 3 frames, check if an attack collides with PerfectParryBox 
};

struct Sprite {
    // the sprite file to use for the entity. 
};

struct Boundary {
    float val; // integer to check when comparing the position, x for walls and y for floors
    int dir; // 1: right side wall, 2: left side wall, 3: ground
};

struct BoundaryCollision {
    Entity boundary;
    BoundaryCollision(Entity& boundary) { this->boundary = boundary; };
};


// TODO: add render related components 
struct Renderable {
    Mesh mesh;
    Shader* shader;
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

// we defeined our own meshes?
// // Mesh datastructure for storing vertex and index buffers
// struct Mesh
// {
// 	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
// 	vec2 original_size = {1,1};
// 	std::vector<ColoredVertex> vertices;
// 	std::vector<uint16_t> vertex_indices;
// };

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

enum class TEXTURE_ASSET_ID {
	TEXTURE_COUNT = 0
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	EFFECT_COUNT =0 
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	GEOMETRY_COUNT = 0
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};


// How do we implement boundries, i.e. player can't move beyond boundriess 
// 1. what components are needed 
// 2. how does system logic work on these components 