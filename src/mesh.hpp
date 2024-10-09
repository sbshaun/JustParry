#include <vector>
#include <gl3w.h>
#include <iostream>

class Mesh {
protected:
    GLuint VAO, VBO;
public:
    Mesh(const std::vector<float>& vertices);
    void draw();
    ~Mesh();
};
