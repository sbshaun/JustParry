#include "mesh.hpp"

Mesh::Mesh(const std::vector<float> &vertices, bool textured)
{
    this->vertices = vertices;
    this->textured = textured;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    gl_has_errors();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (textured)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }
    gl_has_errors();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    gl_has_errors();
}

void Mesh::draw()
{
    glBindVertexArray(VAO);
    gl_has_errors();
    if (this->textured)
    {
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    }
    glBindVertexArray(0);
}

void Mesh::setPosition(const glm::vec3 &pos)
{
    modelMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void Mesh::setRotation(float angle, const glm::vec3 &axis)
{
    modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void Mesh::setScale(const glm::vec3 &scale)
{
    modelMatrix = glm::scale(glm::mat4(1.0f), scale);
}

Mesh::~Mesh()
{
}
