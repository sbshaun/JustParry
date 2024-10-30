#include "linearinterp.hpp"

float a = 0;
float a2 = 0;

float b = 0;
float b2 = 0;

int count = 0;
bool isLoading = true;

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
		Motion &m1 = registry.motions.get(renderer.m_player1);
		Motion &m2 = registry.motions.get(renderer.m_player2);

		PlayerInput &p1 = registry.playerInputs.get(renderer.m_player1);
		PlayerInput &p2 = registry.playerInputs.get(renderer.m_player2);
		p1 = PlayerInput();
		p2 = PlayerInput();

		if (m1.position[0] < -0.5)
		{
			m1.position[0] += 0.0089;
		}
		if (m1.position[1] > (FLOOR_Y + NDC_HEIGHT / 2))
		{
			m1.position[1] -= 0.0047;
		}
		else
		{
			m1.position[1] = (FLOOR_Y + NDC_HEIGHT / 2);
		}

		if (m2.position[0] > 0.5)
		{
			m2.position[0] -= 0.0089;
		}
		if (m2.position[1] > (FLOOR_Y + NDC_HEIGHT / 2))
		{
			m2.position[1] -= 0.0047;
		}
		else
		{
			m2.position[1] = (FLOOR_Y + NDC_HEIGHT / 2);
		}

		if (m1.scale[0] < 1)
		{
			a = m1.scale[0] + 0.01;
			a2 = m1.scale[1] + 0.01;
			m1.scale[0] = a;
			m1.scale[1] = a2;
		}
		else
		{
			m1.scale[0] = 1;
			m1.scale[1] = 1;
		}

		if (m2.scale[0] < 1)
		{
			b = m2.scale[0] + 0.01;
			b2 = m2.scale[1] + 0.01;
			m2.scale[0] = b;
			m2.scale[1] = b2;
		}
		else if ((m2.scale[0] > 1))
		{
			m2.scale[0] = 1;
			m2.scale[1] = 1;
		}

		if (m1.position[0] < 0.3 && m1.position[1] == (FLOOR_Y + NDC_HEIGHT / 2) && m1.scale[0] == 1)
		{
			count = 1;
		}

		if (count == 1)
		{
			isLoading = false;
		}
	}
}