#include "linearinterp.hpp"

double a = 0;
double a2 = 0;
double b = 0;
double b2 = 0;

int count = 0;
bool isLoading = true;
float count_down_timer = 2.0f; // 2 seconds of timer for the count down sound effect
float time_last_updated = 0.0f;

void resetInterpVariables()
{
	a = 0;
	a2 = 0;
	b = 0;
	b2 = 0;
	count = 0;
	isLoading = true;
	count_down_timer = 2.0f; // Reset the countdown timer when resetting variables
}

void interp_moveEntitesToScreen(GlRender &renderer)
{
	if (isLoading)
	{
		Fighters current_char1 = registry.players.get(renderer.m_player1).current_char;
		Fighters current_char2 = registry.players.get(renderer.m_player2).current_char;
		const FighterConfig &config1 = FighterManager::getFighterConfig(current_char1);
		const FighterConfig &config2 = FighterManager::getFighterConfig(current_char2);

		// stop entity movement when loading
		Motion &m1 = registry.motions.get(renderer.m_player1);
		Motion &m2 = registry.motions.get(renderer.m_player2);

		PlayerInput &p1 = registry.playerInputs.get(renderer.m_player1);
		PlayerInput &p2 = registry.playerInputs.get(renderer.m_player2);
		p1 = PlayerInput();
		p2 = PlayerInput();

		m1.position = {0.2f, (FLOOR_Y + config1.NDC_HEIGHT / 2)};
		m2.position = {0.7f, (FLOOR_Y + config2.NDC_HEIGHT / 2)};

		// Render countdown text
		count_down_timer -= 0.01;
		count = 1;

		// Calculate the center of the screen
		float centerX = M_WINDOW_WIDTH_PX / 2.0f;
		float centerY = M_WINDOW_HEIGHT_PX / 2.0f;
		
		// Render countdown numbers based on timer
		if (count_down_timer > 1.5f) {
			renderer.renderText("3", centerX - 20.f, centerY - 80.f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (count_down_timer > 1.0f) {
			renderer.renderText("2", centerX - 20.f, centerY - 80.f, 1.0f, glm::vec3(1.0f, 0.5f, 0.0f));
		}
		else if (count_down_timer > 0.5f) {
			renderer.renderText("1", centerX - 20.f, centerY - 80.f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		}
		else if (count_down_timer > 0.0f) {
			renderer.renderText("FIGHT!", centerX - 150.f, centerY - 80.f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (count == 1 && count_down_timer <= 0)
		{
			isLoading = false;
		}
	}
}