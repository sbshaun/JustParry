#include "mesh.hpp"

Mesh::Mesh(const std::vector<float>& vertices) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error during mesh creation: " << error << std::endl;
    }
    else {
		std::cout << "Mesh created successfully" << std::endl;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw() {
    glBindVertexArray(VAO);
    /*GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error bind vertex array: " << error << std::endl;
    }
    else {
        std::cout << "OpenGL bind vertex array successful" << std::endl;
    }*/
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

Mesh::~Mesh() {}
