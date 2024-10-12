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
	Entity player1 = createPlayer1(renderer, { -0.3, -0.2 });
	Entity player2 = createPlayer2(renderer, { 0.3, -0.2 });

	renderer->m_player1 = player1;
	renderer->m_player2 = player2;
}

bool isKeyPressed(int key) {
    GLFWwindow* window = glfwGetCurrentContext();
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void WorldSystem::handleInput() {
    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);

    // Reset inputs
    player1Input = PlayerInput();
    player2Input = PlayerInput();

    // Player 1 Input
    if (isKeyPressed(GLFW_KEY_W)) player1Input.up = true;
    if (isKeyPressed(GLFW_KEY_S)) player1Input.down = true;
    if (isKeyPressed(GLFW_KEY_A)) player1Input.left = true;
    if (isKeyPressed(GLFW_KEY_D)) player1Input.right = true;
    if (isKeyPressed(GLFW_KEY_R)) player1Input.punch = true;
    if (isKeyPressed(GLFW_KEY_T)) player1Input.kick = true;

    // Player 2 Input
    if (isKeyPressed(GLFW_KEY_UP))player2Input.up = true;
    if (isKeyPressed(GLFW_KEY_DOWN)) player2Input.down = true;
    if (isKeyPressed(GLFW_KEY_LEFT)) player2Input.left = true;
    if (isKeyPressed(GLFW_KEY_RIGHT)) player2Input.right = true;
    if (isKeyPressed(GLFW_KEY_COMMA)) player2Input.punch = true;
    if (isKeyPressed(GLFW_KEY_PERIOD)) player2Input.kick = true;
}

void WorldSystem::updateMovement() {
    Motion& player1Motion = registry.motions.get(renderer->m_player1);
    Motion& player2Motion = registry.motions.get(renderer->m_player2);

    PlayerInput& player1Input = registry.playerInputs.get(renderer->m_player1);
    PlayerInput& player2Input = registry.playerInputs.get(renderer->m_player2);

    static bool player1IsJumping = false;
    static bool player2IsJumping = false;
    static float player1JumpStartY = 0.0f;
    static float player2JumpStartY = 0.0f;

    if (player1Input.left) {
        player1Motion.position.x -= MOVE_SPEED;
        std::cout << "Player 1 Position: " << player1Motion.position.x << ", " << player1Motion.position.y << std::endl;
    }
    if (player1Input.right) {
        player1Motion.position.x += MOVE_SPEED;
        std::cout << "Player 1 Position: " << player1Motion.position.x << ", " << player1Motion.position.y << std::endl;
    }
    
    // TODO: handle up, down, punch, kick

    if (player2Input.left) {
        player2Motion.position.x -= MOVE_SPEED;
        std::cout << "Player 2 Position: " << player2Motion.position.x << ", " << player2Motion.position.y << std::endl;
    }
    if (player2Input.right) {
        player2Motion.position.x += MOVE_SPEED;
        std::cout << "Player 2 Position: " << player2Motion.position.x << ", " << player2Motion.position.y << std::endl;
    }

    // TODO: handle up, down, punch, kick
}