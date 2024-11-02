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
static bool canMove(PlayerState state)
{
    return state != PlayerState::ATTACKING && state != PlayerState::STUNNED && state != PlayerState::RECOVERING && state != PlayerState::PARRYING && state != PlayerState::PERFECT_PARRYING && state != PlayerState::COUNTER_ATTACKING;
}

/*
helper funciton to apply damage to a player
*/
static void applyDamage(Entity player, float damage)
{
    Health &health = registry.healths.get(player);

    health.currentHealth -= damage;

    if (health.currentHealth <= 0)
    {
        health.currentHealth = 0;
    }
}

WorldSystem::WorldSystem() {};

WorldSystem::~WorldSystem()
{
    // Clean up all shaders first
    if (renderer)
    {
        cleanupShaders(renderer->m_player1);
        cleanupShaders(renderer->m_player2);
    }

    // Clear all components
    registry.clear_all_components();
}

void WorldSystem::init(GlRender *renderer)
{
    this->renderer = renderer;

    // Create entities
    FighterConfig birdmanConfig = FighterManager::getFighterConfig(Fighters::BIRDMAN);
    Entity player1 = createPlayer1(renderer, {-1.25, FLOOR_Y + birdmanConfig.NDC_HEIGHT}, Fighters::BIRDMAN);
    Entity player2 = createPlayer2(renderer, {1.25, FLOOR_Y + birdmanConfig.NDC_HEIGHT}, Fighters::BIRDMAN);

    renderer->m_player1 = player1;
    renderer->m_player2 = player2;

    // defined BOUND_SIZE in constants
    Entity boundaryRight = createBoundary(BOUND_SIZE_R, RIGHT);
    Entity boundaryLeft = createBoundary(BOUND_SIZE_L, LEFT);
    Entity boundaryFloor = createFloor(FLOOR_Y, FLOOR);

    // 1. init input handlers and state machines
    initInputHandlers();
    initStateMachines();
}

void WorldSystem::initInputHandlers()
{
    // Player 1 controls
    std::unique_ptr<InputMapping> player1InputMapping = std::make_unique<InputMapping>();
    player1InputMapping->bindKeyToAction(GLFW_KEY_A, Action::MOVE_LEFT);
    player1InputMapping->bindKeyToAction(GLFW_KEY_D, Action::MOVE_RIGHT);
    player1InputMapping->bindKeyToAction(GLFW_KEY_W, Action::JUMP);
    player1InputMapping->bindKeyToAction(GLFW_KEY_S, Action::CROUCH);
    player1InputMapping->bindKeyToAction(GLFW_KEY_R, Action::PUNCH);
    player1InputMapping->bindKeyToAction(GLFW_KEY_T, Action::KICK);

    // Player 2 controls
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

void WorldSystem::initStateMachines()
{
    player1StateMachine = std::make_unique<StateMachine>();
    player2StateMachine = std::make_unique<StateMachine>();

    player1StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player1StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    player1StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player1StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player1StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player1StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    player1StateMachine->addState(PlayerState::STUNNED, std::make_unique<StunnedState>());

    player2StateMachine->addState(PlayerState::IDLE, std::make_unique<IdleState>());
    player2StateMachine->addState(PlayerState::WALKING, std::make_unique<WalkingState>());
    player2StateMachine->addState(PlayerState::JUMPING, std::make_unique<JumpingState>());
    player2StateMachine->addState(PlayerState::ATTACKING, std::make_unique<AttackingState>());
    player2StateMachine->addState(PlayerState::CROUCHING, std::make_unique<CrouchingState>());
    player2StateMachine->addState(PlayerState::PARRYING, std::make_unique<ParryingState>());
    player2StateMachine->addState(PlayerState::STUNNED, std::make_unique<StunnedState>());
}

// IN THE FUTURE WE SHOULD MAKE THE ENTITY LOOPING A SINGLE FUNCTION AND ALL THE PROCESSING PER LOOP HELPERS SO WE ONLY ITERATE THROUGH THE ENTITIES ONCE PER GAME CYCLE

void WorldSystem::handlePlayerInput(Entity player, InputHandler &inputHandler, StateMachine &stateMachine)
{
    auto &motion = registry.motions.get(player);
    auto &input = registry.playerInputs.get(player);

    // Reset inputs
    input = PlayerInput();

    input.left = isKeyPressed(inputHandler.getKeyFromAction(Action::MOVE_LEFT));
    input.right = isKeyPressed(inputHandler.getKeyFromAction(Action::MOVE_RIGHT));
    input.up = isKeyPressed(inputHandler.getKeyFromAction(Action::JUMP));
    input.down = isKeyPressed(inputHandler.getKeyFromAction(Action::CROUCH));
    input.punch = isKeyPressed(inputHandler.getKeyFromAction(Action::PUNCH));
    input.kick = isKeyPressed(inputHandler.getKeyFromAction(Action::KICK));
}

void WorldSystem::handleInput()
{
    // Player 1's input is always handled
    player1InputHandler->handleInput(renderer->m_player1, *player1StateMachine);

    // For Player 2, either handle manual input or bot input
    if (!botEnabled)
    {
        player2InputHandler->handleInput(renderer->m_player2, *player2StateMachine);
    }
    else
    {
        // Process bot's inputs through the state machine
        PlayerInput &p2Input = registry.playerInputs.get(renderer->m_player2);

        // Convert bot inputs to state machine transitions
        if (p2Input.left || p2Input.right)
            player2StateMachine->transition(renderer->m_player2, PlayerState::WALKING);
        if (p2Input.up)
            player2StateMachine->transition(renderer->m_player2, PlayerState::JUMPING);
        if (p2Input.punch || p2Input.kick)
            player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);

        // Update motion based on inputs
        Motion &p2Motion = registry.motions.get(renderer->m_player2);
        if (p2Input.left)
            p2Motion.velocity.x = -MOVE_SPEED;
        else if (p2Input.right)
            p2Motion.velocity.x = MOVE_SPEED;
        else
            p2Motion.velocity.x = 0;
    }

    // Always update state machines for both players
    player1StateMachine->update(renderer->m_player1, TIME_STEP);
    player2StateMachine->update(renderer->m_player2, TIME_STEP);
}

void WorldSystem::inputProcessing(int timer)
{
    Motion &player1Motion = registry.motions.get(renderer->m_player1);
    Motion &player2Motion = registry.motions.get(renderer->m_player2);
    player1Motion.lastPos = player1Motion.position;
    player2Motion.lastPos = player2Motion.position;

    PlayerInput &player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput &player2Input = registry.playerInputs.get(renderer->m_player2);

    // Process player 1's input
    if (player1Input.punch)
        player1StateMachine->transition(renderer->m_player1, PlayerState::ATTACKING);

    // Only process player 2's manual input if bot is disabled
    if (!botEnabled)
    {
        if (player2Input.punch)
            player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);
        if (player2Input.kick)
            player2StateMachine->transition(renderer->m_player2, PlayerState::ATTACKING);
    }
}

void WorldSystem::movementProcessing()
{
    Motion &player1Motion = registry.motions.get(renderer->m_player1);
    Motion &player2Motion = registry.motions.get(renderer->m_player2);

    PlayerCurrentState &player1State = registry.playerCurrentStates.get(renderer->m_player1);
    PlayerCurrentState &player2State = registry.playerCurrentStates.get(renderer->m_player2);

    if (canMove(player1State.currentState))
    {
        // player can only move if not in these non-moveable states.
        player1Motion.position += player1Motion.velocity;
    }
    else
    {
        player1Motion.position.y += player1Motion.velocity.y;
        // std::cout << "Player 1 cannot move, current state: " << PlayerStateToString(player1State.currentState) << std::endl;
    }

    if (canMove(player2State.currentState))
    {
        // player can only move if not in these non-moveable states.
        player2Motion.position += player2Motion.velocity;
    }
    else
    {
        player2Motion.position.y += player2Motion.velocity.y;
        // std::cout << "Player 2 cannot move, current state: " << PlayerStateToString(player2State.currentState) << std::endl;
    }

    if (player1Motion.position.x > player2Motion.position.x)
    {
        player1Motion.direction = false; // Player 1 now facing left
        player2Motion.direction = true;  // Player 2 now facing right
    }
    else if (player1Motion.position.x < player2Motion.position.x)
    {
        player1Motion.direction = true;  // Player 1 now facing right
        player2Motion.direction = false; // Player 2 now facing left
    }
}

void WorldSystem::updateStateTimers(float elapsed_ms)
{
    player1StateMachine->update(renderer->m_player1, elapsed_ms);
    player2StateMachine->update(renderer->m_player2, elapsed_ms);
    StateTimer &player1StateTimer = registry.stateTimers.get(renderer->m_player1);
    StateTimer &player2StateTimer = registry.stateTimers.get(renderer->m_player2);

    HitBox &player1HitBox = registry.hitBoxes.get(renderer->m_player1);
    HitBox &player2HitBox = registry.hitBoxes.get(renderer->m_player2);
}

bool WorldSystem::checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox)
{
    Motion &hitPlayerMotion = registry.motions.get(playerWithHitBox);
    Motion &hurtPlayerMotion = registry.motions.get(playerWithHurtBox);
    HitBox &hitBox = registry.hitBoxes.get(playerWithHitBox);
    HurtBox &hurtBox = registry.hurtBoxes.get(playerWithHurtBox);

    // if hitbox is not active or has already hit, skip check
    if (!hitBox.active || hitBox.hit)
        return false;

    float hitBoxLeft = hitBox.getLeft(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxRight = hitBox.getRight(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxTop = hitBox.getTop(hitPlayerMotion.position, hitPlayerMotion.direction);
    float hitBoxBottom = hitBox.getBottom(hitPlayerMotion.position, hitPlayerMotion.direction);

    float hurtBoxLeft = hurtBox.getLeft(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxRight = hurtBox.getRight(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxTop = hurtBox.getTop(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxBottom = hurtBox.getBottom(hurtPlayerMotion.position, hurtPlayerMotion.direction);

    bool x_collision = hitBoxLeft < hurtBoxRight && hitBoxRight > hurtBoxLeft;
    bool y_collision = hitBoxTop > hurtBoxBottom && hitBoxBottom < hurtBoxTop;

    if (x_collision && y_collision)
    {
        Player &attacker = registry.players.get(playerWithHitBox);
        Player &defender = registry.players.get(playerWithHurtBox);
        std::cout << "Player " << attacker.id << " hits Player " << defender.id << std::endl;
        hitBox.hit = true;
        return true;
    }

    return false;
}

// helper function for AABB Collision
void WorldSystem::checkAABBCollision(bool& xCollision, bool& yCollision,
    const Box& box1, Motion& motion1,
    const Box& box2, Motion& motion2)
{
    vec2 position1 = motion1.position;
    bool facingRight1 = motion1.direction;
    vec2 position2 = motion2.position;
    bool facingRight2 = motion2.direction;

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

    xCollision = x_collision;
    yCollision = y_collision;
    
    // keep track of whether the collision happened from above or the side
    motion1.wasAbove = motion1.above;
    motion2.wasAbove = motion2.above;

    if (x_collision && box1Bottom > box2Top) {
        motion1.above = true;
        motion2.above = false;
    }
    else if (x_collision && box2Bottom > box1Top) {
        motion2.above = true;
        motion1.above = false;
    }
}

/*
1. check if one player attacks the other player, and
2. apply damange if attack succeeds, i.e. hitbox hits hurtbox
3. set state to STUNNED if being hit, set state timer.
*/
void WorldSystem::hitBoxCollisions()
{
    Entity player1 = renderer->m_player1;
    Entity player2 = renderer->m_player2;
    Fighters current_char1 = registry.players.get(player1).current_char;
    Fighters current_char2 = registry.players.get(player2).current_char;

    // check if player 1 hit player 2
    if (checkHitBoxCollisions(player1, player2))
    {
        applyDamage(player2, FighterManager::getFighterConfig(current_char1).PUNCH_DAMAGE);
        PlayerCurrentState &player2State = registry.playerCurrentStates.get(player2);
        StateTimer &player2StateTimer = registry.stateTimers.get(player2);
        player2State.currentState = PlayerState::STUNNED;
        player2StateTimer.reset(FighterManager::getFighterConfig(current_char1).PUNCH_STUN_DURATION);
    }

    // check if player 2 hit player 1
    if (checkHitBoxCollisions(player2, player1))
    {
        applyDamage(player1, FighterManager::getFighterConfig(current_char2).PUNCH_DAMAGE);
        PlayerCurrentState &player1State = registry.playerCurrentStates.get(player1);
        StateTimer &player1StateTimer = registry.stateTimers.get(player1);
        player1State.currentState = PlayerState::STUNNED;
        player1StateTimer.reset(FighterManager::getFighterConfig(current_char2).PUNCH_STUN_DURATION);
    }
}

void WorldSystem::playerCollisions(GlRender *renderer)
{
    Motion &player1Motion = registry.motions.get(renderer->m_player1);
    Motion &player2Motion = registry.motions.get(renderer->m_player2);

    CollisionBox &player1CollisionBox = registry.collisionBoxes.get(renderer->m_player1);
    CollisionBox &player2CollisionBox = registry.collisionBoxes.get(renderer->m_player2);

    bool xCollision;
    bool yCollision;

    checkAABBCollision(xCollision, yCollision,
                       player1CollisionBox, player1Motion,
                       player2CollisionBox, player2Motion);

    if (xCollision && yCollision)
    {
        // Neither player is on top of another
        if (!player1Motion.wasAbove && !player2Motion.wasAbove) {
            player1Motion.position.x = player1Motion.lastPos.x;
            player2Motion.position.x = player2Motion.lastPos.x;
            player1Motion.velocity.x = 0;
            player2Motion.velocity.x = 0;
        }
        // Player one is on top
        else if (player1Motion.wasAbove) {
            player1Motion.velocity.x = 0;
            player2Motion.velocity.y = 0;
            if (player1Motion.direction) {
                player1Motion.position = { player1Motion.lastPos.x - 0.05, player1Motion.lastPos.y };
            }
            else {
                player1Motion.position = { player1Motion.lastPos.x + 0.05, player1Motion.lastPos.y };
            }
        }
        // Player two is on top
        else if (player2Motion.wasAbove) {
            player2Motion.velocity.x = 0;
            player1Motion.velocity.y = 0;
            if (player2Motion.direction) {
                player2Motion.position = { player2Motion.lastPos.x - 0.05, player2Motion.lastPos.y };
            }
            else {
                player2Motion.position = { player2Motion.lastPos.x + 0.05, player2Motion.lastPos.y };
            }
        }
    }
}