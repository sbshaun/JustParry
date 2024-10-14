#include "world_system.hpp"
#include "world_init.hpp"

#include "../physics/physics_system.hpp"
#include "../constants.hpp"

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

    // TODO: uncomment 
    // intentionally commented out for testing 
    // if (health.currentHealth <= 0) {
    //     health.currentHealth = 0;
    // }
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
	Entity player1 = createPlayer1(renderer, { -0.3, -0.3 });
	Entity player2 = createPlayer2(renderer, { 0.3, -0.3 });

	renderer->m_player1 = player1;
	renderer->m_player2 = player2;

    //defined BOUND_SIZE in constants
    Entity boundaryRight = createBoundary(BOUND_SIZE_R, RIGHT);
    Entity boundaryLeft = createBoundary(BOUND_SIZE_L, LEFT);

    Entity boundaryFloor = createFloor(renderer);
}

//IN THE FUTURE WE SHOULD MAKE THE ENTITY LOOPING A SINGLE FUNCTION AND ALL THE PROCESSING PER LOOP HELPERS SO WE ONLY ITERATE THROUGH THE ENTITIES ONCE PER GAME CYCLE

bool isKeyPressed(int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void WorldSystem::handleInput() {
    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);

    PlayerCurrentState& player1State = registry.playerCurrentStates.get(renderer->m_player1);
    PlayerCurrentState& player2State = registry.playerCurrentStates.get(renderer->m_player2);

    // Reset inputs
    player1Input = PlayerInput();

    // not accepting input if player is in these states. 
    if (player1State.currentState == PlayerState::ATTACKING) return;
    if (player1State.currentState == PlayerState::PARRYING) return;
    if (player1State.currentState == PlayerState::PERFECT_PARRYING) return;
    if (player1State.currentState == PlayerState::COUNTER_ATTACKING) return;
    if (player1State.currentState == PlayerState::STUNNED) return;
    if (player1State.currentState == PlayerState::RECOVERING) return;

    if (player2State.currentState == PlayerState::ATTACKING) return;
    if (player2State.currentState == PlayerState::PARRYING) return;
    if (player2State.currentState == PlayerState::PERFECT_PARRYING) return;
    if (player2State.currentState == PlayerState::COUNTER_ATTACKING) return;
    if (player2State.currentState == PlayerState::STUNNED) return;
    if (player2State.currentState == PlayerState::RECOVERING) return;

    // Player 1 Input
    if (isKeyPressed(GLFW_KEY_W)) player1Input.up = true;
    if (isKeyPressed(GLFW_KEY_S)) player1Input.down = true;
    if (isKeyPressed(GLFW_KEY_A)) player1Input.left = true;
    if (isKeyPressed(GLFW_KEY_D)) player1Input.right = true;
    if (isKeyPressed(GLFW_KEY_R)) player1Input.punch = true;
    if (isKeyPressed(GLFW_KEY_T)) player1Input.kick = true;

    // Player 2 Input
    // disabled for now to handle random bot movements
    // player2Input = PlayerInput();
    if(!BOT_ENABLED){
        if (isKeyPressed(GLFW_KEY_UP))player2Input.up = true;
        if (isKeyPressed(GLFW_KEY_DOWN)) player2Input.down = true;
        if (isKeyPressed(GLFW_KEY_LEFT)) player2Input.left = true;
        if (isKeyPressed(GLFW_KEY_RIGHT)) player2Input.right = true;
        if (isKeyPressed(GLFW_KEY_COMMA)) player2Input.punch = true;
        if (isKeyPressed(GLFW_KEY_PERIOD)) player2Input.kick = true;   
    }

}


void WorldSystem::inputProcessing(int timer) { //renamed as it will proccess the input -> ingame logic, not just movement
    // TODO: manage input based on PlayerState more effectively 
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);

    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);

    PlayerCurrentState& player1State = registry.playerCurrentStates.get(renderer->m_player1);
    PlayerCurrentState& player2State = registry.playerCurrentStates.get(renderer->m_player2);

    static bool player1IsJumping = false;
    static bool player2IsJumping = false;
    static float player1JumpStartY = 0.0f;
    static float player2JumpStartY = 0.0f;

    player1Motion.lastPos = player1Motion.position;
    player2Motion.lastPos = player2Motion.position;

    // std::cout << "Last Position" << player1Motion.lastPos.x << std::endl;
    // std::cout << "Last Position" << player1Motion.lastPos.x << std::endl;

    if (player1State.currentState != PlayerState::ATTACKING
        && player1State.currentState != PlayerState::STUNNED
        && player1State.currentState != PlayerState::RECOVERING
        && player1State.currentState != PlayerState::PARRYING
        && player1State.currentState != PlayerState::PERFECT_PARRYING
        && player1State.currentState != PlayerState::COUNTER_ATTACKING 
        ) 
    { 
        // player can only move if not in these non-moveable states. 
        if (player1Input.left) {
            player1Motion.velocity.x = -MOVE_SPEED;
            // std::cout << "Player 1 Position: " << player1Motion.position.x << ", " << player1Motion.position.y << std::endl;
        }
        if (player1Input.right) {
            player1Motion.velocity.x = MOVE_SPEED;
            // std::cout << "Player 1 Position: " << player1Motion.position.x << ", " << player1Motion.position.y << std::endl;
        }

        // Handle jump
        if (player1Input.up && !player1Motion.inAir) {
            std::cout << "Player pressed UP! Starting jump." << std::endl;
            player1Motion.inAir = true;
            player1Motion.velocity.y = 3 * MOVE_SPEED; // Jump upwards
            player1JumpStartY = player1Motion.position.y; // Save starting position
        }

        if(player1Input.right && player1Input.left){ //SOCD CLEANING
            player1Motion.velocity.x = 0;
        }
        if (player1Motion.velocity.x != 0 && !player1IsJumping){ //check if left or right is still pressed and if not sets velocity back to 0 --- I wonder if we should have acceleration system? --probably overcomplicates it...
            if(player1Motion.velocity.x > 0 && !player1Input.right){
                player1Motion.velocity.x = 0;
            }
            if(player1Motion.velocity.x < 0 && !player1Input.left){
                player1Motion.velocity.x = 0;
            }
        }
    }

    if (player2State.currentState != PlayerState::ATTACKING
        && player2State.currentState != PlayerState::STUNNED
        && player2State.currentState != PlayerState::RECOVERING
        && player2State.currentState != PlayerState::PARRYING
        && player2State.currentState != PlayerState::PERFECT_PARRYING
        && player2State.currentState != PlayerState::COUNTER_ATTACKING
        ) 
    {
        // TODO: handle up, down, punch, kick
        if (player2Input.left) {
            player2Motion.velocity.x = -MOVE_SPEED;
            // std::cout << "Player 2 Position: " << player2Motion.position.x << ", " << player2Motion.position.y << std::endl;
        }
        if (player2Input.right) {
            player2Motion.velocity.x = MOVE_SPEED;
            // std::cout << "Player 2 Position: " << player2Motion.position.x << ", " << player2Motion.position.y << std::endl;
        }
        if (player2Input.up && !player2Motion.inAir) {
            std::cout << "Player pressed UP! Starting jump." << std::endl;
            player2Motion.inAir = true;
            player2Motion.velocity.y = 3 * MOVE_SPEED; // Jump upwards
            player2JumpStartY = player2Motion.position.y; // Save starting position
        }

        //SOCD CLEANING
        if(player2Input.right && player2Input.left) { 
            player2Motion.velocity.x = 0;
        }
        //check if left or right is still pressed and if not sets velocity back to 0 --- I wonder if we should have acceleration system? --probably overcomplicates it...
        if (player2Motion.velocity.x != 0 && !player2IsJumping) {
            if(player2Motion.velocity.x > 0 && !player2Input.right){
                player2Motion.velocity.x = 0;
            }
            if(player2Motion.velocity.x < 0 && !player2Input.left){
                player2Motion.velocity.x = 0;
            }
        }
    }

    // TODO: handle up, down, punch, kick

    // TODO: DRY this up. 
    // Hitbox activation
    StateTimer& player1StateTimer = registry.stateTimers.get(renderer->m_player1);
    StateTimer& player2StateTimer = registry.stateTimers.get(renderer->m_player2);
    HitBox& player1HitBox = registry.hitBoxes.get(renderer->m_player1);
    HitBox& player2HitBox = registry.hitBoxes.get(renderer->m_player2);

    /*
    if punch is pressed, 
    1. check if the current state allows to punch 
    2. if yes, activate the hitbox and set the state to attacking 
    3. set the StateTimer, which will be used to deactivate the hitbox at a loop somewhere else. 
    Currently it only considers punch, and IDLE/WALKING states. 
    TODO: extend this to also include kick, jump kick, etc... 
    TODO: extend this to consider other statess. 
    */
    if (player1Input.punch) {
        std::cout << "Player 1 pressed punch, current state: " << PlayerStateToString(player1State.currentState) << std::endl;
        switch (player1State.currentState) {
            case PlayerState::IDLE:
            case PlayerState::WALKING:
                player1HitBox.active = true;
                std::cout << "Player 1 Hitbox Actived" << std::endl;
                player1HitBox.xOffset = PLAYER_1_PUNCH_X_OFFSET;
                player1HitBox.yOffset = PLAYER_1_PUNCH_Y_OFFSET;
                player1HitBox.width = PLAYER_1_PUNCH_WIDTH;
                player1HitBox.height = PLAYER_1_PUNCH_HEIGHT;
                player1State.currentState = PlayerState::ATTACKING;
                player1StateTimer.reset(PLAYER_1_HITBOX_DURATION);
                break;
            case PlayerState::JUMPING:
                // TODO 
                break;
            case PlayerState::CROUCHING:
                // TODO 
                break;
            case PlayerState::ATTACKING:
                // TODO 
                break;
            case PlayerState::PARRYING:
                // TODO 
                break;
            case PlayerState::PERFECT_PARRYING:
                // TODO 
                break;
            case PlayerState::COUNTER_ATTACKING:
                // TODO 
                break;
            case PlayerState::STUNNED:
                // TODO 
                break;
            case PlayerState::RECOVERING:
                // TODO 
                break;
        }
    }

    if (player2Input.punch) {
        std::cout << "Player 2 pressed punch, current state: " << PlayerStateToString(player2State.currentState) << std::endl;
        switch (player2State.currentState) {
            case PlayerState::IDLE:
            case PlayerState::WALKING:
                std::cout << "Player 2 Hitbox Actived" << std::endl;
                player2HitBox.active = true;
                player2HitBox.xOffset = PLAYER_2_PUNCH_X_OFFSET;
                player2HitBox.yOffset = PLAYER_2_PUNCH_Y_OFFSET;
                player2HitBox.width = PLAYER_2_PUNCH_WIDTH;
                player2HitBox.height = PLAYER_2_PUNCH_HEIGHT;
                player2State.currentState = PlayerState::ATTACKING;
                player2StateTimer.reset(PLAYER_1_HITBOX_DURATION);
                break;
            case PlayerState::JUMPING:
                // TODO 
                break;
            case PlayerState::CROUCHING:
                // TODO 
                break;
            case PlayerState::ATTACKING:
                // TODO 
                break;
            case PlayerState::PARRYING:
                // TODO 
                break;
            case PlayerState::PERFECT_PARRYING:
                // TODO 
                break;
            case PlayerState::COUNTER_ATTACKING:
                // TODO 
                break;
            case PlayerState::STUNNED:
                // TODO 
                break;
            case PlayerState::RECOVERING:
                // TODO 
                break;
        }
    }
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
        std::cout << "Player 1 cannot move, current state: " << PlayerStateToString(player1State.currentState) << std::endl;
    }

    if (canMove(player2State.currentState)) {
            // player can only move if not in these non-moveable states. 
            player2Motion.position += player2Motion.velocity;
    } else {
        player2Motion.position.y += player2Motion.velocity.y;
        std::cout << "Player 2 cannot move, current state: " << PlayerStateToString(player2State.currentState) << std::endl;
    }
}

void WorldSystem::updateStateTimers(float elapsed_ms) {
    StateTimer& player1StateTimer = registry.stateTimers.get(renderer->m_player1);
    StateTimer& player2StateTimer = registry.stateTimers.get(renderer->m_player2);

    HitBox& player1HitBox = registry.hitBoxes.get(renderer->m_player1);
    HitBox& player2HitBox = registry.hitBoxes.get(renderer->m_player2);

    // if the timer is active, update it with the step size 
    if (player1StateTimer.isAlive()) {
        player1StateTimer.update(elapsed_ms);
    } else {
        // deactivate hitbox. 
        if (player1HitBox.active) {
            player1HitBox.active = false;
            player1HitBox.hit = false;
            std::cout << "Player 1 HitBox deactivated." << std::endl;
        }
        // transition state 
        PlayerCurrentState& player1State = registry.playerCurrentStates.get(renderer->m_player1);
        if (player1State.currentState != PlayerState::IDLE) {
            std::cout << "Player 1 state: \"" << PlayerStateToString(player1State.currentState) << "\" expired." << std::endl;
            switch (player1State.currentState) {
                case PlayerState::ATTACKING:
                case PlayerState::PARRYING:
                case PlayerState::PERFECT_PARRYING:
                case PlayerState::COUNTER_ATTACKING:
                case PlayerState::STUNNED:
                case PlayerState::RECOVERING:
                    player1State.currentState = PlayerState::IDLE;
                    std::cout << "Player 1 state transition to: \"" << PlayerStateToString(player1State.currentState) << "\"." << std::endl;
                    break;
                default:
                    std::cout << "Player 1 state: \"" << PlayerStateToString(player1State.currentState) << "\" expired, no transition available." << std::endl;
                    break;
            }
        }
    }

    if (player2StateTimer.isAlive()) {
        player2StateTimer.update(PLAYER_STATE_TIMER_STEP);
    } else {
        // deactivate hitbox.
        if (player2HitBox.active) {
            player2HitBox.active = false;
            player2HitBox.hit = false;
            std::cout << "Player 2 HitBox deactivated." << std::endl;
        }
        // transition state 
        PlayerCurrentState& player2State = registry.playerCurrentStates.get(renderer->m_player2);
        
        if (player2State.currentState != PlayerState::IDLE) {
            std::cout << "Player 2 state: \"" << PlayerStateToString(player2State.currentState) << "\" expired." << std::endl;
            switch (player2State.currentState) {
                case PlayerState::ATTACKING:
                case PlayerState::PARRYING:
                case PlayerState::PERFECT_PARRYING:
                case PlayerState::COUNTER_ATTACKING:
                case PlayerState::STUNNED:
                case PlayerState::RECOVERING:
                    player2State.currentState = PlayerState::IDLE;
                    std::cout << "Player 2 state transition to: \"" << PlayerStateToString(player2State.currentState) << "\"." << std::endl;
                    break;
                default:
                    std::cout << "Player 2 state: \"" << PlayerStateToString(player2State.currentState) << "\" expired, no transition available." << std::endl;
                    break;
            }
        }
    }
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
    std::cout << "player's postion: " << hitPlayerMotion.position.x << ", " << hitPlayerMotion.position.y << std::endl;
    std::cout << "Hitbox 4 corners, left, right, top, bottom: " << hitBoxLeft << ", " << hitBoxRight << ", " << hitBoxTop << ", " << hitBoxBottom << std::endl;

    float hurtBoxLeft = hurtBox.getLeft(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxRight = hurtBox.getRight(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxTop = hurtBox.getTop(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    float hurtBoxBottom = hurtBox.getBottom(hurtPlayerMotion.position, hurtPlayerMotion.direction);
    std::cout << "player's postion: " << hurtPlayerMotion.position.x << ", " << hurtPlayerMotion.position.y << std::endl;
    std::cout << "Hurtbox 4 corners, left, right, top, bottom: " << hurtBoxLeft << ", " << hurtBoxRight << ", " << hurtBoxTop << ", " << hurtBoxBottom << std::endl;

    std::cout << "collision: " << collision << std::endl;

    // AABB collision check
    bool x_collision = hitBoxLeft < hurtBoxRight && hitBoxRight > hurtBoxLeft;
    bool y_collision = hitBoxTop > hurtBoxBottom && hitBoxBottom < hurtBoxTop;
    
    if (x_collision && y_collision) {
        hitBox.hit = true;
        std::cout << "Hitbox of Player " << (unsigned int) playerWithHitBox << " collided with Hurtbox of Player" << (unsigned int) playerWithHurtBox << std::endl;
        collision = true;
    }
    
    return collision;
};

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
        std::cout << "Player 2 is stunned for " << PLAYER_1_STUN_DURATION << "ms." << std::endl;
    }

    // check if player 2 hit player 1
    if (checkHitBoxCollisions(player2, player1)) {
        applyDamage(player1, PLAYER_2_DAMAGE);  // apply damage 
        // state transition and state timer
        PlayerCurrentState& player1State = registry.playerCurrentStates.get(player1);
        StateTimer& player1StateTimer = registry.stateTimers.get(player1);
        player1State.currentState = PlayerState::STUNNED;
        player1StateTimer.reset(PLAYER_2_STUN_DURATION);
        std::cout << "Player 2 is stunned for " << PLAYER_1_STUN_DURATION << "ms." << std::endl;
    }
}

void WorldSystem::playerCollisions(GlRender* renderer){
	Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);
	if(abs(player1Motion.position.x - player2Motion.position.x) < NDC_WIDTH){
		std::cout << "PLAYERS COLLIDED"  << std::endl;
	}
}