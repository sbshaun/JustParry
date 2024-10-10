#pragma once
#include <common.hpp>

class Mesh {
protected:
    GLuint VAO, VBO;
public:
    Mesh(const std::vector<float>& vertices);
    void draw();
    ~Mesh();
};
