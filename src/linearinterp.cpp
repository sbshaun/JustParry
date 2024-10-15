#include "linearinterp.hpp"

float a = 0;
float a2 = 0;
int count = 0;
bool isLoading = true;

void interp_moveEntitesToScreen(GlRender& renderer) {
	if (isLoading) {
		Motion& m1 = registry.motions.get(renderer.m_player1);
		if (m1.position[0] < -0.3) {
			m1.position[0] += 0.0089;

		}
		if (m1.position[1] > (FLOOR_Y + NDC_HEIGHT / 2)) {
			m1.position[1] -= 0.003455;

		}

		Motion& m2 = registry.motions.get(renderer.m_player2);
		if (m2.position[0] > 0.3) {
			m2.position[0] -= 0.0089;

		}
		if (m2.position[1] > (FLOOR_Y + NDC_HEIGHT / 2)) {
			m2.position[1] -= 0.003455;

		}

		// spin when you land
		if (m1.position[0] >= -0.3) {
			if (m1.angle > 0) {
				a = m1.angle - 0.05;
				m1.angle = a;

			}
			else if ((m1.angle <= 0) && (a != 0)){
				m1.angle = 0;
				count = 1;				
			}

			if (m2.angle > 0) {
				a2 = m2.angle - 0.05;
				m2.angle = a2;
			}
			else if (m2.angle == 0 && a2 != 0) {
				m2.angle = 0;
			}
			if (count == 1) {
				isLoading = false;
				std::cout << "FINISHED" << std::endl;
			}
		}

	}


	


}