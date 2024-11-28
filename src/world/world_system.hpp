#pragma once
#include "../common.hpp"
#include "../graphics/renderer.hpp"
#include "../input_system/input_handler.hpp"
#include "../input_system/state_machine.hpp"
#include "../settings/settings.hpp"
#include "../settings/default_config.hpp"
#include "../particle_system/particle_system.hpp"
#include "../particle_system/blood_system.hpp"
#include "../particle_system/smoke_system.hpp"
#include "../particle_system/sparkle_system.hpp"

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
			int channel = Mix_PlayChannel(-1, punch_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playKickSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, kick_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playParrySound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, parry_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playCrouchSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, crouch_sound, 0);
    }
  }
	static void playPerfectParrySound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, perfect_parry_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playHurtSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, hurt_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playParryBlockedSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, parry_blocked_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playMenuSelectSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, menu_select_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playMenuConfirmSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, menu_confirm_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}
	static void playGameCountDownSound()
	{
		if (Settings::audioSettings.enable_sound_effects)
		{
			int channel = Mix_PlayChannel(-1, game_count_down_sound, 0);
			Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
		}
	}

	static void playBackgroundMusic()
	{
		if (Settings::audioSettings.enable_music)
		{
			// Stop any existing music first
			Mix_HaltMusic();

			// Playing background music indefinitely
			if (Mix_PlayMusic(background_music, -1) == -1)
			{
				fprintf(stderr, "Failed to play background music: %s\n", Mix_GetError());
			}
			else
			{
				fprintf(stderr, "Background music started\n");
				// Set the correct volume based on settings
				Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.music_volume * Settings::audioSettings.overall_volume));
			}
		}
	}

	static void stopBackgroundMusic()
	{
		Mix_HaltMusic();
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
	void emitSparkleParticles(float x, float y, float z);
	void renderParticles();

	BloodParticleSystem bloodSystem;
	SmokeParticleSystem smokeSystem;
	SparkleParticleSystem sparkleSystem;

	static void updateVolume()
	{
		// Update music volume independently of sound effects
		if (Settings::audioSettings.enable_music)
		{
			Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.music_volume * Settings::audioSettings.overall_volume));
		}
		else
		{
			Mix_VolumeMusic(0);
		}

		// Update sound effects volume separately
		if (Settings::audioSettings.enable_sound_effects)
		{
			// Only update non-music channels
			for (int i = 0; i < Mix_AllocateChannels(-1); i++)
			{
				if (i != WALK_SOUND_CHANNEL) // Skip walk sound channel if needed
				{
					Mix_Volume(i, static_cast<int>(MIX_MAX_VOLUME * Settings::audioSettings.overall_volume));
				}
			}
		}
		else
		{
			// Mute all non-music channels
			for (int i = 0; i < Mix_AllocateChannels(-1); i++)
			{
				Mix_Volume(i, 0);
			}
		}
	}

	static void updateAudioState()
	{
		// Handle music state
		if (!Settings::audioSettings.enable_music)
		{
			stopBackgroundMusic();
		}
		else if (Mix_PlayingMusic() == 0)
		{
			playBackgroundMusic();
		}

		// Always update volumes
		updateVolume();
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

	static Mix_Chunk *parry_sound;
	static Mix_Chunk *perfect_parry_sound;
	static Mix_Chunk *parry_blocked_sound;
	static Mix_Chunk *hurt_sound;
	static Mix_Chunk *kick_sound;
	static Mix_Chunk *menu_select_sound;
	static Mix_Chunk *menu_confirm_sound;
	static Mix_Chunk *game_count_down_sound;
	static Mix_Chunk *crouch_sound;
};