#include "../common.hpp"
#include "../renderer.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class WorldSystem {
public:
	WorldSystem();
	void init(GlRender* renderer);
	void handle_collisions();
	~WorldSystem();

	// TODO: Handle the movement and collision
	void handleInput();
	void inputProcessing(int timer);
	void movementProcessing();
	// bool step(float elapsed_ms);
private:
	GlRender* renderer;
};