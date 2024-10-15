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
	void updateStateTimers(float elapsed_ms);
	bool checkHitBoxCollisions(Entity playerWithHitBox, Entity playerWithHurtBox);
	bool checkAABBCollision(const Box& box1, const vec2& position1, bool facingRight1, const Box& box2, const vec2& position2, bool facingRight2);
	void playerCollisions(GlRender* renderer);
	// bool step(float elapsed_ms);
private:
	GlRender* renderer;
};