#pragma once
#include "../common.hpp"
#include "../renderer.hpp"
#include "../input_system/input_handler.hpp"
#include "../input_system/state_machine.hpp"
#include "../settings.hpp"
#include "../default_config.hpp"
#include "../particle_system/particle_system.hpp"
#include "../particle_system/blood_system.hpp"
#include "../particle_system/smoke_system.hpp"

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
	static void playPunchSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			Mix_PlayChannel(-1, punch_sound, 0);
		}
	}

	static void playBackgroundMusic()
	{
		// Playing background music indefinitely
		Mix_PlayMusic(background_music, -1);
		fprintf(stderr, "Background music played\n");
		Mix_VolumeMusic(MIX_MAX_VOLUME / 4); // set volume to 1/4
	}

	static void stopBackgroundMusic()
	{
		Mix_HaltMusic();
		fprintf(stderr, "Background music stopped\n");
	}

	static void stopAllSounds()
	{
		Mix_HaltChannel(-1);
	}

	static void resumeSounds()
	{
		// Only resume sounds if sound effects are enabled
		if (Settings::audioSettings.enable_sound_effects)
		{
			// Resume any previously playing sounds
			Mix_Resume(-1); // Resume all channels
		}
	}

	void updatePlayableArea();
	// bool step(float elapsed_ms);
	bool botEnabled = false;
	void initInputHandlers();
	static bool isPlayerWalking;
	static const int WALK_SOUND_CHANNEL = 5;
	static constexpr float WALK_SOUND_TIMEOUT = 50.f; // time to wait before stopping walk_sound
	static float walkStopTimer;						  // Timer of how long the player has stopped walking

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms);
	void emitBloodParticles(float x, float y, float z, bool direction);
	void emitSmokeParticles(float x, float y, float z);
	void renderParticles();

	BloodParticleSystem bloodSystem;
	SmokeParticleSystem smokeSystem;

	static void updateVolume()
	{
		// Update music volume
		if (Settings::audioSettings.enable_music)
		{
			Mix_VolumeMusic(MIX_MAX_VOLUME * Settings::audioSettings.music_volume * Settings::audioSettings.overall_volume);
		}
		else
		{
			Mix_VolumeMusic(0);
		}

		// Update sound effects volume
		if (Settings::audioSettings.enable_sound_effects)
		{
			Mix_Volume(-1, MIX_MAX_VOLUME * Settings::audioSettings.overall_volume);
		}
		else
		{
			Mix_Volume(-1, 0);
		}
	}

	static void updateAudioState()
	{
		if (!Settings::audioSettings.enable_music)
		{
			stopBackgroundMusic();
		}
		else if (Mix_PlayingMusic() == 0)
		{
			playBackgroundMusic();
		}
	}

private:
	GlRender *renderer;

	std::unique_ptr<InputHandler> player1InputHandler;
	std::unique_ptr<InputHandler> player2InputHandler;
	std::unique_ptr<StateMachine> player1StateMachine;
	std::unique_ptr<StateMachine> player2StateMachine;

	Motion *player1Motion;
	Motion *player2Motion;

	CollisionBox *player1CollisionBox;
	CollisionBox *player2CollisionBox;

	void initStateMachines();
	void updatePlayerState(float elapsed_ms);
	static Mix_Music *background_music;
	static Mix_Chunk *punch_sound;
	static Mix_Chunk *walk_sound;
};