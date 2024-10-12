#include "../common.hpp"
#include "../renderer.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class WorldSystem {
public:
	WorldSystem();
	void init(GlRender* renderer);
	~WorldSystem();

	// TODO: Handle the movement and collision
	void handleInput();
	void updateMovement();
	// bool step(float elapsed_ms);
	// void handle_collisions();

private:
	GlRender* renderer;
};