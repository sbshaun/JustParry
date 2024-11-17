#include "linearinterp.hpp"

double a = 0;
double a2 = 0;
double b = 0;
double b2 = 0;

int count = 0;
bool isLoading = true;
float count_down_timer = 3.0f; // 3 seconds of timer for the count down sound effect 
float time_last_updated = 0.0f; 

void resetInterpVariables()
{
	a = 0;
	a2 = 0;
	b = 0;
	b2 = 0;
	count = 0;
	isLoading = true;
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

		if (m1.position[0] < -0.5)
		{
			m1.position[0] = m1.position[0] + 0.0089f;
		}
		// move p1 to the ground
		if (m1.position[1] > (FLOOR_Y + config1.NDC_HEIGHT / 2))
		{
			m1.position[1] = m1.position[1] - 0.0047f;
		}
		else
		{
			m1.position[1] = (FLOOR_Y + config1.NDC_HEIGHT / 2);
		}

		if (m2.position[0] > 0.5)
		{
			m2.position[0] = m2.position[0] - 0.0089f;

		} // move p2 to ground
		if (m2.position[1] > (FLOOR_Y + config2.NDC_HEIGHT / 2))
		{
			m2.position[1] = m2.position[1] - 0.0047f;
		}
		else
		{
			m2.position[1] = (FLOOR_Y + config2.NDC_HEIGHT / 2);
		}

		if (m1.scale[0] < 1)
		{
			a = m1.scale[0] + 0.01;
			a2 = m1.scale[1] + 0.01;
			m1.scale = {a, a2};
		}
		else
		{
			m1.scale[0] = 1;
			m1.scale[1] = 1;
			m1.scale[1] = 1;
		}

		if (m2.scale[0] < 1)
		{
			b = m2.scale[0] + 0.01;
			b2 = m2.scale[1] + 0.01;
			m2.scale = {b, b2};
		}
		else if ((m2.scale[0] > 1))
		{
			m2.scale[0] = 1;
			m2.scale[1] = 1;
		}

		// stop once done, and change isLoading to false to avoid inteference w game.
		if (m1.position[0] < 0.3 && m1.position[1] == (FLOOR_Y + config1.NDC_HEIGHT / 2) && m1.scale[0] == 1)
		{
			count = 1;
		}

		count_down_timer -= 0.01; 

		if (count == 1 && count_down_timer <= 0)
		{
			isLoading = false;
		}
	}
}