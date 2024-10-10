#pragma once
#include <string>
#include <gl3w.h>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    unsigned int getProgram() const;
    std::string readShaderFile(const std::string& filePath);
    ~Shader();

private:
    GLuint m_shaderProgram;
    void checkCompileErrors(unsigned int shader, const std::string& type);
};