#include <gl3w.h>
#include <fstream>
#include <sstream>

#include "../external/project_path.hpp"

class GlRender {
private:
	GLuint m_VAO, m_VBO, m_shaderProgram;
	void setupTriangle();
	void loadShaders();
	std::string readShaderFile(const std::string& filename);

public:
	void initialize();
	void render();
	void shutdown();
};