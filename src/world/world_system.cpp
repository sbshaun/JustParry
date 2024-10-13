#include "world_system.hpp"
#include "world_init.hpp"

#include "../physics/physics_system.hpp"
#include "../constants.hpp"

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

    // Reset inputs
    player1Input = PlayerInput();

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
    /*if (isKeyPressed(GLFW_KEY_UP))player2Input.up = true;
    if (isKeyPressed(GLFW_KEY_DOWN)) player2Input.down = true;
    if (isKeyPressed(GLFW_KEY_LEFT)) player2Input.left = true;
    if (isKeyPressed(GLFW_KEY_RIGHT)) player2Input.right = true;
    if (isKeyPressed(GLFW_KEY_COMMA)) player2Input.punch = true;
    if (isKeyPressed(GLFW_KEY_PERIOD)) player2Input.kick = true;
    */
}


void WorldSystem::inputProcessing(int timer) { //renamed as it will proccess the input -> ingame logic, not just movement
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);

    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);

    static bool player1IsJumping = false;
    static bool player2IsJumping = false;
    static float player1JumpStartY = 0.0f;
    static float player2JumpStartY = 0.0f;

    player1Motion.lastPos = player1Motion.position;
    player2Motion.lastPos = player2Motion.position;
    
    std::cout << "Last Position" << player1Motion.lastPos.x << std::endl;

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

    if(player2Input.right && player2Input.left){ //SOCD CLEANING
        player2Motion.velocity.x = 0;
    }
    if (player2Motion.velocity.x != 0 && !player2IsJumping){ //check if left or right is still pressed and if not sets velocity back to 0 --- I wonder if we should have acceleration system? --probably overcomplicates it...
        if(player2Motion.velocity.x > 0 && !player2Input.right){
            player2Motion.velocity.x = 0;
        }
        if(player2Motion.velocity.x < 0 && !player2Input.left){
            player2Motion.velocity.x = 0;
        }
    }

    // TODO: handle up, down, punch, kick
}

void WorldSystem::movementProcessing() {
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);
    player1Motion.position += player1Motion.velocity;
    player2Motion.position += player2Motion.velocity;    
}

