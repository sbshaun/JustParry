#pragma once
#include "../common.hpp"
#include "../renderer.hpp"
#include "../input_system/input_handler.hpp"
#include "../input_system/state_machine.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Forward declarations
class GlRender;
class InputHandler;
class StateMachine;

class WorldSystem
{
public:
	std::queue<Action> inputQueue;
	WorldSystem();
	void init(GlRender *renderer);
	void hitBoxCollisions();
	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	~WorldSystem();

	// TODO: Handle the movement and collision
	void handleInput();
	void inputProcessing();
	void movementProcessing();
	void handlePlayerInput(Entity player, InputHandler &inputHandler, StateMachine &stateMachine);
	void updateStateTimers(float elapsed_ms);
	bool checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox);
	bool checkHitBoxMeshCollision(float hitBoxLeft, float hitBoxRight, float hitBoxTop,
								  float hitBoxBottom, ObjectMesh *mesh, Motion &hurtMotion);
	bool checkParryBoxCollisions(Entity playerWithHitBox, Entity playerWithParryBox);
	void checkAABBCollision(bool &xCollision, bool &yCollision, const Box &box1, Motion &motion1, const Box &box2, Motion &motion2);
	void playerCollisions(GlRender *renderer);
	static void playPunchSound() { Mix_PlayChannel(-1, punch_sound, 0); }

	static void playBackgroundMusic() { 
		// Playing background music indefinitely
		Mix_PlayMusic(background_music, -1);
		fprintf(stderr, "Background music played\n");
		Mix_VolumeMusic(MIX_MAX_VOLUME / 4);  // set volume to 1/4 
	}

	static void stopBackgroundMusic() { 
		Mix_HaltMusic(); 
		fprintf(stderr, "Background music stopped\n");
	}
	

	void updatePlayableArea();
	// bool step(float elapsed_ms);
	bool botEnabled = false;
    static bool isPlayerWalking; 
    static const int WALK_SOUND_CHANNEL = 5; 
    static constexpr float WALK_SOUND_TIMEOUT = 50.f;  // time to wait before stopping walk_sound 
    static float walkStopTimer;  // Timer of how long the player has stopped walking 

private:
	GlRender *renderer;

	std::unique_ptr<InputHandler> player1InputHandler;
	std::unique_ptr<InputHandler> player2InputHandler;
	std::unique_ptr<StateMachine> player1StateMachine;
	std::unique_ptr<StateMachine> player2StateMachine;

	Motion *player1Motion;
	Motion* player2Motion;

	CollisionBox* player1CollisionBox;
	CollisionBox *player2CollisionBox;

	void initInputHandlers();
	void initStateMachines();
	void updatePlayerState(float elapsed_ms);
	static Mix_Music* background_music; 
	static Mix_Chunk* punch_sound; 
	static Mix_Chunk* walk_sound;
};