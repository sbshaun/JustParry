#include "world_system.hpp"
#include "world_init.hpp"

#include "../physics/physics_system.hpp"
#include "../constants.hpp"
#include "../input_system/input_utils.hpp"

/* notes: 
1. a helper function to check if player is movable. 
2. static: scoped to this file, since it's just a small helper function 
3. placed at the top for it to be declared before being used, otherwise compiler error. 
*/
static bool canMove(PlayerState state) {
    return state != PlayerState::ATTACKING
        && state != PlayerState::STUNNED
        && state != PlayerState::RECOVERING
        && state != PlayerState::PARRYING
        && state != PlayerState::PERFECT_PARRYING
        && state != PlayerState::COUNTER_ATTACKING;
}

/*
helper funciton to apply damage to a player 
*/
static void applyDamage(Entity player, float damage) {
    Health& health = registry.healths.get(player);

    health.currentHealth -= damage;

    if (health.currentHealth <= 0) {
        health.currentHealth = 0;
    }

    std::cout << "Damange: " << damage << std::endl; 
    std::cout << "Player " << (unsigned int) player << " remaining health: " << health.currentHealth << std::endl; 
}

WorldSystem::WorldSystem() {};

WorldSystem::~WorldSystem() {
	registry.clear_all_components();
}

void WorldSystem::init(GlRender *renderer) {
	this->renderer = renderer;

	// Create entities
	Entity player1 = createPlayer1(renderer, { -1.25, FLOOR_Y + NDC_HEIGHT }, Fighters::BIRDMAN);
	Entity player2 = createPlayer2(renderer, { 1.25, FLOOR_Y + NDC_HEIGHT }, Fighters::PLACEHOLDER);

	renderer->m_player1 = player1;
	renderer->m_player2 = player2;

    //defined BOUND_SIZE in constants
    Entity boundaryRight = createBoundary(BOUND_SIZE_R, RIGHT);
    Entity boundaryLeft = createBoundary(BOUND_SIZE_L, LEFT);
    Entity boundaryFloor = createFloor(FLOOR_Y, FLOOR);

    // 1. init input handlers and state machines 
    initInputHandlers();
    initStateMachines();
}

void WorldSystem::initInputHandlers() {
    // init default keys for 2 players
    // 1. player 1 <input_key -> action> mapping 
    std::unique_ptr<InputMapping> player1InputMapping = std::make_unique<InputMapping>();
    player1InputMapping->bindKeyToAction(GLFW_KEY_A, Action::MOVE_LEFT);
    player1InputMapping->bindKeyToAction(GLFW_KEY_D, Action::MOVE_RIGHT);
    player1InputMapping->bindKeyToAction(GLFW_KEY_W, Action::JUMP);
    player1InputMapping->bindKeyToAction(GLFW_KEY_S, Action::CROUCH);
    player1InputMapping->bindKeyToAction(GLFW_KEY_R, Action::PUNCH);
    player1InputMapping->bindKeyToAction(GLFW_KEY_T, Action::KICK);

    // 2. player 2 <input_key -> action> mapping
    std::unique_ptr<InputMapping> player2InputMapping = std::make_unique<InputMapping>();
    player2InputMapping->bindKeyToAction(GLFW_KEY_LEFT, Action::MOVE_LEFT);
    player2InputMapping->bindKeyToAction(GLFW_KEY_RIGHT, Action::MOVE_RIGHT);
    player2InputMapping->bindKeyToAction(GLFW_KEY_UP, Action::JUMP);
    player2InputMapping->bindKeyToAction(GLFW_KEY_DOWN, Action::CROUCH);
    player2InputMapping->bindKeyToAction(GLFW_KEY_COMMA, Action::PUNCH);
    player2InputMapping->bindKeyToAction(GLFW_KEY_PERIOD, Action::KICK);

    // 3. player 1 and player 2 init <action -> command> mapping 
    player1InputHandler = std::make_unique<InputHandler>(std::move(player1InputMapping));
    player2InputHandler = std::make_unique<InputHandler>(std::move(player2InputMapping));
    player1InputHandler->initDefaultActionToCommandMapping();
    player2InputHandler->initDefaultActionToCommandMapping();
}

void WorldSystem::initStateMachines() {
    player1StateMachine = std::make_unique<StateMachine>();
    player2StateMachine = std::make_unique<StateMachine>();

    player1StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player1StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    player1StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player1StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player1StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player1StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());

    player2StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player2StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    player2StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player2StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player2StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player2StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
}

//IN THE FUTURE WE SHOULD MAKE THE ENTITY LOOPING A SINGLE FUNCTION AND ALL THE PROCESSING PER LOOP HELPERS SO WE ONLY ITERATE THROUGH THE ENTITIES ONCE PER GAME CYCLE

void WorldSystem::handlePlayerInput(Entity player, InputHandler& inputHandler, StateMachine& stateMachine) {
    auto& motion = registry.motions.get(player);
    auto& input  = registry.playerInputs.get(player);

    // Reset inputs
    input = PlayerInput();

    input.left = isKeyPressed(inputHandler.getKeyFromAction(Action::MOVE_LEFT));
    input.right = isKeyPressed(inputHandler.getKeyFromAction(Action::MOVE_RIGHT));
    input.up = isKeyPressed(inputHandler.getKeyFromAction(Action::JUMP));
    input.down = isKeyPressed(inputHandler.getKeyFromAction(Action::CROUCH));
    input.punch = isKeyPressed(inputHandler.getKeyFromAction(Action::PUNCH));
    input.kick = isKeyPressed(inputHandler.getKeyFromAction(Action::KICK));
}

void WorldSystem::handleInput() {
    player1InputHandler->handleInput(renderer->m_player1, *player1StateMachine);
    if (!BOT_ENABLED) player2InputHandler->handleInput(renderer->m_player2, *player2StateMachine);
    player1StateMachine->update(renderer->m_player1, TIME_STEP);
    player2StateMachine->update(renderer->m_player2, TIME_STEP);
}


void WorldSystem::inputProcessing(int timer) {
    // TODO: manage input based on PlayerState more effectively 
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);
    // TODO: what are these 2 lines for? 
    // removing them seems to break the player rendering 
    player1Motion.lastPos = player1Motion.position;
    player2Motion.lastPos = player2Motion.position;

    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);
    if (player1Input.punch) player1StateMachine->transition(renderer->m_player1, PlayerState::ATTACKING);
    if (player2Input.punch) player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);
    // TODO: handle up, down, punch, kick
}

void WorldSystem::movementProcessing() {
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);

    PlayerCurrentState& player1State = registry.playerCurrentStates.get(renderer->m_player1);
    PlayerCurrentState& player2State = registry.playerCurrentStates.get(renderer->m_player2);

    if (canMove(player1State.currentState)) {
            // player can only move if not in these non-moveable states. 
            player1Motion.position += player1Motion.velocity;
    } else {
        player1Motion.position.y += player1Motion.velocity.y;
        // std::cout << "Player 1 cannot move, current state: " << PlayerStateToString(player1State.currentState) << std::endl;
    }

    if (canMove(player2State.currentState)) {
            // player can only move if not in these non-moveable states. 
            player2Motion.position += player2Motion.velocity;
    } else {
        player2Motion.position.y += player2Motion.velocity.y;
        // std::cout << "Player 2 cannot move, current state: " << PlayerStateToString(player2State.currentState) << std::endl;
    }

    if (player1Motion.position.x > player2Motion.position.x) {
        player1Motion.direction = false; // Player 1 now facing left
        player2Motion.direction = true; // Player 2 now facing right
    } else if (player1Motion.position.x < player2Motion.position.x) {
        player1Motion.direction = true; // Player 1 now facing right
        player2Motion.direction = false; // Player 2 now facing left
    }
}

void WorldSystem::updateStateTimers(float elapsed_ms) {
    player1StateMachine->update(renderer->m_player1, elapsed_ms);
    player2StateMachine->update(renderer->m_player2, elapsed_ms);
    StateTimer& player1StateTimer = registry.stateTimers.get(renderer->m_player1);
    StateTimer& player2StateTimer = registry.stateTimers.get(renderer->m_player2);

    HitBox& player1HitBox = registry.hitBoxes.get(renderer->m_player1);
    HitBox& player2HitBox = registry.hitBoxes.get(renderer->m_player2);
}

bool WorldSystem::checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox) {
    Motion& hitPlayerMotion = registry.motions.get(playerWithHitBox);
    Motion& hurtPlayerMotion = registry.motions.get(playerWithHurtBox);
    HitBox& hitBox = registry.hitBoxes.get(playerWithHitBox);
    HurtBox& hurtBox = registry.hurtBoxes.get(playerWithHurtBox);

    
    // if  hitbox is not active, skip check 
    if (!hitBox.active ||  hitBox.hit) return false; 

    bool collision = false;

    float hitBoxLeft = hitBox.getLeft(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxRight = hitBox.getRight(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxTop = hitBox.getTop(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxBottom = hitBox.getBottom(hitPlayerMotion.position, hitPlayerMotion.direction);
    // std::cout << "player's postion: " << hitPlayerMotion.position.x << ", " << hitPlayerMotion.position.y << std::endl;
    // std::cout << "Hitbox 4 corners, left, right, top, bottom: " << hitBoxLeft << ", " << hitBoxRight << ", " << hitBoxTop << ", " << hitBoxBottom << std::endl;

    float hurtBoxLeft = hurtBox.getLeft(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxRight = hurtBox.getRight(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxTop = hurtBox.getTop(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxBottom = hurtBox.getBottom(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    std::cout << "player's postion: " << hurtPlayerMotion.position.x << ", " << hurtPlayerMotion.position.y << std::endl;
    std::cout << "Hurtbox 4 corners, left, right, top, bottom: " << hurtBoxLeft << ", " << hurtBoxRight << ", " << hurtBoxTop << ", " << hurtBoxBottom << std::endl;

    // std::cout << "collision: " << collision << std::endl;

    // AABB collision check
    bool x_collision = hitBoxLeft < hurtBoxRight && hitBoxRight > hurtBoxLeft;
    bool y_collision = hitBoxTop > hurtBoxBottom && hitBoxBottom < hurtBoxTop;
    
    if (x_collision && y_collision) {
        hitBox.hit = true;
        // std::cout << "Hitbox of Player " << (unsigned int) playerWithHitBox << " collided with Hurtbox of Player" << (unsigned int) playerWithHurtBox << std::endl;
        collision = true;
    }
    
    return collision;
};

// helper function for AABB Collision
bool WorldSystem::checkAABBCollision(const Box& box1, const vec2& position1, bool facingRight1,
    const Box& box2, const vec2& position2, bool facingRight2) {

    float box1Left = box1.getLeft(position1, facingRight1);
    float box1Right = box1.getRight(position1, facingRight1);
    float box1Top = box1.getTop(position1, facingRight1);
    float box1Bottom = box1.getBottom(position1, facingRight1);

    float box2Left = box2.getLeft(position2, facingRight2);
    float box2Right = box2.getRight(position2, facingRight2);
    float box2Top = box2.getTop(position2, facingRight2);
    float box2Bottom = box2.getBottom(position2, facingRight2);

    // Perform AABB collision check
    bool x_collision = box1Left < box2Right && box1Right > box2Left;
    bool y_collision = box1Top > box2Bottom && box1Bottom < box2Top;

    return x_collision && y_collision;
}


/*
1. check if one player attacks the other player, and 
2. apply damange if attack succeeds, i.e. hitbox hits hurtbox 
3. set state to STUNNED if being hit, set state timer. 
*/
void WorldSystem::handle_collisions() {
    Entity player1 = renderer->m_player1;
    Entity player2 = renderer->m_player2;

    // check if player 1 hit player 2
    if (checkHitBoxCollisions(player1, player2)) {
        applyDamage(player2, PLAYER_1_DAMAGE);  // apply damage 
        // state transition and state timer
        PlayerCurrentState& player2State = registry.playerCurrentStates.get(player2);
        StateTimer& player2StateTimer = registry.stateTimers.get(player2);
        player2State.currentState = PlayerState::STUNNED;
        player2StateTimer.reset(PLAYER_1_STUN_DURATION);
        // std::cout << "Player 2 is stunned for " << PLAYER_1_STUN_DURATION << "ms." << std::endl;
    }

    // check if player 2 hit player 1
    if (checkHitBoxCollisions(player2, player1)) {
        applyDamage(player1, PLAYER_2_DAMAGE);  // apply damage 
        // state transition and state timer
        PlayerCurrentState& player1State = registry.playerCurrentStates.get(player1);
        StateTimer& player1StateTimer = registry.stateTimers.get(player1);
        player1State.currentState = PlayerState::STUNNED;
        player1StateTimer.reset(PLAYER_2_STUN_DURATION);
        // std::cout << "Player 2 is stunned for " << PLAYER_1_STUN_DURATION << "ms." << std::endl;
    }
}

void WorldSystem::playerCollisions(GlRender* renderer) {
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);

    CollisionBox& player1CollisionBox = registry.collisionBoxes.get(renderer->m_player1);
    CollisionBox& player2CollisionBox = registry.collisionBoxes.get(renderer->m_player2);

    bool collision = checkAABBCollision(player1CollisionBox, player1Motion.position, player1Motion.direction,
        player2CollisionBox, player2Motion.position, player2Motion.direction);

    if (collision) {
        std::cout << "PLAYERS COLLIDED" << std::endl;

        player1Motion.position = player1Motion.lastPos;
        player2Motion.position = player2Motion.lastPos;

        // add boundary checks here for wall or edge of the screen handling
    }
}