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
	void hitBoxCollisions();
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
	void checkAABBCollision(bool& xCollision, bool& yCollision, const Box& box1, Motion& motion1, const Box& box2, Motion& motion2);
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