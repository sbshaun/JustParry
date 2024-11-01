#pragma once
#include "../common.hpp"
#include "../renderer.hpp"
#include "../input_system/input_handler.hpp"
#include "../input_system/state_machine.hpp"

// Forward declarations
class GlRender;
class InputHandler;
class StateMachine;

class WorldSystem
{
public:
	WorldSystem();
	void init(GlRender *renderer);
	void handle_collisions();
	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	~WorldSystem();

	// TODO: Handle the movement and collision
	void handleInput();
	void inputProcessing(int timer);
	void movementProcessing();
	void handlePlayerInput(Entity player, InputHandler &inputHandler, StateMachine &stateMachine);
	void updateStateTimers(float elapsed_ms);
	bool checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox);
	void checkAABBCollision(bool& xCollision, bool& yCollision, int& playerOneOnTop, const Box& box1, const vec2& position1, bool facingRight1, const Box& box2, const vec2& position2, bool facingRight2);
	void playerCollisions(GlRender *renderer);
	// bool step(float elapsed_ms);
	bool botEnabled = false;

private:
	GlRender *renderer;

	std::unique_ptr<InputHandler> player1InputHandler;
	std::unique_ptr<InputHandler> player2InputHandler;
	std::unique_ptr<StateMachine> player1StateMachine;
	std::unique_ptr<StateMachine> player2StateMachine;
	void initInputHandlers();
	void initStateMachines();
	void updatePlayerState(float elapsed_ms);
};