#include "linearinterp.hpp"

float a = 0;
float a2 = 0;

float b = 0;
float b2 = 0;

int count = 0;
bool isLoading = true;

void interp_moveEntitesToScreen(GlRender& renderer) {
	if (isLoading) {
		// stop entity movement when loading
		Motion& m1 = registry.motions.get(renderer.m_player1);
		Motion& m2 = registry.motions.get(renderer.m_player2);

		PlayerInput& p1 = registry.playerInputs.get(renderer.m_player1);
		PlayerInput& p2 = registry.playerInputs.get(renderer.m_player2);
		p1 = PlayerInput();
		p2 = PlayerInput();

		// move p1 from left to right
		if (m1.position[0] < -0.5) {
			m1.position[0] += 0.0089;
		}
		// move p1 to the ground
		if (m1.position[1] > (FLOOR_Y + NDC_HEIGHT / 2)) {
			m1.position[1] -= 0.00455;
		}


		// move p2 from left to right
		if (m2.position[0] > 0.5) {
			m2.position[0] -= 0.0089;

		} // move p2 to ground
		if (m2.position[1] > (FLOOR_Y + NDC_HEIGHT / 2)) {
			m2.position[1] -= 0.00455;

		}

		// scale p1 from 0.1 to 1
		if (m1.scale[0] < 1) {
			a = m1.scale[0] + 0.01;
			a2 = m1.scale[1] + 0.01;
			m1.scale[0] = a;
			m1.scale[1] = a2;
		} // stop when done, and set scale = 1.
		else if (m1.scale[0] > 1){
			m1.scale[0] = 1;
			m1.scale[1] = 1;
							
		}

		// scale p2 from 0.1 to 1 (not working)
		if (m2.scale[0] < 1) {
			b = m2.scale[0] + 0.01;
			b2 = m2.scale[1] + 0.01;
			m2.scale[0] = b;
			m2.scale[1] = b2;
		} // stop when done, and set scale = 1.
		else if ((m2.scale[0] > 1)) {
			std::cout << m2.scale[1] << std::endl;
			m2.scale[0] = 1;
			m2.scale[1] = 1;

		}

		// stop once done, and change isLoading to false to avoid inteference w game.
		if (m1.position[0] < 0.3 && m1.position[1] < (FLOOR_Y + NDC_HEIGHT / 2) && m1.scale[0] > 1) {
			count = 1;

		}

		// set isLoading to false, when done.
		if (count == 1) {
			isLoading = false;
			std::cout << "FINISHED" << std::endl;
		}

	}


	


}