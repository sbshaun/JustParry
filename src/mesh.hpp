#pragma once
#include "common.hpp"

class Mesh {
protected:
    GLuint VAO, VBO;
    glm::mat4 modelMatrix;
    bool textured = false;
public:
    std::vector<float> vertices;
    Mesh(const std::vector<float>& vertices, bool textured);
    void draw();
    void setPosition(const glm::vec3& pos);
    void setRotation(float angle, const glm::vec3& axis);
    void setScale(const glm::vec3& scale);
    ~Mesh();
};
